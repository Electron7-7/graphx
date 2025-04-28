#include "r_rendering.hpp"
#include "g_devices.hpp"
#include "g_theatre.hpp"
#include "g_actors.hpp"
#include "sanity.hpp"
#include "t_common.hpp"
#include "t_interpreter.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#define TINYOBJLOADER_DONOT_INCLUDE_MAPBOX_EARCUT
#define TINYOBJLOADER_USE_DOUBLE
#include <earcut.hpp>
#include <tiny_obj_loader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <cmath>
#include <filesystem> // Yes, the devil hath been invoked...

std::array<unsigned int, graphx::rendering::VAOS_AMOUNT> VAOs;
std::array<GLShader, graphx::rendering::SHADERS_AMOUNT> shaders;
// Todo: Phase these two booleans out already, fucking hell...
bool time_to_render = false;
bool time_to_store_buffers = false;
// Todo: Make this better or get rid of it
int debug_render_switches = 0;

//---------
// GLShader
//---------
GLShader::GLShader(std::string vertex_shader_code, std::string fragment_shader_code)
{
	const char *v_shader_code = vertex_shader_code.c_str();
	const char *f_shader_code = fragment_shader_code.c_str();

	unsigned int vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_shader_code, nullptr);
	glCompileShader(vertex);
	GLShaderErrorHandler(vertex);

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_shader_code, nullptr);
	glCompileShader(fragment);
	GLShaderErrorHandler(fragment);

	id = glCreateProgram();
	glAttachShader(id, vertex);
	glAttachShader(id, fragment);
	glLinkProgram(id);
	GLShaderErrorHandler(id, true);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void GLShader::GLShaderErrorHandler(const unsigned int& shader_id, const bool is_program_linking)
{
	// https://stackoverflow.com/a/63420289
	int v_result = GL_FALSE;
	int info_log_length;
	std::string shader_error_type = "Shader Compilation";
	if(is_program_linking)
	{
		shader_error_type = "Program Linking";
		glGetProgramiv(shader_id, GL_LINK_STATUS, &v_result);
		glGetProgramiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	}
	else
	{
		glGetShaderiv(shader_id, GL_COMPILE_STATUS, &v_result);
		glGetShaderiv(shader_id, GL_INFO_LOG_LENGTH, &info_log_length);
	}
	if(info_log_length > 0)
	{
		std::vector<char> shader_error_message(info_log_length + 1);
		if(is_program_linking)
			glGetProgramInfoLog(shader_id, info_log_length, nullptr, shader_error_message.data());
		else
			glGetShaderInfoLog(shader_id, info_log_length, nullptr, shader_error_message.data());

		PRINTERR("GLSL " << shader_error_type << " Error(s):\n" << shader_error_message.data())
	}
}

template<> void GLShader::setUniform<bool>(const std::string &name, bool value) const
{ glProgramUniform1i(id, glGetUniformLocation(id, name.c_str()), static_cast<int>(value)); }

template<> void GLShader::setUniform<int>(const std::string &name, int value) const
{ glProgramUniform1i(id, glGetUniformLocation(id, name.c_str()), value); }

template<> void GLShader::setUniform<float>(const std::string &name, float value) const
{ glProgramUniform1f(id, glGetUniformLocation(id, name.c_str()), value); }

template<> void GLShader::setUniform<glm::vec2>(const std::string &name, glm::vec2 value) const
{ glProgramUniform2fv(id, glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value)); }

template<> void GLShader::setUniform<glm::vec3>(const std::string &name, glm::vec3 value) const
{ glProgramUniform3fv(id, glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value)); }

template<> void GLShader::setUniform<glm::vec4>(const std::string &name, glm::vec4 value) const
{ glProgramUniform4fv(id, glGetUniformLocation(id, name.c_str()), 1, glm::value_ptr(value)); }

