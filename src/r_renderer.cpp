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
#include <cmath>

std::array<unsigned int, VAOS_AMOUNT> VAOs;
std::vector<GLShader *> shaders;
bool time_to_render = false;
bool time_to_store_buffers = false;
bool do_interpolation = true; // For testing when I change the interpolation method to be more like GZDoom
int graphx_api = 0; // Cheeky lil joke of a name
int shader_debug_value = 4;
unsigned int shader_index = SHADER_BLINN_PHONG;
glm::vec2 main_window_size(1280.0f, 720.0f);
float camera_near = 0.1f;
float camera_far = 1000.0f;
bool jolt_debug_render = false;

// Keeping these out of the header file for safety/isolation
std::vector<RenderCmd> render_commands;
std::vector<PrimitiveRenderCmd> primitive_render_commands;

std::map<std::string, gMeshData> mesh_data_map =
{
	{GRAPHX_CUBE, gMeshData(VAO_DEFAULT, CUBE_INDICES, CUBE_POSITIONS, CUBE_NORMALS, CUBE_UVS)},
	{GRAPHX_PYRAMID, gMeshData(VAO_DEFAULT, PYRAMID_POSITIONS, PYRAMID_POSITIONS, PYRAMID_UVS)},
	{GRAPHX_QUAD, gMeshData(VAO_DEFAULT, QUAD_POSITIONS, QUAD_NORMALS, QUAD_UVS)},
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
	gMeshData mesh_data;
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
		mesh_data = M_LoadOBJ(file_string_data.str());
		mesh_data_name = M_GetOBJName(file_string_data.str());

		if(!mesh_data_name.empty() && !mesh_data_map.contains(mesh_data_name))
			mesh_data_map[mesh_data_name] = mesh_data;
	}

	return mesh_data_name;
}


gMeshData M_LoadOBJ(std::string embedded_obj_file)
{
	namespace TO = tinyobj;

	gMeshData mesh_data;

	TO::ObjReaderConfig reader_config;
	TO::ObjReader reader;

	if(!reader.ParseFromString(embedded_obj_file, "", reader_config))
		if(!reader.Error().empty())
			PRINTERR("TinyObjReader: " << reader.Error())

	// if(!reader.Warning().empty())
		// PRINTDEBUG("TinyObjReader: " << reader.Warning())

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
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
				tinyobj::real_t vx = attrib.vertices[3*size_t(idx.vertex_index)+0];
				tinyobj::real_t vy = attrib.vertices[3*size_t(idx.vertex_index)+1];
				tinyobj::real_t vz = attrib.vertices[3*size_t(idx.vertex_index)+2];

				// std::cout << "Vertex: " << vx << ", " << vy << ", " << vz << std::endl;
				mesh_data.vertex_positions.insert(mesh_data.vertex_positions.end(), {(float)vx, (float)vy, (float)vz});

				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0)
				{
					tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
					tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
					tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];

					// std::cout << "Normal: " << nx << ", " << ny << ", " << nz << std::endl;
					mesh_data.vertex_normals.insert(mesh_data.vertex_normals.end(), {(float)nx, (float)ny, (float)nz});
				}

				else
				{
					mesh_data.vertex_normals.insert(mesh_data.vertex_normals.end(), {0.0f, 0.0f, 0.0f});
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0)
				{
					tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
					tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];

					// std::cout << "Texture Coordinate: " << tx << ", " << ty << std::endl;
					mesh_data.vertex_uvs.insert(mesh_data.vertex_uvs.end(), {(float)tx, (float)ty});
				}

				else
				{
					mesh_data.vertex_uvs.insert(mesh_data.vertex_uvs.end(), {0.0f, 0.0f});
				}

				if (idx.texcoord_index >= 0)
				{
					tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
					// std::cout << "Vertex Color: " << red << ", " << green << ", " << blue << std::endl;
					mesh_data.vertex_colors.insert(mesh_data.vertex_colors.end(), {(float)red, (float)green, (float)blue});
				}

				else
				{
					mesh_data.vertex_colors.insert(mesh_data.vertex_colors.end(), {1.0f, 1.0f, 1.0f});
				}
			}

			index_offset += fv;
		}
	}

	return mesh_data;
}

