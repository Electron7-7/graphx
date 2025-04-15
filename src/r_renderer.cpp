#include "r_common.hpp"
#include "g_common.hpp"
#include "g_actors.hpp"
#include "sanity.hpp"
#include "t_common.hpp"
#include "graphx_classes_namespace.hpp"
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
#include <filesystem> // Yes, the devil hath been evoked...

std::array<unsigned int, graphx::rendering::VAOS_AMOUNT> VAOs;
std::array<GLShader, graphx::rendering::SHADERS_AMOUNT> shaders;
// Todo: Phase these two booleans out already, fucking hell...
bool time_to_render = false;
bool time_to_store_buffers = false;
// Todo: Make this better or get rid of it
int debug_render_switches = 0;

std::map<std::string, MeshData> mesh_data_storage =
{
	{GRAPHX_CUBE,    MeshData(graphx::rendering::VAO_DEFAULT, CUBE_POSITIONS,    CUBE_NORMALS,    CUBE_UVS,    CUBE_COLORS,    CUBE_INDICES)},
	{GRAPHX_QUAD,    MeshData(graphx::rendering::VAO_DEFAULT, QUAD_POSITIONS,    QUAD_NORMALS,    QUAD_UVS,    QUAD_COLORS,    QUAD_INDICES)},
	{GRAPHX_PYRAMID, MeshData(graphx::rendering::VAO_DEFAULT, PYRAMID_POSITIONS, PYRAMID_NORMALS, PYRAMID_UVS, PYRAMID_COLORS, PYRAMID_INDICES)},
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


GLFWwindow *W_CreateWindow(int width, int height, const char *title, bool make_context_current)
{
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWwindow *new_window = glfwCreateWindow(width, height, title, NULL, NULL);
	
	if(new_window == NULL)
	{
		std::cerr << "[ERROR] Failed to create GLFW window!" << std::endl;
		glfwTerminate();
	}

	if(make_context_current)
		glfwMakeContextCurrent(new_window);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cerr << "[ERROR] Failed to initialize GLAD!" << std::endl;

	return new_window;
}

void W_SwapAndClear(GLFWwindow *w_window, glm::vec4 w_clear_color)
{
	glfwSwapBuffers(w_window);
	glClearColor(w_clear_color[0], w_clear_color[1], w_clear_color[2], w_clear_color[3]);
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

	Texture new_texture(file_string_data.str(), 1600*1600);

	texture_storage[texture_name] = new_texture;

	image_file.close();
	return texture_name;
}

std::string M_LoadModelFile(std::string file_path, std::string file_extension)
{
	if(valid_extensions.find(file_extension) == std::string::npos)
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

MeshData M_LoadOBJ(std::string embedded_obj_file)
{
	MeshData mesh_data;

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
	std::set<std::string> used_texture_names = getCurrentTheatre()->getTextureNames();

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
	std::set<std::string> used_mesh_data_names = getCurrentTheatre()->getMeshDataNames();

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
	return glm::perspective(glm::radians(getCurrentPlayer()->field_of_view), graphx::rendering::main_window_width / graphx::rendering::main_window_height, graphx::rendering::camera_near, graphx::rendering::camera_far);
}

std::vector<RenderCmd> render_commands_buffer;
std::vector<LightRenderCmd> light_render_commands_buffer;
std::vector<TextRenderCmd> text_render_commands_buffer;

FT_Library freetype;
std::map<std::string, Font> font_map;

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

	FT_GlyphSlot glyph_slot = new_face->glyph;

	font_map[font_name] = Font(font_name);

	for(unsigned char character = 0 ; character < 128 ; character++)
	{
		if(FT_Load_Char(new_face, character, FT_LOAD_RENDER))
		{
			PRINTERR("FreeType failed to load glyph (character: " << character << ")")
			continue;
		}

		FT_Render_Glyph(glyph_slot, FT_RENDER_MODE_SDF);

		unsigned int texture_id;
		glGenTextures(1, &texture_id);
		glBindTexture(GL_TEXTURE_2D, texture_id);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, new_face->glyph->bitmap.width, new_face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, new_face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		font_map.at(font_name).character_set[character] = Character(texture_id, new_face->glyph->bitmap.width, new_face->glyph->bitmap.rows, new_face->glyph->bitmap_left, new_face->glyph->bitmap_top, static_cast<int>(new_face->glyph->advance.x));
	}

	FT_Done_Face(new_face);

	glBindVertexArray(VAOs[graphx::rendering::VAO_TEXT]);
	glGenBuffers(1, &font_map.at(font_name).VBO);
	glBindBuffer(GL_ARRAY_BUFFER, font_map.at(font_name).VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(0));
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
}

void R_BufferMeshesAndTextures()
{
	if(loading_new_main_theatre)
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
	if(render_command.isValid())
		render_commands_buffer.insert(render_commands_buffer.end(), render_command);
}

void R_BufferRenderCmd(LightRenderCmd light_render_command)
{
	if(light_render_command.isValid())
		light_render_commands_buffer.insert(light_render_commands_buffer.end(), light_render_command);
}

void R_BufferRenderCmd(TextRenderCmd text_render_command)
{
	if(text_render_command.isValid())
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
	shaders[graphx::rendering::SHADER_SKYBOX].setUniform("skybox_view_matrix", glm::mat4(glm::mat3(getCurrentPlayer()->getViewMatrix())));
	shaders[graphx::rendering::SHADER_SKYBOX].setUniform("skybox_projection_matrix", R_GL_GetProjectionMatrix());
	glBindVertexArray(VAOs[graphx::rendering::VAO_SKYBOX]);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture.texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

void R_GL_RenderFonts()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);

	// Todo: find out if it's worth it to take these out of the for loop
	shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("projection_matrix", R_GL_GetProjectionMatrix());
	shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("view_matrix", getCurrentPlayer()->getViewMatrix());
	shaders[graphx::rendering::SHADER_FONTS_2D].setUniform("projection_matrix", glm::ortho(0.0f, graphx::rendering::main_window_height, 0.0f, graphx::rendering::main_window_width));

	for(auto rendercmd_iterator = text_render_commands_buffer.begin() ; rendercmd_iterator != text_render_commands_buffer.end() ;)
	{
		if(!font_map.contains(rendercmd_iterator->font_name))
		{
			rendercmd_iterator = text_render_commands_buffer.erase(rendercmd_iterator);
			continue;
		}

		if(rendercmd_iterator->is3D())
		{
			glm::mat4 model_matrix = glm::mat4(1.0f);
			model_matrix = glm::translate(model_matrix, rendercmd_iterator->current_render_state->render_position);
			model_matrix *= glm::toMat4(rendercmd_iterator->current_render_state->render_quaternion);
			model_matrix = glm::scale(model_matrix, rendercmd_iterator->current_render_state->render_scale);
			shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("model_matrix", model_matrix);
			shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("text_color", rendercmd_iterator->color);
			glUseProgram(shaders[graphx::rendering::SHADER_FONTS_3D].id);
		}
		else
		{
			shaders[graphx::rendering::SHADER_FONTS_2D].setUniform("text_color", rendercmd_iterator->color);
			glUseProgram(shaders[graphx::rendering::SHADER_FONTS_2D].id);
		}

		Font &font = font_map.at(rendercmd_iterator->font_name);

		for(std::string::const_iterator character_iterator = rendercmd_iterator->text.begin() ; character_iterator != rendercmd_iterator->text.end() ; character_iterator++)
		{
			// https://stackoverflow.com/a/62629272
			Character &character = font.character_set.at(*character_iterator);
			float x_position = rendercmd_iterator->position_x + character.bearing_x * rendercmd_iterator->scale;
			float y_position = rendercmd_iterator->position_y - (character.size_y - character.bearing_y) * rendercmd_iterator->scale;
			float width = character.size_x * rendercmd_iterator->scale;
			float height = character.size_y * rendercmd_iterator->scale;
			float vertices[24] =
			{
				x_position        , y_position + height, 0.0f, 0.0f,
				x_position        , y_position         , 0.0f, 1.0f,
				x_position + width, y_position         , 1.0f, 1.0f,
				x_position        , y_position + height, 0.0f, 0.0f,
				x_position + width, y_position         , 1.0f, 1.0f,
				x_position + width, y_position + height, 1.0f, 0.0f,
			};

			shaders[graphx::rendering::SHADER_FONTS_3D].setUniform("glyph_scale", glm::vec2(width, height));

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
			rendercmd_iterator->position_x += (character.advance >> 6) * rendercmd_iterator->scale; // bitshift by 6 to get value in pixels (2^6 = 64)
		}

		rendercmd_iterator = text_render_commands_buffer.erase(rendercmd_iterator);
	}

	glDepthMask(GL_TRUE);
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

		if(render_command.light_type == graphx::classes::LIGHT)
		{
			which_light = "point_lights[" + std::to_string(point_light_index++) + "].";
		}

		else if(render_command.light_type == graphx::classes::LIGHTDIRECTIONAL)
		{
			which_light = "directional_lights[" + std::to_string(directional_light_index++) + "].";
		}

		else if(render_command.light_type == graphx::classes::LIGHTSPOT)
		{
			which_light = "spot_lights[" + std::to_string(spot_light_index++) + "].";
		}

		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "color", render_command.light_data.color);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "energy", render_command.light_data.energy);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "specular_strength", render_command.light_data.specular_strength);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "ambient_strength", render_command.light_data.ambient_strength);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "attenuation", render_command.light_data.attenuation);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "range", render_command.light_data.range);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "position", render_command.light_data.position);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "direction", render_command.light_data.direction);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "spot_cutoff", render_command.light_data.spot_cutoff);
		shaders[graphx::rendering::SHADER_DEFAULT].setUniform(which_light + "spot_cutoff_fade", render_command.light_data.spot_cutoff_fade);

		rendercmd_iterator = light_render_commands_buffer.erase(rendercmd_iterator);
	}
}