template<> void GLShader::setUniform<glm::mat3>(const std::string &name, glm::mat3 value) const
{ glProgramUniformMatrix3fv(id, glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }

template<> void GLShader::setUniform<glm::mat4>(const std::string &name, glm::mat4 value) const
{ glProgramUniformMatrix4fv(id, glGetUniformLocation(id, name.c_str()), 1, GL_FALSE, glm::value_ptr(value)); }

template<> void GLShader::setUniform<LightsCount>(const std::string &name, LightsCount value) const
{
	glProgramUniform1i(id, glGetUniformLocation(id, std::string(name + ".point_lights").c_str()), value.point_lights);
	glProgramUniform1i(id, glGetUniformLocation(id, std::string(name + ".spot_lights").c_str()), value.spot_lights);
	glProgramUniform1i(id, glGetUniformLocation(id, std::string(name + ".directional_lights").c_str()), value.directional_lights);
}

//
// Mesh
//
Mesh::Mesh()
{}

Mesh::Mesh(int init_vao_index, std::vector<glm::vec3> init_positions, std::vector<glm::vec3> init_normals, std::vector<glm::vec2> init_uvs, std::vector<glm::vec3> init_colors, std::vector<gmath::vec3uint> init_indices)
{
	// These for loops make sure that every vertex has normal, uv, and color data
	for(int i = init_normals.size() ; i < init_positions.size() ; i++)
		init_normals.insert(init_normals.end(), glm::vec3(0.0f, 0.0f, 0.0f));

	for(int i = init_uvs.size() ; i < init_positions.size() ; i++)
		init_uvs.insert(init_uvs.end(), glm::vec2(0.0f, 0.0f));

	for(int i = init_colors.size() ; i < init_positions.size() ; i++)
		init_colors.insert(init_colors.end(), glm::vec3(1.0f, 1.0f, 1.0f));

	vertex_positions = init_positions;
	vertex_normals = init_normals;
	vertex_uvs = init_uvs;
	vertex_colors = init_colors;

	if(init_indices.empty())
		for(int i = 0 ; i < vertex_positions.size() * 3; i += 3)
			vertex_indices.insert(vertex_indices.end(), gmath::vec3uint(i, i+1, i+2));
	else
		vertex_indices = init_indices;
}

Mesh::Mesh(int init_vao_index, std::vector<float> init_positions, std::vector<float> init_normals, std::vector<float> init_uvs, std::vector<float> init_colors, std::vector<unsigned int> init_indices)
{
	// These for loops make sure that every vertex has normal, uv, and color data
	for(int i = init_normals.size() ; i < init_positions.size() ; i++)
		init_normals.insert(init_normals.end(), 0.0f);

	for(int i = init_uvs.size() ; i < (2 * init_positions.size() / 3) ; i++)
		init_uvs.insert(init_uvs.end(), 0.0f);

	for(int i = init_colors.size() ; i < init_positions.size() ; i++)
		init_colors.insert(init_colors.end(), 1.0f);

	if(!init_indices.empty())
		for(int i = init_indices.size() ; i < init_positions.size() ; i++)
			init_colors.insert(init_colors.end(), 1.0f);

	for(int it = 0,uv_it = 0 ; it < init_positions.size() ; it += 3,uv_it += 2)
	{
		vertex_positions.insert(vertex_positions.end(), glm::vec3(init_positions[it], init_positions[it + 1], init_positions[it + 2]));
		vertex_normals.insert(vertex_normals.end(), glm::vec3(init_normals[it], init_normals[it + 1], init_normals[it + 2]));
		vertex_uvs.insert(vertex_uvs.end(), glm::vec2(init_uvs[uv_it], init_uvs[uv_it + 1]));
		vertex_colors.insert(vertex_colors.end(), glm::vec3(init_colors[it], init_colors[it + 1], init_colors[it + 2]));
		if(!init_indices.empty())
			vertex_indices.insert(vertex_indices.end(), gmath::vec3uint(init_indices[it], init_indices[it + 1], init_indices[it + 2]));
	}

	if(init_indices.empty())
		for(int i = 0 ; i < vertex_positions.size() * 3; i += 3)
			vertex_indices.insert(vertex_indices.end(), gmath::vec3uint(i, i+1, i+2));
}

void Mesh::addVertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uv, glm::vec3 color)
{
	vertex_positions.insert(vertex_positions.end(), position);
	vertex_normals.insert(vertex_normals.end(), normal);
	vertex_uvs.insert(vertex_uvs.end(), uv);
	vertex_colors.insert(vertex_colors.end(), color);
}

void Mesh::addVertex(float position_x, float position_y, float position_z, float normal_x, float normal_y, float normal_z, float uv_x, float uv_y, float color_x, float color_y, float color_z)
{
	vertex_positions.insert(vertex_positions.end(), glm::vec3(position_x, position_y, position_z));
	vertex_normals.insert(vertex_normals.end(), glm::vec3(normal_x, normal_y, normal_z));
	vertex_uvs.insert(vertex_uvs.end(), glm::vec2(uv_x, uv_y));
	vertex_colors.insert(vertex_colors.end(), glm::vec3(color_x, color_y, color_z));
}

void Mesh::addVertex(std::vector<float> vertex)
{
	vertex_positions.insert(vertex_positions.end(), glm::vec3(vertex[0], vertex[1], vertex[2]));
	vertex_normals.insert(vertex_normals.end(), glm::vec3(vertex[3], vertex[4], vertex[5]));
	vertex_uvs.insert(vertex_uvs.end(), glm::vec2(vertex[6], vertex[7]));
	vertex_colors.insert(vertex_colors.end(), glm::vec3(vertex[8], vertex[9], vertex[10]));
}

void Mesh::addIndex(gmath::vec3uint indices)
{
	vertex_indices.insert(vertex_indices.end(), indices);
}

void Mesh::addIndex(unsigned int index_1, unsigned int index_2, unsigned int index_3)
{
	vertex_indices.insert(vertex_indices.end(), gmath::vec3uint(index_1, index_2, index_3));
}

void Mesh::fixOBJData()
{
	float max_coordinate = 0.0f;
	float min_coordinate = 0.0f;

	for(glm::vec3 vertex : vertex_positions)
	{
		if(std::abs(vertex.x) > 1 || std::abs(vertex.y) > 1 || std::abs(vertex.z) > 1)
		{
			float biggest_pos = glm::compMax(vertex);
			float smallest_pos = glm::compMin(vertex);
			if(biggest_pos > max_coordinate)
				max_coordinate = biggest_pos;
			if(smallest_pos < min_coordinate)
				min_coordinate = smallest_pos;
		}
	}

	for(glm::vec3 vertex : vertex_positions)
	{
		for(float component : vertex)
		{
			component = (component - min_coordinate) / (max_coordinate - min_coordinate);
		}
	}

	vertex_indices.clear();
	for(int i = 0 ; i < vertex_positions.size(); i += 3)
	{
		vertex_indices.insert(vertex_indices.end(), gmath::vec3uint(i, i + 1, i + 2));
	}
}

const std::vector<float> Mesh::vertices()
{
	std::vector<float> vertices;
	for(int i = 0 ; i < vertex_positions.size() ; i++)
	{
		vertices.insert(vertices.end(),
		{
			vertex_positions.at(i).x,
			vertex_positions.at(i).y,
			vertex_positions.at(i).z,
			vertex_normals.at(i).x,
			vertex_normals.at(i).y,
			vertex_normals.at(i).z,
			vertex_uvs.at(i).x,
			vertex_uvs.at(i).y,
			vertex_colors.at(i).x,
			vertex_colors.at(i).y,
			vertex_colors.at(i).z
		});
	}
	return vertices;
}

