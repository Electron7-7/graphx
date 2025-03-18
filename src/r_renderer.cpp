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

std::array<GLuint, VAOS_AMOUNT> VAOs;
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
int current_vao_index = VAOS_AMOUNT + 1; // Pulled out of the render function so my debug menu can see it
bool jolt_debug_render = false;

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
		{
			mesh_data_storage[mesh_data_name] = M_LoadOBJ(file_string_data.str());
		}
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

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();

	// Loop over shapes
	for (size_t s = 0; s < shapes.size(); s++)
	{
		// Loop over faces(polygon)
		size_t index_offset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
		{
			std::vector<float> vertex;
			size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]);

			// Loop over vertices in the face.
			for (size_t v = 0; v < fv; v++)
			{
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
			}

			mesh_data.addVertex(vertex);
			index_offset += fv;
		}
	}

	return mesh_data;
}

void R_StoreBuffers()
{
	switch(graphx_api)
	{
	case GRAPHX_OPENGL:
		R_GL_BufferMeshes();
		break;
	}

	time_to_store_buffers = false;
	time_to_render = true;
}

std::vector<RenderCmd> render_commands; // Keeping this out of the header file for now

void R_BufferRenderCmd(RenderCmd render_command)
{
	render_commands.insert(render_commands.end(), render_command);
}

void R_Render(std::mutex &state_mutex, float interpolation_time)
{
	if(loading_new_main_theatre)
		return;

	if(time_to_store_buffers)
		R_StoreBuffers();

	switch(graphx_api)
	{
	case GRAPHX_OPENGL:
		R_GL_Render(state_mutex, interpolation_time);
		break;
	}
}

void R_GL_BufferMeshes()
{
	if(loading_new_main_theatre)
		return;

	glBindVertexArray(VAOs[VAO_DEFAULT]); // There's only one VAO, currently

	// START CODING HERE
	// START CODING HERE
	// START CODING HERE
	// START CODING HERE
	// START CODING HERE
	// START CODING HERE
	// START CODING HERE
	for()
	{
		RenderCmd *render_command = rendercmd_iterator.base();

		if(!render_command->isRenderable())
		{
			rendercmd_iterator = render_commands.erase(rendercmd_iterator);
			continue;
		}

		if(render_command->mesh_material->embedded_texture_diffuse != nullptr)
			render_command->mesh_material->texture_diffuse = render_command->mesh_material->bufferTextureFromMemory(render_command->mesh_material->embedded_texture_diffuse);
		if(render_command->mesh_material->embedded_texture_specular != nullptr)
			render_command->mesh_material->texture_specular = render_command->mesh_material->bufferTextureFromMemory(render_command->mesh_material->embedded_texture_specular);

		glGenBuffers(1, &render_command->mesh_data->VBO);
		glGenBuffers(1, &render_command->mesh_data->IBO);

		glBindBuffer(GL_ARRAY_BUFFER, render_command->mesh_data->VBO);
		glBufferData(GL_ARRAY_BUFFER, render_command->mesh_data->vertices.size() * sizeof(float), &render_command->mesh_data->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_command->mesh_data->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, render_command->mesh_data->indices.size() * sizeof(unsigned int), &render_command->mesh_data->indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		rendercmd_iterator = render_commands.erase(rendercmd_iterator);
	}
}

void R_GL_RenderPrimitive(RenderCmd *render_command)
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

	glBindVertexArray(VAO_OBJ);
	glBindBuffer(GL_ARRAY_BUFFER, render_command->getVBO());
	glDrawArrays(GL_LINES, 0, render_command->getVertexData().size());
}