void R_GL_Render(std::mutex &state_mutex, float interpolation_time)
{
	if(loading_new_main_theatre)
		return;

	glBindVertexArray(VAOs[graphx::rendering::VAO_DEFAULT]);

	R_GL_RenderLights(state_mutex, interpolation_time);

	shaders[graphx::rendering::SHADER_DEFAULT].setUniform("enable_ambient", static_cast<int>(graphx::rendering::lighting_switch_ambient));
	shaders[graphx::rendering::SHADER_DEFAULT].setUniform("enable_diffuse", static_cast<int>(graphx::rendering::lighting_switch_diffuse));
	shaders[graphx::rendering::SHADER_DEFAULT].setUniform("enable_specular", static_cast<int>(graphx::rendering::lighting_switch_specular));
	shaders[graphx::rendering::SHADER_DEFAULT].setUniform("point_lights_count", getCurrentTheatre()->point_lights_count);
	shaders[graphx::rendering::SHADER_DEFAULT].setUniform("spot_lights_count", getCurrentTheatre()->spot_lights_count);
	shaders[graphx::rendering::SHADER_DEFAULT].setUniform("directional_lights_count", getCurrentTheatre()->directional_lights_count);

	for(auto rendercmd_iterator = render_commands_buffer.begin() ; rendercmd_iterator != render_commands_buffer.end() ;)
	{
		if(!enable_default_shader || !rendercmd_iterator->isValid())
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

		if(rendercmd_iterator->is_light_debug_mesh || rendercmd_iterator->mesh_material.mat_fullbright)
			graphx::rendering::current_shader = graphx::rendering::SHADER_DEBUG_FULLBRIGHT;

		glUseProgram(shaders[graphx::rendering::current_shader].id);

		glm::mat4 model_matrix = glm::mat4(1.0f);

		std::lock_guard guard(state_mutex);

		// Todo: maybe change RenderState pointers to just copy the de-referenced RenderState pointers? (bc I don't like that R_GL_Render is accessing an Actor pointer)
		RenderState *current_state		=	rendercmd_iterator->current_render_state;
		RenderState *previous_state		=	rendercmd_iterator->previous_render_state;

		glm::vec3 interpolated_position	=	current_state->render_position;
		glm::vec3 interpolated_scale	=	current_state->render_scale;
		glm::quat interpolated_quat		=	current_state->render_quaternion;

		if(graphx::rendering::do_interpolation) // Eventually, I want to change interpolation to be more like GZDoom, and this will be how I test that
		{
			for(unsigned int i = 0 ; i < 3 ; i++)
				interpolated_position[i] = std::lerp(previous_state->render_position[i], current_state->render_position[i], interpolation_time);

			interpolated_quat = glm::slerp(previous_state->render_quaternion, current_state->render_quaternion, interpolation_time);

			for(unsigned int i = 0 ; i < 3 ; i++)
				interpolated_scale[i] = std::lerp(previous_state->render_scale[i], current_state->render_scale[i], interpolation_time);
		}

		model_matrix = glm::translate(model_matrix, interpolated_position);
		model_matrix *= glm::toMat4(interpolated_quat);
		model_matrix = glm::scale(model_matrix, interpolated_scale);

		glBindTextureUnit(0, texture_storage.at(rendercmd_iterator->mesh_material.diffuse_texture_name).texture_id);
		glBindTextureUnit(1, texture_storage.at(rendercmd_iterator->mesh_material.specular_texture_name).texture_id);

		shaders[graphx::rendering::current_shader].setUniform("model_matrix", model_matrix);
		shaders[graphx::rendering::current_shader].setUniform("view_matrix", getCurrentPlayer()->getViewMatrix());
		shaders[graphx::rendering::current_shader].setUniform("projection_matrix", R_GL_GetProjectionMatrix());
		shaders[graphx::rendering::current_shader].setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(model_matrix))));
		shaders[graphx::rendering::current_shader].setUniform("view_position", getCurrentPlayer()->getViewPosition());
		shaders[graphx::rendering::current_shader].setUniform("current_material.texture_diffuse", 0);
		shaders[graphx::rendering::current_shader].setUniform("current_material.texture_specular", 1);
		shaders[graphx::rendering::current_shader].setUniform("current_material.diffuse_color", rendercmd_iterator->mesh_material.color);
		shaders[graphx::rendering::current_shader].setUniform("current_material.specular_sharpness", rendercmd_iterator->mesh_material.specular_sharpness);
		shaders[graphx::rendering::current_shader].setUniform("current_material.specular_strength", rendercmd_iterator->mesh_material.specular_strength);
		shaders[graphx::rendering::current_shader].setUniform("current_environment.ambient_light_contribution", getCurrentEnvironment()->ambient_light_amount);
		shaders[graphx::rendering::current_shader].setUniform("current_environment.ambient_light_color", getCurrentEnvironment()->ambient_light_color);

		MeshData &mesh_data = mesh_data_storage.at(rendercmd_iterator->mesh_data_name);
		glDrawElementsBaseVertex(GL_TRIANGLES, mesh_data.indices_count(), GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * mesh_data.base_index), mesh_data.base_vertex);

		rendercmd_iterator = render_commands_buffer.erase(rendercmd_iterator);
	}

	R_BufferRenderCmd(TextRenderCmd("Tr2n", "Fucking WHAT", 0, 100, 1.0f, glm::vec3(0.2f, 0.5f, 1.0f)));

	R_GL_RenderSkybox();
	R_GL_RenderFonts();
}