const std::vector<unsigned int> Mesh::indices()
{
	std::vector<unsigned int> indices;
	for(int i = 0 ; i < vertex_indices.size() ; i++)
	{
		indices.insert(indices.end(),
		{
			vertex_indices.at(i).x(),
			vertex_indices.at(i).y(),
			vertex_indices.at(i).z()
		});
	}
	return indices;
}

size_t Mesh::vertices_count()
{
	return (vertex_positions.size());
}

size_t Mesh::vertices_size()
{
	return
	(
		(3 * sizeof(float) * vertex_positions.size()) +
		(3 * sizeof(float) * vertex_normals.size())   +
		(2 * sizeof(float) * vertex_uvs.size())       +
		(3 * sizeof(float) * vertex_colors.size())
	);
}

size_t Mesh::indices_count()
{
	return (vertex_indices.size() * 3);
}

size_t Mesh::indices_size()
{
	return (3 * sizeof(unsigned int) * vertex_indices.size());
}

//
// Character
//
Character::Character(unsigned int init_texture_id, int init_size_x, int init_size_y, int init_bearing_x, int init_bearing_y, int init_advance)
{
	texture_id = init_texture_id;
	size_x = init_size_x;
	size_y = init_size_y;
	bearing_x = init_bearing_x;
	bearing_y = init_bearing_y;
	advance = init_advance;
}

Character::Character(unsigned int init_texture_id, glm::vec2 init_size, glm::vec2 init_bearing, int init_advance)
: Character(init_texture_id, init_size.x, init_size.y, init_bearing.x, init_bearing.y, init_advance)
{}

//
// Font
//
Font::Font(std::string init_font_name)
: font_name(init_font_name)
{}

//
// LightRenderCmd
//
LightRenderCmd::LightRenderCmd(const unsigned int my_light_type)
: light_type(my_light_type)
{}

//
// TextRenderCmd
//
TextRenderCmd::TextRenderCmd(std::string init_text, std::string init_font_name, float init_position_x, float init_position_y, float init_scale, glm::vec3 init_color)
: text(init_text), position_x(init_position_x), position_y(init_position_y), scale(init_scale), color(init_color), is_debug_label(false), render_state(nullptr), font_name(init_font_name)
{}

void TextRenderCmd::setFontName(const std::string& new_font_name)
{ font_name = new_font_name; }

std::string TextRenderCmd::getFontName() const
{ return (font_storage.contains(font_name)) ? font_name : "Arial"; }

bool TextRenderCmd::is3D() const
{   // TODO: FIND A WAY TO REMOVE THIS
	return (render_state != nullptr);
}

std::map<std::string, Mesh> mesh_data_storage =
{
	{GRAPHX_CUBE,    Mesh(graphx::rendering::VAO_DEFAULT, CUBE_POSITIONS,    CUBE_NORMALS,    CUBE_UVS,    CUBE_COLORS,    CUBE_INDICES)},
	{GRAPHX_QUAD,    Mesh(graphx::rendering::VAO_DEFAULT, QUAD_POSITIONS,    QUAD_NORMALS,    QUAD_UVS,    QUAD_COLORS,    QUAD_INDICES)},
	{GRAPHX_PYRAMID, Mesh(graphx::rendering::VAO_DEFAULT, PYRAMID_POSITIONS, PYRAMID_NORMALS, PYRAMID_UVS, PYRAMID_COLORS, PYRAMID_INDICES)},
	{ERROR_MODEL,    M_LoadOBJ(ERROR_obj)},
	{suzanne_MODEL,  M_LoadOBJ(suzanne_obj)},
	{ramiel_MODEL,   M_LoadOBJ(ramiel_obj)},
	{purely_for_testing_MODEL, M_LoadOBJ(purely_for_testing_obj)},
};

std::map<std::string, Texture> texture_storage =
{
	{COMP04_5, Texture(COMP04_5_png, COMP04_5_png_len)},
	{COMP04_5_SPECULAR, Texture(COMP04_5_SPECULAR_jpg, COMP04_5_SPECULAR_jpg_len)},
	{FLAT_SPEC, Texture(FLAT_SPEC_jpg, FLAT_SPEC_jpg_len)},
	{LIGHT_DEBUGGING, Texture(LIGHT_DEBUGGING_jpg, LIGHT_DEBUGGING_jpg_len)},
	{MISSING_TEXTURE, Texture(MISSING_TEXTURE_jpg, MISSING_TEXTURE_jpg_len)},
	{debug_checkers, Texture(debug_checkers_png, debug_checkers_png_len)},
	{NO_TEXTURE, Texture(NO_TEXTURE_jpg, NO_TEXTURE_jpg_len)},
	{SOURCE_LIGHT_GREY, Texture(SOURCE_LIGHT_GREY_png, SOURCE_LIGHT_GREY_png_len)},
	{SOURCE_ORANGE, Texture(SOURCE_ORANGE_png, SOURCE_ORANGE_png_len)},
};

GLFWwindow* W_CreateWindow(int width, int height, const char *title, bool make_context_current)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow* new_window = glfwCreateWindow(width, height, title, NULL, NULL);
	
	if(new_window == NULL)
	{
		PRINTERR("Failed to create GLFW window!")
		glfwTerminate();
	}

	if(make_context_current)
		glfwMakeContextCurrent(new_window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		PRINTERR("Failed to initialize GLAD!")

	return new_window;
}

