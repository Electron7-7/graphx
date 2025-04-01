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
#include <cmath>

std::array<unsigned int, graphx::rendering::VAOS_AMOUNT> VAOs;
std::vector<GLShader *> shaders;
bool time_to_render = false;
bool time_to_store_buffers = false;

std::map<std::string, MeshData> mesh_data_storage =
{
	{GRAPHX_CUBE, MeshData(graphx::rendering::VAO_DEFAULT, CUBE_POSITIONS, CUBE_NORMALS, CUBE_UVS, CUBE_COLORS, CUBE_INDICES)},
	{GRAPHX_PYRAMID, MeshData(graphx::rendering::VAO_DEFAULT, PYRAMID_POSITIONS, PYRAMID_NORMALS, PYRAMID_UVS, PYRAMID_COLORS, PYRAMID_INDICES)},
	{GRAPHX_QUAD, MeshData(graphx::rendering::VAO_DEFAULT, QUAD_POSITIONS, QUAD_NORMALS, QUAD_UVS, QUAD_COLORS, QUAD_INDICES)},
	{ERROR_MODEL, M_LoadOBJ(ERROR_obj)},
	{suzanne_MODEL, M_LoadOBJ(suzanne_obj)},
	{ramiel_MODEL, M_LoadOBJ(ramiel_obj)},
	{purely_for_testing_MODEL, M_LoadOBJ(purely_for_testing_obj)},
};