void R_GL_Initialize()
{
	glCreateVertexArrays(VAOS_AMOUNT, &VAOs[VAO_DEFAULT]);
	glBindVertexArray(VAOs[VAO_DEFAULT]);

	glVertexAttribFormat(0, 3, GL_FLOAT, GL_FALSE, 0);
	glVertexAttribBinding(0, 0);
	glEnableVertexAttribArray(0);

	glVertexAttribFormat(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float));
	glVertexAttribBinding(1, 0);
	glEnableVertexAttribArray(1);

	glVertexAttribFormat(2, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(float));
	glVertexAttribBinding(2, 0);
	glEnableVertexAttribArray(2);

	glVertexAttribFormat(3, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float));
	glVertexAttribBinding(3, 0);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
}

unsigned int M_GL_BufferMaterialTexture(unsigned char *texture_buffer)
{
	stbi_set_flip_vertically_on_load(true); // Obviously, automate this to flip relevant textures (when Y-Axis 0.0 is not on the bottom of the image)

	unsigned int texture_id;
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

	return texture_id;
}

void R_GL_BufferMeshes()
{
	if(loading_new_main_theatre)
		return;

	for(auto &mesh_data_pair : mesh_data_map)
		mesh_data_pair.second.needed_by_current_theatre = false;
	mesh_data_map.at(M_GetOBJName(ERROR_obj)).needed_by_current_theatre = true;

	// Eventually, I'd like to move Materials into their own separate storage, like with Mesh data
	for(Device *mesh_device : getCurrentTheatre()->getAllDevicesOfType(graphx::classes::MESH))
	{
		Material *material = static_cast<Mesh*>(mesh_device)->material;

		if(material->embedded_texture_diffuse != nullptr)
			material->texture_diffuse = M_GL_BufferMaterialTexture(material->embedded_texture_diffuse);
		if(material->embedded_texture_specular != nullptr)
			material->texture_specular = M_GL_BufferMaterialTexture(material->embedded_texture_specular);

		if(mesh_data_map.contains(static_cast<Mesh*>(mesh_device)->mesh_data_name))
			mesh_data_map.at(static_cast<Mesh*>(mesh_device)->mesh_data_name).needed_by_current_theatre = true;
	}

	for(auto &mesh_data_pair : mesh_data_map)
	{
		if(!mesh_data_pair.second.needed_by_current_theatre && mesh_data_pair.second.VBO != 0)
		{
			glDeleteBuffers(1, &mesh_data_pair.second.VBO);
			continue;
		}

		if(mesh_data_pair.second.VBO != 0)
			continue;

		glGenBuffers(1, &mesh_data_pair.second.VBO);
		glBindBuffer(GL_ARRAY_BUFFER, mesh_data_pair.second.VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh_data_pair.second.getVertexDataSize() * sizeof(float), mesh_data_pair.second.getVertexData().data(), GL_STATIC_DRAW);

		if(!mesh_data_pair.second.indices.empty())
		{
			glGenBuffers(1, &mesh_data_pair.second.IBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_data_pair.second.IBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh_data_pair.second.indices.size() * sizeof(unsigned int), mesh_data_pair.second.indices.data(), GL_STATIC_DRAW);
		}
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

/*void R_GL_RenderPrimitive(RenderCmd *render_command)
{
	glUseProgram(shaders[shader_index]->id);

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

	glBindVertexArray(VAO_DEFAULT);
	glBindBuffer(GL_ARRAY_BUFFER, render_command->getVBO());
	glDrawArrays(GL_LINES, 0, render_command->getVertexData().size());
}*/

void R_GL_Render(std::mutex &state_mutex, float interpolation_time)
{
	if(loading_new_main_theatre)
		return;

	// int current_vao_index = -1;
	int point_light_index = 0;
	int spot_light_index = 0;

	glUseProgram(shaders[shader_index]->id);
	shaders[shader_index]->setUniform("point_lights_count", getCurrentTheatre()->point_lights_count);
	shaders[shader_index]->setUniform("spot_lights_count", getCurrentTheatre()->spot_lights_count);
	shaders[shader_index]->setUniform("shader_debug_value", shader_debug_value);

	for(auto rendercmd_iterator = render_commands.begin() ; rendercmd_iterator != render_commands.end() ;)
	{
		RenderCmd render_command = *rendercmd_iterator.base();

		if(render_command.actor_pointer->isType(graphx::classes::LIGHTS))
		{
			Light *current_light = static_cast<Light *>(render_command.actor_pointer);
			shaders[shader_index]->setUniform("is_light", true);
			std::string which_light;

			if(current_light->isLightType(graphx::classes::LIGHT))
			{
				which_light = "point_lights[" + std::to_string(point_light_index++) + "].";
			}

			else if(current_light->isLightType(graphx::classes::LIGHTDIRECTIONAL))
			{
				which_light = "directional_light.";
				shaders[shader_index]->setUniform(which_light + "direction", static_cast<LightDirectional *>(current_light)->direction);
			}

			else if(current_light->isLightType(graphx::classes::LIGHTSPOT))
			{
				which_light = "spot_lights[" + std::to_string(spot_light_index++) + "].";
				shaders[shader_index]->setUniform(which_light + "inner_cutoff", static_cast<LightSpot *>(current_light)->getCutoffAngles()[0]);
				shaders[shader_index]->setUniform(which_light + "outer_cutoff", static_cast<LightSpot *>(current_light)->getCutoffAngles()[1]);
				shaders[shader_index]->setUniform(which_light + "direction", static_cast<LightSpot *>(current_light)->direction);
			}

			shaders[shader_index]->setUniform(which_light + "position", current_light->getPosition<glm::vec3>());
			shaders[shader_index]->setUniform(which_light + "strength", current_light->light_strength);
			shaders[shader_index]->setUniform(which_light + "color", current_light->light_color);
			shaders[shader_index]->setUniform(which_light + "specular", current_light->light_color);
			shaders[shader_index]->setUniform(which_light + "ambient_strength", current_light->light_ambient_strength);
			shaders[shader_index]->setUniform(which_light + "range", current_light->range);
			shaders[shader_index]->setUniform(which_light + "intensity", current_light->intensity);
			shaders[shader_index]->setUniform(which_light + "falloff", current_light->falloff);

			shaders[shader_index]->setUniform("material.color", (current_light->light_color * current_light->light_strength));
		}

		if(!render_command.actor_pointer->wantsToBeRendered())
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

		gMeshData mesh_data = mesh_data_map.at(render_command.mesh_data_name);

		glBindVertexArray(VAOs[VAO_DEFAULT]);

		glBindVertexBuffer(0, mesh_data.VBO, 0, 11 * sizeof(float));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, render_command.mesh_material->texture_diffuse);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, render_command.mesh_material->texture_specular);

		shaders[shader_index]->setUniform("model_matrix", model_matrix);
		shaders[shader_index]->setUniform("view_matrix", getCurrentPlayer()->getViewMatrix());
		shaders[shader_index]->setUniform("projection_matrix", projection_matrix);
		shaders[shader_index]->setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(model_matrix))));
		shaders[shader_index]->setUniform("view_position", getCurrentPlayer()->getViewPosition());

		shaders[shader_index]->setUniform("is_light", false);
		shaders[shader_index]->setUniform("is_primitive", false);
		shaders[shader_index]->setUniform("material.texture_diffuse", 0);
		shaders[shader_index]->setUniform("material.texture_specular", 1);
		shaders[shader_index]->setUniform("material.color", render_command.mesh_material->color);
		shaders[shader_index]->setUniform("material.specular_sharpness", render_command.mesh_material->specular_sharpness);
		shaders[shader_index]->setUniform("material.specular_strength", render_command.mesh_material->specular_strength);
		shaders[shader_index]->setUniform("mat_fullbright", render_command.mesh_material->mat_fullbright);
		shaders[shader_index]->setUniform("environment.ambient_light", getCurrentEnvironment()->getAmbientLight());

		if(mesh_data.indices.empty())
			glDrawArrays(GL_TRIANGLES, 0, mesh_data.getVertexDataSize() / 11);
		else
			glDrawElements(GL_TRIANGLES, mesh_data.indices.size(), GL_UNSIGNED_INT, mesh_data.indices.data());

		rendercmd_iterator = render_commands.erase(rendercmd_iterator);
	}

	glBindVertexArray(0);
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