void W_SwapAndClear(GLFWwindow* window, glm::vec4 clear_color)
{
	glfwSwapBuffers(window);
	glClearColor(clear_color[0], clear_color[1], clear_color[2], clear_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

std::string T_LoadImageFile(std::string file_path)
{
	std::string binary_path = BINARY_PATH;

	// If the file path is relative, it should be relative to the program's location.
	std::string file_path_checked = std::string(binary_path + file_path);

	// If the file path is absolute, use it as is.
	if(file_path.starts_with('/') || !file_path.substr(1, 2).compare(":/"))
		file_path_checked = file_path;

	std::string texture_name = std::filesystem::path(file_path_checked).stem().string();

	if(texture_storage.contains(texture_name))
		return texture_name;

	std::ifstream image_file = std::ifstream(file_path_checked);
	std::stringstream file_string_data;
	file_string_data << image_file.rdbuf();

	if(!image_file.is_open())
	{
		PRINTERR("T_LoadImageFile(std::string file_path) - Image file unable to be read / does not exist!")
		return MISSING_TEXTURE;
	}

	int image_x;
	int image_y;
	int image_channels;
	stbi_info(file_path_checked.c_str(), &image_x, &image_y, &image_channels);

	Texture new_texture(reinterpret_cast<unsigned char*>(const_cast<char*>(file_string_data.str().c_str())), 1600*1600); // Todo: THIS IS VERY FUCKING DANGEROUS

	texture_storage[texture_name] = new_texture;

	image_file.close();
	return texture_name;
}

std::string M_LoadModelFile(std::string file_path, std::string file_extension)
{
	if(graphx::Interpreter.validExtensions().find(file_extension) == std::string::npos)
	{
		PRINTERR("M_LoadModelFile called with an unsupported file type! An error mesh will be returned!")
		return ERROR_MODEL;
	}

	// Last minute realization that I had to move this out of the header file "sanity.hpp"
	std::string binary_path = BINARY_PATH;

	// If the file path is relative, it should be relative to the program's location.
	std::string file_path_checked = std::string(binary_path + file_path);

	// If the file path is absolute, use it as is.
	if(file_path.starts_with('/') || !file_path.substr(1, 2).compare(":/"))
		file_path_checked = file_path;

	std::ifstream model_file = std::ifstream(file_path_checked);
	std::stringstream file_string_data;
	file_string_data << model_file.rdbuf();

	if(!model_file.is_open())
	{
		PRINTERR("M_LoadModelFile called but the file \"" << (binary_path + file_path) << "\" could not be opened/found! An error mesh will be returned!")
		return ERROR_MODEL;
	}

	model_file.close();

	if(!file_extension.compare("obj"))
	{
		std::string mesh_data_name = "";
		unsigned long i = file_string_data.str().find("\no ") + 3;
		while(file_string_data.str()[i] != '\n')
			mesh_data_name += file_string_data.str()[i++];
		if(!mesh_data_name.empty() && !mesh_data_storage.contains(mesh_data_name))
			mesh_data_storage[mesh_data_name] = M_LoadOBJ(file_string_data.str());
		return mesh_data_name;
	}

	PRINTERR("M_LoadModelFile called with an unsupported file type! An error mesh will be returned!")
	return ERROR_MODEL;
}

Mesh M_LoadOBJ(std::string embedded_obj_file)
{
	Mesh mesh_data;

	tinyobj::ObjReaderConfig reader_config;
	tinyobj::ObjReader reader;

	if(!reader.ParseFromString(embedded_obj_file, "", reader_config))
		if(!reader.Error().empty())
			PRINTERR("TinyObjReader Error - " << reader.Error())

	if (!reader.Warning().empty())
		PRINTDEBUG("TinyObjReader Warning - " + reader.Warning());

	auto &attrib = reader.GetAttrib();
	auto &shapes = reader.GetShapes();

	// Loop over shapes
	// Shapes are full meshes in the OBJ
	for (size_t s = 0; s < shapes.size(); s++)
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
				std::vector<float> vertex;

				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
				tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
				tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

				vertex.insert(vertex.end(), {(float)vx, (float)vy, (float)vz});

				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0)
				{
					tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
					tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
					tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];

					vertex.insert(vertex.end(), {(float)nx, (float)ny, (float)nz});
				}

				else
				{
					vertex.insert(vertex.end(), {0.0f, 0.0f, -1.0f});
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0)
				{
					tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
					tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];

					vertex.insert(vertex.end(), {(float)tx, (float)ty});
				}

				else
				{
					vertex.insert(vertex.end(), {0.0f, 0.0f});
				}

				if (idx.texcoord_index >= 0)
				{
					tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
					vertex.insert(vertex.end(), {(float)red, (float)green, (float)blue});
				}

				else
				{
					vertex.insert(vertex.end(), {1.0f, 1.0f, 1.0f});
				}

				mesh_data.addVertex(vertex);
			}

			index_offset += fv;
		}
	}

	mesh_data.fixOBJData();
	return mesh_data;
}

void R_GL_BufferTextures()
{
	std::set<std::string> used_texture_names = graphx::current::theatre.getTextureNames();

	for(auto &texture_pair : texture_storage)
	{
		if(used_texture_names.contains(texture_pair.first))
			texture_pair.second.is_in_use = true;
		else
			texture_pair.second.is_in_use = false;
	}

	for(auto &texture_pair : texture_storage)
	{
		if(!texture_pair.second.is_in_use)
		{
			if(texture_pair.second.texture_id != 0)
				glDeleteBuffers(1, &texture_pair.second.texture_id);
			continue;
		}

		if(texture_pair.second.texture_id != 0)
			return;

		stbi_set_flip_vertically_on_load(true); // Obviously, automate this to flip relevant textures (when Y-Axis 0.0 is not on the bottom of the image)

		glGenTextures(1, &texture_pair.second.texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_pair.second.texture_id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		int t_width, t_height, t_channels;
		unsigned char *t_data = stbi_load_from_memory(texture_pair.second.texture_data[0], texture_pair.second.texture_size[0], &t_width, &t_height, &t_channels, STBI_rgb);

		if(!t_data)
			PRINTERR("Failed to load texture!");

		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, t_data);
		glGenerateMipmap(GL_TEXTURE_2D);
		stbi_image_free(t_data);
	}
}

