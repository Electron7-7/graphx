#include "r_common.hpp"
#include "g_common.hpp"
#include "g_actors.hpp"
#include "t_common.hpp"
#include "graphx_namespace.hpp"
#define TINYOBJLOADER_IMPLEMENTATION
#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#define TINYOBJLOADER_DONOT_INCLUDE_MAPBOX_EARCUT
#define TINYOBJLOADER_USE_DOUBLE
#include <earcut.hpp>
#include <tiny_obj_loader.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <cmath>

std::array<unsigned int, VAOS_AMOUNT> VAOs;
std::vector<GLShader *> shaders;
bool time_to_render = false;
bool time_to_store_buffers = false;
bool do_interpolation = true; // For testing when I change the interpolation method to be more like GZDoom
int graphx_api = 0;
int shader_debug_value = 4;
unsigned int shader_index = SHADER_BLINN_PHONG;
glm::vec2 main_window_size(1280.0f, 720.0f);
float camera_near = 0.1f;
float camera_far = 1000.0f;
bool jolt_debug_render = false;
bool lighting_switch_diffuse = true;
bool lighting_switch_specular = true;
bool lighting_switch_ambient = true;

std::map<std::string, MeshData> mesh_data_storage =
{
	{GRAPHX_CUBE, MeshData(VAO_DEFAULT, CUBE_POSITIONS, CUBE_NORMALS, CUBE_UVS, CUBE_COLORS, CUBE_INDICES)},
	{GRAPHX_PYRAMID, MeshData(VAO_DEFAULT, PYRAMID_POSITIONS, PYRAMID_NORMALS, PYRAMID_UVS, PYRAMID_COLORS, PYRAMID_INDICES)},
	{GRAPHX_QUAD, MeshData(VAO_DEFAULT, QUAD_POSITIONS, QUAD_NORMALS, QUAD_UVS, QUAD_COLORS, QUAD_INDICES)},
	{M_GetOBJName(ERROR_obj), M_LoadOBJ(ERROR_obj)},
	{M_GetOBJName(suzanne_obj), M_LoadOBJ(suzanne_obj)},
	{M_GetOBJName(purely_for_testing_obj), M_LoadOBJ(purely_for_testing_obj)},
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

void W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color)
{
	glfwSwapBuffers(w_window);
	glClearColor(w_clear_color[0], w_clear_color[1], w_clear_color[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

std::string M_GetOBJName(std::string file_as_string)
{
	std::string buffer = "";
	unsigned long i = file_as_string.find("\no ") + 3;
		while(file_as_string[i] != '\n')
			buffer += file_as_string[i++];
	return buffer;
}

std::string M_LoadModelFile(std::string file_path, std::string file_extension)
{
	std::string mesh_data_name = M_GetOBJName(ERROR_obj);

	if(valid_extensions.find(file_extension) == std::string::npos)
	{
		PRINTERR("M_LoadModelFile called with an unsupported file type! An error mesh will be returned!")
		return mesh_data_name;
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
		return mesh_data_name;
	}

	model_file.close();

	if(!file_extension.compare("obj"))
	{
		mesh_data_name = M_GetOBJName(file_string_data.str());
		if(!mesh_data_name.empty() && !mesh_data_storage.contains(mesh_data_name))
			mesh_data_storage[mesh_data_name] = M_LoadOBJ(file_string_data.str());
	}

	PRINTERR("M_LoadModelFile called with an unsupported file type! An error mesh will be returned!")
	return mesh_data_name;
}

MeshData M_LoadOBJ(std::string embedded_obj_file)
{
	MeshData mesh_data;

	tinyobj::ObjReaderConfig reader_config;
	tinyobj::ObjReader reader;

	if(!reader.ParseFromString(embedded_obj_file, "", reader_config))
		if(!reader.Error().empty())
			PRINTERR("TinyObjReader: " << reader.Error())

	// if (!reader.Warning().empty())
		// PRINTDEBUG("TinyObjReader: " + reader.Warning());

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
					vertex.insert(vertex.end(), {0.0f, 0.0f, 0.0f});
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

void M_GL_BufferMaterialTexture(unsigned int &texture_id, unsigned char *texture_buffer)
{
	if(texture_id != 0)
		return;

	stbi_set_flip_vertically_on_load(true); // Obviously, automate this to flip relevant textures (when Y-Axis 0.0 is not on the bottom of the image)

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 16);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int t_width, t_height, t_channels;
	unsigned char *t_data = stbi_load_from_memory(texture_buffer, 1600*1600, &t_width, &t_height, &t_channels, STBI_rgb);

	if(!t_data)
		PRINTERR("Failed to load texture!");

	// glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, t_data);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, t_width, t_height, 0, GL_RGB, GL_UNSIGNED_BYTE, t_data);
	glGenerateMipmap(GL_TEXTURE_2D);
	stbi_image_free(t_data);
}

// Temporary
/*unsigned int primitives_VBO;
#define NUMBER_OF_PRIMITIVES_IN_ONE_VBO 500*/

void R_GL_BufferMeshes()
{
	if(loading_new_main_theatre)
		return;

	//
	// Buffer big VBO for primitives up here
	//

	std::set<std::string> used_mesh_data_names = getCurrentTheatre()->getMeshDataNames();

	for(auto &mesh_data_pair : mesh_data_storage)
	{
		mesh_data_pair.second.is_in_use = graphx::identifiers::mesh_data::NOT_IN_USE;
		if(used_mesh_data_names.contains(mesh_data_pair.first))
			mesh_data_pair.second.is_in_use = graphx::identifiers::mesh_data::IN_USE;
	}

	for(auto &mesh_data_pair : mesh_data_storage)
	{
		if(mesh_data_pair.second.is_in_use == graphx::identifiers::mesh_data::NOT_CHECKED)
		{
			PRINTDEBUG("Uh... not checked got found for Mesh Data \"" << mesh_data_pair.first << "\"")
			continue;
		}

		if(mesh_data_pair.second.is_in_use == graphx::identifiers::mesh_data::NOT_IN_USE)
		{
			// Note: glIsBuffer will only work right if the unsigned int is a buffer name.
			// Buffer names are created/assigned by doing one of two things:
			//   1. Creating a buffer using glCreateBuffers
			//   2. Generating a buffer using glGenBuffers AND THEN binding it with glBindBuffer
			if(glIsBuffer(mesh_data_pair.second.VBO))
				glDeleteBuffers(1, &mesh_data_pair.second.VBO);
			if(glIsBuffer(mesh_data_pair.second.IBO))
				glDeleteBuffers(1, &mesh_data_pair.second.IBO);
			continue;
		}

		if(glIsBuffer(mesh_data_pair.second.VBO)) // If the VBO is buffered, the IBO doesn't need to be checked
			continue;

		glBindVertexArray(VAOs[VAO_DEFAULT]);
		glGenBuffers(1, &mesh_data_pair.second.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_data_pair.second.VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh_data_pair.second.vertices_size(), mesh_data_pair.second.vertices().data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		if(mesh_data_pair.second.hasValidIndices())
		{
			glGenBuffers(1, &mesh_data_pair.second.IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_data_pair.second.IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data_pair.second.indices_size(), mesh_data_pair.second.indices().data(), GL_STATIC_DRAW);
		}
	}
}

std::vector<RenderCmd> render_commands;
std::vector<LightRenderCmd> light_render_commands;
std::vector<PrimitiveRenderCmd> primitive_render_commands;

/*void R_GL_RenderPrimitives()
{
	glBindVertexArray(VAOs[VAO_PRIMITIVES]);
	glBindBuffer(GL_ARRAY_BUFFER, primitives_VBO);
	unsigned int primitive_offset = 0;
	for(auto rendercmd_iterator = primitive_render_commands.begin() ; rendercmd_iterator != primitive_render_commands.end() ; rendercmd_iterator++)
	{
		glBufferSubData(GL_ARRAY_BUFFER, primitive_offset, rendercmd_iterator.base()->numberOfVertices() * 11 * sizeof(float), rendercmd_iterator.base()->getVertices().data());
		primitive_offset += rendercmd_iterator.base()->numberOfVertices() * 11 * sizeof(float);
	}

	glUseProgram(shaders[shader_index]->id);

	for(auto rendercmd_iterator = primitive_render_commands.begin() ; rendercmd_iterator != primitive_render_commands.end() ;)
	{
		PrimitiveRenderCmd *render_command = rendercmd_iterator.base();
		std::vector<float> vertices = render_command->getVertices();

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glEnableVertexAttribArray(3);

		glm::mat4 projection_matrix = glm::perspective(glm::radians(getCurrentPlayer()->field_of_view), main_window_size[0] / main_window_size[1], camera_near, camera_far);

		if(render_command->primitive_material_override != nullptr)
		{
			shaders[shader_index]->setUniform("material.texture_diffuse", 0);
			shaders[shader_index]->setUniform("material.texture_specular", 1);
			shaders[shader_index]->setUniform("material.color", render_command->primitive_material_override->color);
			shaders[shader_index]->setUniform("material.specular_sharpness", render_command->primitive_material_override->specular_sharpness);
			shaders[shader_index]->setUniform("material.specular_strength", render_command->primitive_material_override->specular_strength);
			shaders[shader_index]->setUniform("mat_fullbright", render_command->primitive_material_override->mat_fullbright);
		}

		shaders[shader_index]->setUniform("is_primitive", true);
		shaders[shader_index]->setUniform("model_matrix", glm::mat4(1.0f));
		shaders[shader_index]->setUniform("view_matrix", getCurrentPlayer()->getViewMatrix());
		shaders[shader_index]->setUniform("projection_matrix", projection_matrix);
		shaders[shader_index]->setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(glm::mat4(1.0f)))));

		glDrawArrays(GL_LINES, render_command->array_offset, render_command->numberOfVertices());

		rendercmd_iterator = primitive_render_commands.erase(rendercmd_iterator);
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}*/

void R_GL_RenderLights(std::mutex &state_mutex, float interpolation_time)
{
	int point_light_index = 0;
	int spot_light_index = 0;

	glUseProgram(shaders[shader_index]->id);

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
			shaders[shader_index]->setUniform(which_light + "direction", render_command.light_data->direction);
		}

		else if(render_command.light_type == graphx::classes::LIGHTSPOT)
		{
			which_light = "spot_lights[" + std::to_string(spot_light_index++) + "].";
			shaders[shader_index]->setUniform(which_light + "inner_cutoff", render_command.light_data->inner_cutoff);
			shaders[shader_index]->setUniform(which_light + "outer_cutoff", render_command.light_data->outer_cutoff);
			shaders[shader_index]->setUniform(which_light + "direction", render_command.light_data->direction);
		}

		shaders[shader_index]->setUniform(which_light + "position", render_command.light_data->position);
		shaders[shader_index]->setUniform(which_light + "strength", render_command.light_data->strength);
		shaders[shader_index]->setUniform(which_light + "color", render_command.light_data->color);
		shaders[shader_index]->setUniform(which_light + "specular", render_command.light_data->color);
		shaders[shader_index]->setUniform(which_light + "ambient_strength", render_command.light_data->ambient_strength);
		shaders[shader_index]->setUniform(which_light + "range", render_command.light_data->range);
		shaders[shader_index]->setUniform(which_light + "intensity", render_command.light_data->intensity);
		shaders[shader_index]->setUniform(which_light + "falloff", render_command.light_data->falloff);

		if(render_command.renderDebugMesh())
			R_BufferRenderCmd(RenderCmd(render_command, (render_command.light_data->color * render_command.light_data->strength)));

		rendercmd_iterator = light_render_commands.erase(rendercmd_iterator);
	}
}

void R_GL_Render(std::mutex &state_mutex, float interpolation_time)
{
	if(loading_new_main_theatre)
		return;

	R_GL_RenderLights(state_mutex, interpolation_time);

	glUseProgram(shaders[shader_index]->id);
	shaders[shader_index]->setUniform("shader_debug_value", shader_debug_value);
	shaders[shader_index]->setUniform("lighting_debug_switches", glm::bvec3(lighting_switch_diffuse, lighting_switch_specular, lighting_switch_ambient));
	shaders[shader_index]->setUniform("point_lights_count", getCurrentTheatre()->point_lights_count);
	shaders[shader_index]->setUniform("spot_lights_count", getCurrentTheatre()->spot_lights_count);

	for(auto rendercmd_iterator = render_commands.begin() ; rendercmd_iterator != render_commands.end() ;)
	{
		RenderCmd render_command = *rendercmd_iterator.base();

		if(!render_command.isRenderable())
		{
			rendercmd_iterator = render_commands.erase(rendercmd_iterator);
			continue;
		}

		glm::mat4 model_matrix = glm::mat4(1.0f);
		glm::mat4 projection_matrix = glm::perspective(glm::radians(getCurrentPlayer()->field_of_view), main_window_size[0] / main_window_size[1], camera_near, camera_far);

		std::lock_guard guard(state_mutex);

		RenderState *current_state		=	render_command.current_render_state;
		RenderState *previous_state		=	render_command.previous_render_state;

		glm::vec3 interpolated_position	=	current_state->render_position;
		glm::vec3 interpolated_scale	=	current_state->render_scale;
		glm::quat interpolated_quat		=	current_state->render_quaternion;

		if(do_interpolation) // Eventually, I want to change interpolation to be more like GZDoom, and this will be how I test that
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

		glBindVertexArray(VAOs[VAO_DEFAULT]);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_data.VBO);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(0));
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));

		M_GL_BufferMaterialTexture(render_command.mesh_material->texture_diffuse, render_command.mesh_material->embedded_texture_diffuse);
		M_GL_BufferMaterialTexture(render_command.mesh_material->texture_specular, render_command.mesh_material->embedded_texture_specular);

		glBindTextureUnit(0, render_command.mesh_material->texture_diffuse);
		glBindTextureUnit(1, render_command.mesh_material->texture_specular);

		shaders[shader_index]->setUniform("model_matrix", model_matrix);
		shaders[shader_index]->setUniform("view_matrix", getCurrentPlayer()->getViewMatrix());
		shaders[shader_index]->setUniform("projection_matrix", projection_matrix);
		shaders[shader_index]->setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(model_matrix))));
		shaders[shader_index]->setUniform("view_position", getCurrentPlayer()->getViewPosition());

		shaders[shader_index]->setUniform("is_light", render_command.is_light_debug_mesh);
		shaders[shader_index]->setUniform("is_primitive", false);
		shaders[shader_index]->setUniform("material.texture_diffuse", 0);
		shaders[shader_index]->setUniform("material.texture_specular", 1);
		shaders[shader_index]->setUniform("material.color", render_command.mesh_material->color);
		shaders[shader_index]->setUniform("material.specular_sharpness", render_command.mesh_material->specular_sharpness);
		shaders[shader_index]->setUniform("material.specular_strength", render_command.mesh_material->specular_strength);
		shaders[shader_index]->setUniform("mat_fullbright", render_command.mesh_material->mat_fullbright);
		shaders[shader_index]->setUniform("environment.ambient_light", getCurrentEnvironment()->getAmbientLight());

		if(mesh_data.hasValidIndices())
		{
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_data.IBO);
			glDrawElements(GL_TRIANGLES, mesh_data.indices_count(), GL_UNSIGNED_INT, 0);
		}
		else
		{
			glDrawArrays(GL_TRIANGLES, 0, mesh_data.vertices_count());
		}

		rendercmd_iterator = render_commands.erase(rendercmd_iterator);
	}

	// R_GL_RenderPrimitives();
}

void R_BufferMeshes()
{
	if(loading_new_main_theatre)
		return;

	switch(graphx_api)
	{
	case GRAPHX_OPENGL:
		R_GL_BufferMeshes();
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

void R_Render(std::mutex &state_mutex, float interpolation_time)
{
	if(loading_new_main_theatre)
		return;

	getCurrentTheatre()->probeActorsForRenderCommands();

	switch(graphx_api)
	{
	case GRAPHX_OPENGL:
		R_GL_Render(state_mutex, interpolation_time);
		break;
	}
}

void R_GL_Initialize()
{
	glGenVertexArrays(VAOS_AMOUNT, &VAOs[VAO_DEFAULT]);
}

void R_InitializeRenderingAPI()
{
	switch(graphx_api)
	{
	case GRAPHX_OPENGL:
		R_GL_Initialize();
		break;
	}
}
