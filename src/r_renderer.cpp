#include "r_common.hpp"
#include "g_common.hpp"
#include "g_actors.hpp"
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

graphx::gMeshData M_LoadModelFile(std::string file_path, std::string file_extension)
{
	std::ifstream model_file(file_path);
	std::stringstream file_string_data;
	file_string_data << model_file.rdbuf();

	if(!model_file.is_open())
	{
		PRINTERR("M_LoadModelFile called but file could not be opened/found! An error mesh will be returned!")
		return M_LoadOBJ(ERROR_obj);
	}

	model_file.close();

	if(!file_extension.compare("obj"))
		return M_LoadOBJ(file_string_data.str());

	PRINTERR("M_LoadModelFile called but no file type was provided! An error mesh will be returned!")
	return M_LoadOBJ(ERROR_obj);
}

namespace TO = tinyobj;

graphx::gMeshData M_LoadOBJ(std::string embedded_obj_file)
{
	TO::ObjReaderConfig reader_config;
	TO::ObjReader reader;

	if(!reader.ParseFromString(embedded_obj_file, "", reader_config))
	{
		if(!reader.Error().empty())
		{
			PRINTERR("TinyObjReader: " << reader.Error())
		}

		exit(1);
	}

	if (!reader.Warning().empty())
	{
		// std::cout << "TinyObjReader: " << reader.Warning();
	}

	auto& attrib = reader.GetAttrib();
	auto& shapes = reader.GetShapes();

	std::vector<float> vertices;

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
				vertices.insert(vertices.end(), {(float)vx / PREEMPTIVE_OBJ_SCALE, (float)vy / PREEMPTIVE_OBJ_SCALE, (float)vz / PREEMPTIVE_OBJ_SCALE});

				// Check if `normal_index` is zero or positive. negative = no normal data
				if (idx.normal_index >= 0)
				{
					tinyobj::real_t nx = attrib.normals[3*size_t(idx.normal_index)+0];
					tinyobj::real_t ny = attrib.normals[3*size_t(idx.normal_index)+1];
					tinyobj::real_t nz = attrib.normals[3*size_t(idx.normal_index)+2];

					// std::cout << "Normal: " << nx << ", " << ny << ", " << nz << std::endl;
					vertices.insert(vertices.end(), {(float)nx, (float)ny, (float)nz});
				}

				else
				{
					vertices.insert(vertices.end(), {0.0f, 0.0f, 0.0f});
				}

				// Check if `texcoord_index` is zero or positive. negative = no texcoord data
				if (idx.texcoord_index >= 0)
				{
					tinyobj::real_t tx = attrib.texcoords[2*size_t(idx.texcoord_index)+0];
					tinyobj::real_t ty = attrib.texcoords[2*size_t(idx.texcoord_index)+1];
				
					// std::cout << "Texture Coordinate: " << tx << ", " << ty << std::endl;
					vertices.insert(vertices.end(), {(float)tx, (float)ty});
				}

				else
				{
					vertices.insert(vertices.end(), {0.0f, 0.0f});
				}

				if (idx.texcoord_index >= 0)
				{
					tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];
					// std::cout << "Vertex Color: " << red << ", " << green << ", " << blue << std::endl;
					vertices.insert(vertices.end(), {(float)red, (float)green, (float)blue});
				}

				else
				{
					vertices.insert(vertices.end(), {1.0f, 1.0f, 1.0f});
				}
			}

			index_offset += fv;
		}
	}

	return graphx::gMeshData(vertices, {}, VAO_OBJ);
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

void R_Render(std::mutex &state_mutex, float interpolation_time)
{
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
	current_vao_index = VAOS_AMOUNT + 1;

	for(Actor *actor : getCurrentTheatre()->troupe)
	{
		// There used to exist Actor::wantsToBeBuffered, but it was only ever used here, so I got rid of it
		if(actor->mesh == nullptr || actor->mesh->is_buffered || actor->isType(graphx::classes::GRAPHXPLAYER))
			continue;

		Mesh *mesh = actor->mesh;
		Material *material = mesh->material;

		if(mesh->vao_index != current_vao_index)
		{
			current_vao_index = mesh->vao_index;
			glBindVertexArray(VAOs[current_vao_index]);
		}

		if(material->embedded_texture_diffuse != NULL)
			material->texture_diffuse = material->bufferTextureFromMemory(material->embedded_texture_diffuse);
		if(material->embedded_texture_specular != NULL)
			material->texture_specular = material->bufferTextureFromMemory(material->embedded_texture_specular);

		glGenBuffers(1, &mesh->VBO);
		glGenBuffers(1, &mesh->IBO);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(float), &mesh->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);

		mesh->is_buffered = true;
	}
}

void R_GL_Render(std::mutex &state_mutex, float interpolation_time)
{
	current_vao_index = VAOS_AMOUNT + 1; // Make sure we always switch to and bind the first used VAO
	int point_light_index = 0;
	int spot_light_index = 0;

	glUseProgram(shaders[shader_index]->id);
	shaders[shader_index]->setUniform("point_lights_count", getCurrentTheatre()->point_lights_count);
	shaders[shader_index]->setUniform("spot_lights_count", getCurrentTheatre()->spot_lights_count);
	shaders[shader_index]->setUniform("shader_debug_value", shader_debug_value);

	for(Actor *actor : getCurrentTheatre()->troupe)
	{
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
			continue;

		glm::mat4 model_matrix = glm::mat4(1.0f);
		glm::mat4 projection_matrix = glm::perspective(glm::radians(getCurrentPlayer()->field_of_view), main_window_size[0] / main_window_size[1], camera_near, camera_far);

		Mesh *mesh = actor->mesh;

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
		glBindTexture(GL_TEXTURE_2D, mesh->material->texture_diffuse);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, mesh->material->texture_specular);

		shaders[shader_index]->setUniform("material.texture_diffuse", 0);
		shaders[shader_index]->setUniform("material.texture_specular", 1);
		shaders[shader_index]->setUniform("material.color", mesh->material->color);
		shaders[shader_index]->setUniform("is_light", false);
		shaders[shader_index]->setUniform("model_matrix", model_matrix);
		shaders[shader_index]->setUniform("view_matrix", getCurrentPlayer()->getViewMatrix());
		shaders[shader_index]->setUniform("projection_matrix", projection_matrix);
		shaders[shader_index]->setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(model_matrix))));
		shaders[shader_index]->setUniform("view_position", getCurrentPlayer()->getViewPosition());
		shaders[shader_index]->setUniform("environment.ambient_light", getCurrentEnvironment()->getAmbientLight());
		shaders[shader_index]->setUniform("material.specular_sharpness", actor->mesh->material->specular_sharpness);
		shaders[shader_index]->setUniform("material.specular_strength", actor->mesh->material->specular_strength);
		shaders[shader_index]->setUniform("mat_fullbright", actor->mesh->material->mat_fullbright);

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
			continue;
		}

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}
}