unsigned int VBO;
unsigned int IBO;

void R_GL_BufferMeshes()
{
	std::set<std::string> used_mesh_data_names = graphx::current::theatre.getMeshDataNames();

	std::vector<float> all_vertices;
	std::vector<unsigned int> all_indices;

	glBindVertexArray(VAOs[graphx::rendering::VAO_DEFAULT]);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	for(auto &mesh_data_pair : mesh_data_storage)
	{
		mesh_data_pair.second.debug_name = mesh_data_pair.first; // Debugging shit

		if(!used_mesh_data_names.contains(mesh_data_pair.first))
			continue;

		std::vector<float> vertices = mesh_data_pair.second.vertices();
		std::vector<unsigned int> indices = mesh_data_pair.second.indices();

		mesh_data_pair.second.base_vertex = all_vertices.size() / 11;
		mesh_data_pair.second.base_index = all_indices.size();

		all_vertices.insert(all_vertices.end(), vertices.begin(), vertices.end());
		all_indices.insert(all_indices.end(), indices.begin(), indices.end());
	}

	glBufferData(GL_ARRAY_BUFFER, all_vertices.size() * sizeof(float), all_vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, all_indices.size() * sizeof(unsigned int), all_indices.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);
}

glm::mat4 R_GL_GetProjectionMatrix()
{
	return glm::perspective(glm::radians(graphx::current::player->field_of_view), graphx::rendering::main_window_width / graphx::rendering::main_window_height, graphx::rendering::camera_near, graphx::rendering::camera_far);
}

std::vector<RenderCmd> render_commands_buffer;
std::vector<LightRenderCmd> light_render_commands_buffer;
std::vector<TextRenderCmd> text_render_commands_buffer;

FT_Library freetype;
std::map<std::string, Font> font_storage;

void F_InitializeFreeType()
{
	if(FT_Init_FreeType(&freetype))
		PRINTERR("FreeType library failed to initialize!")
}