void R_Render(std::mutex &state_mutex, float interpolation_time)
{
	if(loading_new_main_theatre)
		return;

	getCurrentTheatre()->probeActorsForRenderCommands();

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

	shaders[graphx::rendering::SHADER_DEFAULT] = GLShader(blinn_phong_vert, blinn_phong_frag, "SHADER_DEFAULT");
	shaders[graphx::rendering::SHADER_FONTS_2D] = GLShader(font2d_vert, font2d_frag, "SHADER_FONTS_2D");
	shaders[graphx::rendering::SHADER_FONTS_3D] = GLShader(font3d_vert, font3d_frag, "SHADER_FONTS_3D");
	shaders[graphx::rendering::SHADER_SKYBOX] = GLShader(skybox_vert, skybox_frag, "SHADER_SKYBOX");
	shaders[graphx::rendering::SHADER_DEBUG_FULLBRIGHT] = GLShader(blinn_phong_vert, light_debug_frag, "SHADER_DEBUG_FULLBRIGHT");
	shaders[graphx::rendering::SHADER_DEBUG_NORMALS] = GLShader(blinn_phong_vert, debug_normals_frag, "SHADER_DEBUG_NORMALS");
	shaders[graphx::rendering::SHADER_DEBUG_VERTEX_COLORS] = GLShader(blinn_phong_vert, debug_vertex_colors_frag, "SHADER_DEBUG_VERTEX_COLORS");
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