std::map<std::string, Texture> texture_storage =
{
	{COMP04_5, Texture(COMP04_5_png, COMP04_5_png_len)},
	{COMP04_5_SPECULAR, Texture(COMP04_5_SPECULAR_jpg, COMP04_5_SPECULAR_jpg_len)},
	{FLAT_SPEC, Texture(FLAT_SPEC_jpg, FLAT_SPEC_jpg_len)},
	{LIGHT_DEBUGGING, Texture(LIGHT_DEBUGGING_jpg, LIGHT_DEBUGGING_jpg_len)},
	{MISSING_TEXTURE, Texture(MISSING_TEXTURE_jpg, MISSING_TEXTURE_jpg_len)},
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

	std::string texture_name = std::filesystem::path(file_path_checked).stem();

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

unsigned int PrimitivesVBO;
unsigned int VBO;
unsigned int IBO;

void R_GL_BufferMeshes()
{
	//------------//
	// Primitives //
	//------------//
	std::vector<float> primitive_verts = 
	{
		-1.0f, 0.0f, -1.0f,
		 1.0f, 0.0f, -1.0f,
		 0.0f, 3.0f, -1.0f
	};

	glBindVertexArray(VAOs[graphx::rendering::VAO_PRIMITIVES]);
	glDeleteBuffers(1, &PrimitivesVBO);
	glGenBuffers(1, &PrimitivesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, PrimitivesVBO);
	glBufferData(GL_ARRAY_BUFFER, primitive_verts.size() * sizeof(float), primitive_verts.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//-------------------//
	// ...Not Primitives //
	//-------------------//
	std::set<std::string> used_mesh_data_names = getCurrentTheatre()->getMeshDataNames();

	long vertex_buffer_size = 0; // The start of the VBO's empty store, in bytes
	long index_buffer_size = 0;  // The start of the IBO's empty store, in bytes
	unsigned int number_of_vertices = 0;
	unsigned int number_of_indices = 0;

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

		all_vertices.insert(all_vertices.end(), vertices.begin(), vertices.end());
		all_indices.insert(all_indices.end(), indices.begin(), indices.end());

		mesh_data_pair.second.base_vertex = number_of_vertices;
		mesh_data_pair.second.base_index = number_of_indices;

		number_of_vertices += mesh_data_pair.second.vertices_count();
		vertex_buffer_size += mesh_data_pair.second.vertices_size();
		number_of_indices += mesh_data_pair.second.indices_count();
		index_buffer_size += mesh_data_pair.second.indices_size();
	}

	glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size, all_vertices.data(), GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_buffer_size, all_indices.data(), GL_STATIC_DRAW);

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

void R_GL_DrawSkybox()
{
	static unsigned int background_vao = 0;
	static unsigned int background_vbo = 0;

	static GLShader skybox_shader(skybox_vert, skybox_frag);
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

	if(background_vao == 0)
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

		glGenVertexArrays(1, &background_vao);
		glGenBuffers(1, &background_vbo);

		glBindVertexArray(background_vao);
		glBindBuffer(GL_ARRAY_BUFFER, background_vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(0));

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	glDepthFunc(GL_LEQUAL);
	glUseProgram(skybox_shader.id);
	skybox_shader.setUniform("skybox_view_matrix", glm::mat4(glm::mat3(getCurrentPlayer()->getViewMatrix())));
	skybox_shader.setUniform("skybox_projection_matrix", R_GL_GetProjectionMatrix());
	glBindVertexArray(background_vao);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skybox_texture.texture_id);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

std::vector<RenderCmd> render_commands;
std::vector<LightRenderCmd> light_render_commands;
std::vector<PrimitiveRenderCmd> primitive_render_commands;

void R_DrawPrimitive(PrimitiveRenderCmd primitive)
{
	if(loading_new_main_theatre)
		return;

	glBindVertexArray(VAOs[graphx::rendering::VAO_PRIMITIVES]);

	glUseProgram(shaders[2]->id);
	shaders[2]->setUniform("vertex_position[0]", primitive.vertex_1);
	shaders[2]->setUniform("vertex_position[1]", primitive.vertex_2);
	shaders[2]->setUniform("vertex_color[0]", primitive.colors_1);
	shaders[2]->setUniform("vertex_color[1]", primitive.colors_2);

	if(primitive.primitive_type == graphx::identifiers::primitive::LINE)
		glDrawArrays(GL_LINE, 0, 2);

	else if(primitive.primitive_type == graphx::identifiers::primitive::TRIANGLE)
	{
		shaders[2]->setUniform("vertex_position[2]", primitive.vertex_3);
		shaders[2]->setUniform("vertex_color[2]", primitive.colors_3);
		glDrawArrays(GL_TRIANGLES, 0, 3);
	}
}

void R_GL_RenderLights(std::mutex &state_mutex, float interpolation_time)
{
	int point_light_index = 0;
	int spot_light_index = 0;

	glUseProgram(shaders[graphx::rendering::shader_index]->id);

	for(auto rendercmd_iterator = light_render_commands.begin() ; rendercmd_iterator != light_render_commands.end() ;)
	{
		LightRenderCmd render_command = *rendercmd_iterator.base();
		std::string which_light;

		if(render_command.light_type == graphx::classes::LIGHT)
		{
			which_light = "point_lights[" + std::to_string(point_light_index++) + "].";
		}

		else if(render_command.light_type == graphx::classes::LIGHTDIRECTIONAL)
		{
			which_light = "directional_light.";
			shaders[graphx::rendering::shader_index]->setUniform(which_light + "direction", render_command.light_data->direction);
		}

		else if(render_command.light_type == graphx::classes::LIGHTSPOT)
		{
			which_light = "spot_lights[" + std::to_string(spot_light_index++) + "].";
			shaders[graphx::rendering::shader_index]->setUniform(which_light + "inner_cutoff", render_command.light_data->inner_cutoff);
			shaders[graphx::rendering::shader_index]->setUniform(which_light + "outer_cutoff", render_command.light_data->outer_cutoff);
			shaders[graphx::rendering::shader_index]->setUniform(which_light + "direction", render_command.light_data->direction);
		}

		shaders[graphx::rendering::shader_index]->setUniform(which_light + "strength", render_command.light_data->strength);
		shaders[graphx::rendering::shader_index]->setUniform(which_light + "diffuse", render_command.light_data->color);
		shaders[graphx::rendering::shader_index]->setUniform(which_light + "specular", render_command.light_data->color);
		shaders[graphx::rendering::shader_index]->setUniform(which_light + "position", render_command.light_data->position);
		shaders[graphx::rendering::shader_index]->setUniform(which_light + "range", render_command.light_data->range);
		shaders[graphx::rendering::shader_index]->setUniform(which_light + "intensity", render_command.light_data->intensity);
		shaders[graphx::rendering::shader_index]->setUniform(which_light + "falloff", render_command.light_data->falloff);

		if(render_command.renderDebugMesh())
		{
			R_BufferRenderCmd(RenderCmd(render_command, (render_command.light_data->color * render_command.light_data->strength)));
		}

		rendercmd_iterator = light_render_commands.erase(rendercmd_iterator);
	}
}

void R_GL_Render(std::mutex &state_mutex, float interpolation_time, JPH::DebugRenderer *debug_renderer)
{
	if(loading_new_main_theatre)
		return;

	glBindVertexArray(VAOs[graphx::rendering::VAO_DEFAULT]);

	glUseProgram(shaders[graphx::rendering::shader_index]->id);
	shaders[graphx::rendering::shader_index]->setUniform("shader_debug_value", graphx::rendering::shader_debug_value);
	shaders[graphx::rendering::shader_index]->setUniform("enable_diffuse", static_cast<float>(graphx::rendering::lighting_switch_diffuse));
	shaders[graphx::rendering::shader_index]->setUniform("enable_specular", static_cast<float>(graphx::rendering::lighting_switch_specular));
	shaders[graphx::rendering::shader_index]->setUniform("point_lights_count", getCurrentTheatre()->point_lights_count);
	shaders[graphx::rendering::shader_index]->setUniform("spot_lights_count", getCurrentTheatre()->spot_lights_count);
	shaders[graphx::rendering::shader_index]->setUniform("directional_lights_count", getCurrentTheatre()->directional_lights_count);

	R_GL_RenderLights(state_mutex, interpolation_time);

	for(auto rendercmd_iterator = render_commands.begin() ; rendercmd_iterator != render_commands.end() ;)
	{
		RenderCmd render_command = *rendercmd_iterator.base();

		if(!render_command.isRenderable())
		{
			rendercmd_iterator = render_commands.erase(rendercmd_iterator);
			continue;
		}

		glm::mat4 model_matrix = glm::mat4(1.0f);

		std::lock_guard guard(state_mutex);

		RenderState *current_state		=	render_command.current_render_state;
		RenderState *previous_state		=	render_command.previous_render_state;

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

		MeshData mesh_data = mesh_data_storage.at(render_command.mesh_data_name);

		glBindTextureUnit(0, texture_storage.at(render_command.mesh_material.diffuse_texture_name).texture_id);
		glBindTextureUnit(1, texture_storage.at(render_command.mesh_material.specular_texture_name).texture_id);

		shaders[graphx::rendering::shader_index]->setUniform("model_matrix", model_matrix);
		shaders[graphx::rendering::shader_index]->setUniform("view_matrix", getCurrentPlayer()->getViewMatrix());
		shaders[graphx::rendering::shader_index]->setUniform("projection_matrix", R_GL_GetProjectionMatrix());
		shaders[graphx::rendering::shader_index]->setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(model_matrix))));
		shaders[graphx::rendering::shader_index]->setUniform("view_position", getCurrentPlayer()->getViewPosition());

		shaders[graphx::rendering::shader_index]->setUniform("is_light", render_command.is_light_debug_mesh);
		shaders[graphx::rendering::shader_index]->setUniform("is_primitive", false);
		shaders[graphx::rendering::shader_index]->setUniform("material.texture_diffuse", 0);
		shaders[graphx::rendering::shader_index]->setUniform("material.texture_specular", 1);
		shaders[graphx::rendering::shader_index]->setUniform("material.color", render_command.mesh_material.color);
		shaders[graphx::rendering::shader_index]->setUniform("material.specular_sharpness", render_command.mesh_material.specular_sharpness);
		shaders[graphx::rendering::shader_index]->setUniform("material.specular_strength", render_command.mesh_material.specular_strength);
		shaders[graphx::rendering::shader_index]->setUniform("mat_fullbright", render_command.mesh_material.mat_fullbright);
		// shaders[graphx::rendering::shader_index]->setUniform("environment.ambient_strength", getCurrentEnvironment()->ambient_light_strength * getCurrentEnvironment()->ambient_lighting_enabled);

		glDrawElementsBaseVertex(GL_TRIANGLES, mesh_data.indices_count(), GL_UNSIGNED_INT, (void *)(sizeof(unsigned int) * mesh_data.base_index), mesh_data.base_vertex);

		rendercmd_iterator = render_commands.erase(rendercmd_iterator);
	}

	R_GL_DrawSkybox();

	// R_DrawPrimitive(PrimitiveRenderCmd(glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(-3.0f, 1.0f, 0.0f), glm::vec3(3.0f, 1.0f, 0.0f), glm::vec3(1.0f, 0.5f, 1.0f)));
	// JPH::BodyManager::DrawSettings jolt_draw_settings;
	// jolt_draw_settings.mDrawShape = true;
	// jolt_draw_settings.mDrawShapeWireframe = true;
	// jolt_draw_settings.mDrawBoundingBox = false;
	// jolt_physics_system.DrawBodies(jolt_draw_settings, debug_renderer);
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

void R_BufferRenderCmd(RenderCmd render_command)
{
	render_commands.insert(render_commands.end(), render_command);
}

void R_BufferRenderCmd(LightRenderCmd light_render_command)
{
	light_render_commands.insert(light_render_commands.end(), light_render_command);
}

void R_BufferRenderCmd(PrimitiveRenderCmd primitive_render_command)
{
	primitive_render_commands.insert(primitive_render_commands.end(), primitive_render_command);
}

void R_Render(std::mutex &state_mutex, float interpolation_time, JPH::DebugRenderer *debug_renderer)
{
	if(loading_new_main_theatre)
		return;

	getCurrentTheatre()->probeActorsForRenderCommands();

	switch(graphx::rendering::graphx_api)
	{
	case graphx::rendering::GRAPHX_OPENGL:
		R_GL_Render(state_mutex, interpolation_time, debug_renderer);
		break;
	}
}

void R_GL_Initialize()
{
	glGenVertexArrays(graphx::rendering::VAOS_AMOUNT, &VAOs[graphx::rendering::VAO_DEFAULT]);
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