void F_LoadFont(std::string ttf_file_path, std::string font_name)
{
	FT_Face new_face;
	if(FT_New_Face(freetype, ttf_file_path.c_str(), 0, &new_face))
	{
		PRINTERR("FreeType failed to load font face (filepath: " << ttf_file_path << ")")
		return;
	}

	FT_Set_Pixel_Sizes(new_face, 0, 48);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	font_storage[font_name] = Font(font_name);

	for(unsigned char character = 0 ; character < 128 ; character++)
	{
		if(FT_Load_Char(new_face, character, FT_LOAD_RENDER))
		{
			PRINTERR("FreeType failed to load glyph (character: " << character << ")")
			continue;
		}

		FT_GlyphSlot glyph_slot = new_face->glyph;
		FT_Render_Glyph(glyph_slot, FT_RENDER_MODE_SDF);

		unsigned int texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, glyph_slot->bitmap.width, glyph_slot->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, glyph_slot->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		font_storage.at(font_name).character_set[character] = Character(texture_id, glyph_slot->bitmap.width, glyph_slot->bitmap.rows, glyph_slot->bitmap_left, glyph_slot->bitmap_top, static_cast<int>(glyph_slot->advance.x));
	}

	FT_Done_Face(new_face);

	glBindVertexArray(VAOs[graphx::rendering::VAO_TEXT]);
	glGenBuffers(1, &font_storage.at(font_name).VBO);
	glBindBuffer(GL_ARRAY_BUFFER, font_storage.at(font_name).VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void R_BufferMeshesAndTextures()
{
	if(graphx::state::loading_new_main_theatre)
		return;

	switch(graphx::rendering::graphx_api)
	{
	case graphx::rendering::GRAPHX_OPENGL:
		R_GL_BufferMeshes();
		R_GL_BufferTextures();
		break;
	}

	time_to_store_buffers = false;
	time_to_render = true;
}

void R_BufferRenderCommands(RenderCommands render_commands)
{
	R_BufferRenderCmd(render_commands.render_command);
	R_BufferRenderCmd(render_commands.light_render_command);
	R_BufferRenderCmd(render_commands.text_render_command);
}

void R_BufferRenderCmd(RenderCmd render_command)
{
	render_commands_buffer.insert(render_commands_buffer.end(), render_command);
}

void R_BufferRenderCmd(LightRenderCmd light_render_command)
{
	light_render_commands_buffer.insert(light_render_commands_buffer.end(), light_render_command);
}

void R_BufferRenderCmd(TextRenderCmd text_render_command)
{
	text_render_commands_buffer.insert(text_render_commands_buffer.end(), text_render_command);
}

bool enable_default_shader = true; // Todo: delete this, lmfao

void R_GL_RenderSkybox()
{ // Todo: get rid of static variables and make this function less terrible
	static unsigned int background_vbo = 0;

	static Texture skybox_texture(
	{
		SHIT_SKYBOX_XPOS_png,
		SHIT_SKYBOX_XNEG_png,
		SHIT_SKYBOX_YPOS_png,
		SHIT_SKYBOX_YNEG_png,
		SHIT_SKYBOX_ZPOS_png,
		SHIT_SKYBOX_ZNEG_png,
	},
	{
		SHIT_SKYBOX_XPOS_png_len,
		SHIT_SKYBOX_XNEG_png_len,
		SHIT_SKYBOX_YPOS_png_len,
		SHIT_SKYBOX_YNEG_png_len,
		SHIT_SKYBOX_ZPOS_png_len,
		SHIT_SKYBOX_ZNEG_png_len,
	});

	if(skybox_texture.texture_id == 0)
	{
		glGenTextures(1, &skybox_texture.texture_id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture.texture_id);

		for(int i = 0 ; i < 6 ; i++)
		{
			stbi_set_flip_vertically_on_load(true);

			int t_width, t_height, t_channels;
			unsigned char *t_data = stbi_load_from_memory(skybox_texture.texture_data[i], skybox_texture.texture_size[i], &t_width, &t_height, &t_channels, STBI_rgb);

			if(!t_data)
				PRINTERR("Failed to load texture!");

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_SRGB_ALPHA, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, t_data);

			stbi_image_free(t_data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_ANISOTROPY, 16);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	}

	if(background_vbo == 0)
	{
		float skybox_vertices[] =
		{
		    -1.0f,  1.0f, -1.0f,
		    -1.0f, -1.0f, -1.0f,
		     1.0f, -1.0f, -1.0f,
		     1.0f, -1.0f, -1.0f,
		     1.0f,  1.0f, -1.0f,
		    -1.0f,  1.0f, -1.0f,

		    -1.0f, -1.0f,  1.0f,
		    -1.0f, -1.0f, -1.0f,
		    -1.0f,  1.0f, -1.0f,
		    -1.0f,  1.0f, -1.0f,
		    -1.0f,  1.0f,  1.0f,
		    -1.0f, -1.0f,  1.0f,

		     1.0f, -1.0f, -1.0f,
		     1.0f, -1.0f,  1.0f,
		     1.0f,  1.0f,  1.0f,
		     1.0f,  1.0f,  1.0f,
		     1.0f,  1.0f, -1.0f,
		     1.0f, -1.0f, -1.0f,

		    -1.0f, -1.0f,  1.0f,
		    -1.0f,  1.0f,  1.0f,
		     1.0f,  1.0f,  1.0f,
		     1.0f,  1.0f,  1.0f,
		     1.0f, -1.0f,  1.0f,
		    -1.0f, -1.0f,  1.0f,

		    -1.0f,  1.0f, -1.0f,
		     1.0f,  1.0f, -1.0f,
		     1.0f,  1.0f,  1.0f,
		     1.0f,  1.0f,  1.0f,
		    -1.0f,  1.0f,  1.0f,
		    -1.0f,  1.0f, -1.0f,

		    -1.0f, -1.0f, -1.0f,
		    -1.0f, -1.0f,  1.0f,
		     1.0f, -1.0f, -1.0f,
		     1.0f, -1.0f, -1.0f,
		    -1.0f, -1.0f,  1.0f,
		     1.0f, -1.0f,  1.0f
		};

		glGenBuffers(1, &background_vbo);

		glBindVertexArray(VAOs[graphx::rendering::VAO_SKYBOX]);
		glBindBuffer(GL_ARRAY_BUFFER, background_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glDepthFunc(GL_LEQUAL);
	glUseProgram(shaders[graphx::rendering::SHADER_SKYBOX].id);
	shaders[graphx::rendering::SHADER_SKYBOX].setUniform("skybox_view_matrix", glm::mat4(glm::mat3(graphx::current::player->getViewMatrix())));
	shaders[graphx::rendering::SHADER_SKYBOX].setUniform("skybox_projection_matrix", R_GL_GetProjectionMatrix());
	glBindVertexArray(VAOs[graphx::rendering::VAO_SKYBOX]);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture.texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void R_GL_RenderFont(TextRenderCmd& render_command)
{
	Font &font = font_storage.at(render_command.getFontName());
	float render_command_x_position = render_command.position_x;
	float render_command_y_position = render_command.position_y;
	for(std::string::const_iterator character_iterator = render_command.text.begin() ; character_iterator != render_command.text.end() ; character_iterator++)
	{
		// https://stackoverflow.com/a/62629272
		Character &character = font.character_set.at(*character_iterator);
		float x_position = render_command.position_x + character.bearing_x * render_command.scale;
		float y_position = render_command.position_y - (character.size_y - character.bearing_y) * render_command.scale;
		float width = character.size_x * render_command.scale;
		float height = character.size_y * render_command.scale;

		if(*character_iterator == '\n')
		{
			render_command.position_y -= character.size_y * render_command.scale;
			render_command.position_x = render_command_x_position;
			continue;
		}

		float vertices[24] =
		{
			x_position        , y_position + height, 0.0f, 0.0f,
			x_position        , y_position         , 0.0f, 1.0f,
			x_position + width, y_position         , 1.0f, 1.0f,
			x_position        , y_position + height, 0.0f, 0.0f,
			x_position + width, y_position         , 1.0f, 1.0f,
			x_position + width, y_position + height, 1.0f, 0.0f,
		};

		glBindVertexArray(VAOs[graphx::rendering::VAO_TEXT]);
		glBindTextureUnit(0, character.texture_id);
		glBindBuffer(GL_ARRAY_BUFFER, font.VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// Advance cursors for next glyph
		render_command.position_x += (character.advance >> 6) * render_command.scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}

	render_command.position_x = render_command_x_position;
	render_command.position_y = render_command_y_position;
}

void R_GL_RenderFonts()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_CULL_FACE);

	// Todo: find out if it's worth it to take these out of the for loop
	shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("projection_matrix", R_GL_GetProjectionMatrix());
	shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("ortho_matrix", glm::ortho(0.0f, graphx::rendering::main_window_height, 0.0f, graphx::rendering::main_window_width));
	shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("view_matrix", graphx::current::player->getViewMatrix());
	shaders[graphx::rendering::SHADER_FONTS_2D].setUniform("ortho_matrix", glm::ortho(0.0f, graphx::rendering::main_window_height, 0.0f, graphx::rendering::main_window_width));

	for(auto rendercmd_iterator = text_render_commands_buffer.begin() ; rendercmd_iterator != text_render_commands_buffer.end() ;)
	{
		if(!font_storage.contains(rendercmd_iterator->getFontName()))
		{
			rendercmd_iterator = text_render_commands_buffer.erase(rendercmd_iterator);
			continue;
		}

		if(rendercmd_iterator->is3D())
		{
			glm::mat4 model_matrix = glm::mat4(1.0f);
			model_matrix = glm::translate(model_matrix, rendercmd_iterator->render_state->render_position);
			if(!rendercmd_iterator->is_debug_label) // Keep debug labels from following Actor rotation
				model_matrix *= glm::toMat4(rendercmd_iterator->render_state->render_quaternion);
			shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("model_matrix", model_matrix);
			shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("text_color", rendercmd_iterator->color);
			shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("text_scale", rendercmd_iterator->scale);
			glUseProgram(shaders[graphx::rendering::SHADER_FONTS_3D].id);
		}
		else
		{
			shaders[graphx::rendering::SHADER_FONTS_2D].setUniform("text_color", rendercmd_iterator->color);
			glUseProgram(shaders[graphx::rendering::SHADER_FONTS_2D].id);
		}

		if(rendercmd_iterator->is_debug_label)
		{
			shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("z_offset", 0.01f);
			shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("is_debug", true);
			R_GL_RenderFont(*rendercmd_iterator);
			glClear(GL_DEPTH_BUFFER_BIT);
			shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("z_offset", 0.0f);
			shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("is_debug", false);
		}

		R_GL_RenderFont(*rendercmd_iterator);

		rendercmd_iterator = text_render_commands_buffer.erase(rendercmd_iterator);
	}

	glDisable(GL_BLEND);
}

void R_GL_RenderLights(std::mutex &state_mutex, float interpolation_time)
{
	int point_light_index = 0;
	int spot_light_index = 0;
	int directional_light_index = 0;

	for(auto rendercmd_iterator = light_render_commands_buffer.begin() ; rendercmd_iterator != light_render_commands_buffer.end() ;)
	{
		LightRenderCmd render_command = *rendercmd_iterator.base();
		std::string which_light;

		if(render_command.light_type == LightRenderCmd::POINT_LIGHT)
			which_light = "point_lights[" + std::to_string(point_light_index++) + "].";

		else if(render_command.light_type == LightRenderCmd::DIRECTIONAL_LIGHT)
			which_light = "directional_lights[" + std::to_string(directional_light_index++) + "].";

		else if(render_command.light_type == LightRenderCmd::SPOT_LIGHT)
			which_light = "spot_lights[" + std::to_string(spot_light_index++) + "].";

		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "color", render_command.color);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "energy", render_command.energy);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "specular_strength", render_command.specular_strength);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "ambient_strength", render_command.ambient_strength);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "attenuation", render_command.attenuation);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "range", render_command.range);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "position", render_command.position);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "direction", render_command.direction);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "spot_cutoff", render_command.spot_cutoff);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "spot_cutoff_fade", render_command.spot_cutoff_fade);

		rendercmd_iterator = light_render_commands_buffer.erase(rendercmd_iterator);
	}
}