void R_GL_Render(std::mutex &state_mutex, float interpolation_time)
{
	if(loading_new_main_theatre)
		return;

	// current_vao_index = VAOS_AMOUNT + 1; // Make sure we always switch to and bind the first used VAO
	int point_light_index = 0;
	int spot_light_index = 0;

	glUseProgram(shaders[shader_index]->id);
	shaders[shader_index]->setUniform("point_lights_count", getCurrentTheatre()->point_lights_count);
	shaders[shader_index]->setUniform("spot_lights_count", getCurrentTheatre()->spot_lights_count);
	shaders[shader_index]->setUniform("shader_debug_value", shader_debug_value);

	getCurrentTheatre()->probeActorsForRenderCommands();

	for(auto rendercmd_iterator = render_commands.begin() ; rendercmd_iterator != render_commands.end() ;)
	{
		if(rendercmd_iterator.base()->isPrimitive())
		{
			R_GL_RenderPrimitive(rendercmd_iterator.base());
			rendercmd_iterator = render_commands.erase(rendercmd_iterator);
			continue;
		}

		Actor *actor = rendercmd_iterator.base()->render_actor;

		if(actor->isType(graphx::classes::LIGHTS))
		{
			Light *current_light = static_cast<Light *>(actor);
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

		if(!actor->wantsToBeRendered())
		{
			rendercmd_iterator = render_commands.erase(rendercmd_iterator);
			continue;
		}

		Mesh *mesh = actor->mesh;
		Material *material = mesh->material;

		glm::mat4 model_matrix = glm::mat4(1.0f);
		glm::mat4 projection_matrix = glm::perspective(glm::radians(getCurrentPlayer()->field_of_view), main_window_size[0] / main_window_size[1], camera_near, camera_far);

		std::lock_guard guard(state_mutex);

		RenderState current_state		=	actor->current_state_buffer[actor->state_index];
		RenderState previous_state		=	actor->previous_state_buffer[actor->state_index];

		glm::vec3 interpolated_position	=	current_state.render_position;
		glm::vec3 interpolated_scale	=	current_state.render_scale;
		glm::quat interpolated_quat		=	current_state.render_quaternion;

		if(do_interpolation) // Eventually, I want to change interpolation to be more like GZDoom, and this will be how I test that
		{
			for(unsigned int i = 0 ; i < 3 ; i++)
				interpolated_position[i] = std::lerp(previous_state.render_position[i], current_state.render_position[i], interpolation_time);

			interpolated_quat = glm::slerp(previous_state.render_quaternion, current_state.render_quaternion, interpolation_time);

			for(unsigned int i = 0 ; i < 3 ; i++)
				interpolated_scale[i] = std::lerp(previous_state.render_scale[i], current_state.render_scale[i], interpolation_time);
		}

		model_matrix = glm::translate(model_matrix, interpolated_position);
		model_matrix *= glm::toMat4(interpolated_quat);
		model_matrix = glm::scale(model_matrix, interpolated_scale * mesh->mesh_scale);

		if(mesh->vao_index != current_vao_index)
		{
			current_vao_index = mesh->vao_index;
			glBindVertexArray(VAOs[current_vao_index]);
		}

		glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IBO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, material->texture_diffuse);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, material->texture_specular);

		shaders[shader_index]->setUniform("model_matrix", model_matrix);
		shaders[shader_index]->setUniform("view_matrix", getCurrentPlayer()->getViewMatrix());
		shaders[shader_index]->setUniform("projection_matrix", projection_matrix);
		shaders[shader_index]->setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(model_matrix))));
		shaders[shader_index]->setUniform("view_position", getCurrentPlayer()->getViewPosition());

		shaders[shader_index]->setUniform("is_light", false);
		shaders[shader_index]->setUniform("is_primitive", false);
		shaders[shader_index]->setUniform("material.texture_diffuse", 0);
		shaders[shader_index]->setUniform("material.texture_specular", 1);
		shaders[shader_index]->setUniform("material.color", material->color);
		shaders[shader_index]->setUniform("material.specular_sharpness", material->specular_sharpness);
		shaders[shader_index]->setUniform("material.specular_strength", material->specular_strength);
		shaders[shader_index]->setUniform("mat_fullbright", material->mat_fullbright);
		shaders[shader_index]->setUniform("environment.ambient_light", getCurrentEnvironment()->getAmbientLight());

		int vao_stride_size = 8;

		// Currently, both VAOs share the same first three attributes, with VAO_OBJ having an extra fourth attribute
		// This is subject to change, so keep in mind I'll probably need to move these into a switch statement or something
		if(current_vao_index == VAO_OBJ)
		{
			vao_stride_size = 11;
			glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, vao_stride_size * sizeof(float), (void*)(8 * sizeof(float)));
			glEnableVertexAttribArray(3);
		}

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vao_stride_size * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, vao_stride_size * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, vao_stride_size * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		if(current_vao_index == VAO_OBJ)
		{
			glDrawArrays(GL_TRIANGLES, mesh->vertices[0], mesh->vertices.size());
			rendercmd_iterator = render_commands.erase(rendercmd_iterator);
			continue;
		}

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
		rendercmd_iterator = render_commands.erase(rendercmd_iterator);
	}
}

void R_GL_InitializeRenderingAPI()
{
	glGenVertexArrays(VAOS_AMOUNT, &VAOs[0]);
}

void R_InitializeRenderingAPI()
{
	switch(graphx_api)
	{
	case GRAPHX_OPENGL:
		R_GL_InitializeRenderingAPI();
		break;
	}
}