void R_GL_Render(std::mutex &state_mutex, float interpolation_time)
{
	if(graphx::state::loading_new_main_theatre)
		return;

	glBindVertexArray(VAOs[graphx::rendering::VAO_DEFAULT]);

	R_GL_RenderLights(state_mutex, interpolation_time);

	shaders[graphx::rendering::SHADER_DEFAULT].setUniform("enable_ambient", static_cast<int>(graphx::rendering::lighting_switch_ambient));
	shaders[graphx::rendering::SHADER_DEFAULT].setUniform("enable_diffuse", static_cast<int>(graphx::rendering::lighting_switch_diffuse));
	shaders[graphx::rendering::SHADER_DEFAULT].setUniform("enable_specular", static_cast<int>(graphx::rendering::lighting_switch_specular));
	shaders[graphx::rendering::SHADER_DEFAULT].setUniform("lights_count", graphx::current::theatre.getLightsCount());

	glEnable(GL_BLEND);

	for(auto rendercmd_iterator = render_commands_buffer.begin() ; rendercmd_iterator != render_commands_buffer.end() ;)
	{
		if(!enable_default_shader)
		{
			rendercmd_iterator = render_commands_buffer.erase(rendercmd_iterator);
			continue;
		}

		// Todo: make this bullshit better or remove it
		switch(debug_render_switches)
		{
		case USE_FULLBRIGHT:
			graphx::rendering::current_shader = graphx::rendering::SHADER_DEBUG_FULLBRIGHT;
			break;
		case USE_NORMALS:
			graphx::rendering::current_shader = graphx::rendering::SHADER_DEBUG_NORMALS;
			break;
		case USE_VERTEX_COLORS:
			graphx::rendering::current_shader = graphx::rendering::SHADER_DEBUG_VERTEX_COLORS;
			break;
		default:
			graphx::rendering::current_shader = graphx::rendering::SHADER_DEFAULT;
			break;
		}

		// When I change RenderCmd::mesh_material to be an id instead of a pointer, I can remove this shitty nullptr check
		if(rendercmd_iterator->is_light_debug_mesh || (rendercmd_iterator->mesh_material != nullptr && rendercmd_iterator->mesh_material->mat_fullbright))
			graphx::rendering::current_shader = graphx::rendering::SHADER_DEBUG_FULLBRIGHT;

		glUseProgram(shaders[graphx::rendering::current_shader].id);

		glm::mat4 model_matrix = glm::mat4(1.0f);

		std::lock_guard guard(state_mutex);

		// Todo: maybe change RenderState pointers to just copy the de-referenced RenderState pointers? (bc I don't like that R_GL_Render is accessing an Actor pointer)
		RenderState current_state		=	rendercmd_iterator->current_render_state;
		RenderState previous_state		=	rendercmd_iterator->previous_render_state;

		glm::vec3 interpolated_position	=	current_state.render_position;
		glm::vec3 interpolated_scale	=	current_state.render_scale;
		glm::quat interpolated_quat		=	current_state.render_quaternion;

		if(graphx::rendering::do_interpolation) // Eventually, I want to change interpolation to be more like GZDoom, and this will be how I test that
		{
			for(unsigned int i = 0 ; i < 3 ; i++)
				interpolated_position[i] = std::lerp(previous_state.render_position[i], current_state.render_position[i], interpolation_time);

			interpolated_quat = glm::slerp(previous_state.render_quaternion, current_state.render_quaternion, interpolation_time);

			for(unsigned int i = 0 ; i < 3 ; i++)
				interpolated_scale[i] = std::lerp(previous_state.render_scale[i], current_state.render_scale[i], interpolation_time);
		}

		model_matrix = glm::translate(model_matrix, interpolated_position);
		model_matrix *= glm::toMat4(interpolated_quat);
		model_matrix = glm::scale(model_matrix, interpolated_scale);


		// When I change RenderCmd::mesh_material to be an id instead of a pointer, I can remove this shitty nullptr check
		if(rendercmd_iterator->mesh_material != nullptr)
		{
			glBindTextureUnit(0, texture_storage.at(rendercmd_iterator->mesh_material->diffuse_texture_name).texture_id);
			glBindTextureUnit(1, texture_storage.at(rendercmd_iterator->mesh_material->specular_texture_name).texture_id);
			shaders[graphx::rendering::current_shader].setUniform("current_material.texture_diffuse", 0);
			shaders[graphx::rendering::current_shader].setUniform("current_material.texture_specular", 1);
			shaders[graphx::rendering::current_shader].setUniform("current_material.diffuse_color", rendercmd_iterator->mesh_material->color);
			shaders[graphx::rendering::current_shader].setUniform("current_material.alpha", rendercmd_iterator->mesh_material->color_alpha);
			shaders[graphx::rendering::current_shader].setUniform("current_material.specular_sharpness", rendercmd_iterator->mesh_material->specular_sharpness);
			shaders[graphx::rendering::current_shader].setUniform("current_material.specular_strength", rendercmd_iterator->mesh_material->specular_strength);
		}
		shaders[graphx::rendering::current_shader].setUniform("model_matrix", model_matrix);
		shaders[graphx::rendering::current_shader].setUniform("view_matrix", graphx::current::player->getViewMatrix());
		shaders[graphx::rendering::current_shader].setUniform("projection_matrix", R_GL_GetProjectionMatrix());
		shaders[graphx::rendering::current_shader].setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(model_matrix))));
		shaders[graphx::rendering::current_shader].setUniform("view_position", graphx::current::player->getViewPosition());
		shaders[graphx::rendering::current_shader].setUniform("current_environment.ambient_light_contribution", graphx::current::environment->ambient_light_amount);
		shaders[graphx::rendering::current_shader].setUniform("current_environment.ambient_light_color", graphx::current::environment->ambient_light_color);

		shaders[graphx::rendering::current_shader].setUniform("debug_highlight", rendercmd_iterator->debug_highlight_color);

		Mesh &mesh_data = mesh_data_storage.at(rendercmd_iterator->mesh_data_name);
		glDrawElementsBaseVertex(GL_TRIANGLES, mesh_data.indices_count(), GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * mesh_data.base_index), mesh_data.base_vertex);

		rendercmd_iterator = render_commands_buffer.erase(rendercmd_iterator);
	}

	R_BufferRenderCmd(TextRenderCmd("Tr2n", "Fucking WHAT", 0, 100, 1.0f, glm::vec3(0.2f, 0.5f, 1.0f)));

	R_GL_RenderSkybox();
	R_GL_RenderFonts();
}

void R_Render(std::mutex &state_mutex, float interpolation_time)
{
	if(graphx::state::loading_new_main_theatre)
		return;

	graphx::current::theatre.probeRenderCommands();

	switch(graphx::rendering::graphx_api)
	{
	case graphx::rendering::GRAPHX_OPENGL:
		R_GL_Render(state_mutex, interpolation_time);
		break;
	}
}

void R_GL_Initialize()
{
	glGenVertexArrays(graphx::rendering::VAOS_AMOUNT, VAOs.data());

	shaders[graphx::rendering::SHADER_DEFAULT] = GLShader(blinn_phong_vert, blinn_phong_frag);
	shaders[graphx::rendering::SHADER_FONTS_2D] = GLShader(font2d_vert, font2d_frag);
	shaders[graphx::rendering::SHADER_FONTS_3D] = GLShader(font3d_vert, font3d_frag);
	shaders[graphx::rendering::SHADER_SKYBOX] = GLShader(skybox_vert, skybox_frag);
	shaders[graphx::rendering::SHADER_DEBUG_FULLBRIGHT] = GLShader(blinn_phong_vert, light_debug_frag);
	shaders[graphx::rendering::SHADER_DEBUG_NORMALS] = GLShader(blinn_phong_vert, debug_normals_frag);
	shaders[graphx::rendering::SHADER_DEBUG_VERTEX_COLORS] = GLShader(blinn_phong_vert, debug_vertex_colors_frag);
}

void R_InitializeRenderingAPI()
{
	switch(graphx::rendering::graphx_api)
	{
	case graphx::rendering::GRAPHX_OPENGL:
		R_GL_Initialize();
		break;
	}
}