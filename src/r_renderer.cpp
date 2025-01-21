#include "r_common.hpp"
#include "g_actors.hpp"
#include "g_theatre.hpp"
#include <cmath>

std::array<GLuint, VAOS_AMOUNT> VAOs;
std::vector<GLShader *> shaders;
bool time_to_render = false;
bool time_to_store_buffers = false;
bool do_interpolation = true; // For testing when I change the interpolation method to be more like GZDoom

void W_SwapAndClear(GLFWwindow *w_window, glm::vec3 w_clear_color)
{
	glfwSwapBuffers(w_window);
	glClearColor(w_clear_color[0], w_clear_color[1], w_clear_color[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void R_TroupeChanged()
{
	for(Actor *actor : current_theatre->troupe)
	{
		if(!actor->wantsToBeBuffered() || actor->mesh.is_buffered)
			continue;

		unsigned int current_vao_index = VAOS_AMOUNT + 1;

		if(actor->mesh.vao_index != current_vao_index)
		{
			current_vao_index = actor->mesh.vao_index;
			glBindVertexArray(VAOs[current_vao_index]);
		}
		
		R_GL_BufferMeshData(&actor->mesh);
	}

	current_troupe_changed = false;
}

void R_StoreBuffers()
{
	unsigned int current_vao_index = VAOS_AMOUNT + 1;

	for(Actor *actor : current_theatre->troupe)
	{
		if(!actor->wantsToBeBuffered())
			continue;

		if(actor->mesh.vao_index != current_vao_index)
		{
			current_vao_index = actor->mesh.vao_index;
			glBindVertexArray(VAOs[current_vao_index]);
		}

		R_GL_BufferMeshData(&actor->mesh);
	}

	R_GL_BufferMeshData(&current_theatre->stage);

	time_to_store_buffers = false;
	time_to_render = true;
}

void R_GL_BufferMeshData(Mesh *mesh)
{
	Material *material = &mesh->material;
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

void R_Render(std::mutex &state_mutex, double interpolation_time, glm::mat4 projection_matrix, Environment *current_environment)
{
	if(current_troupe_changed)
		R_TroupeChanged();

	unsigned int current_vao_index = VAOS_AMOUNT + 1; // Make sure we always switch to and bind the first used VAO
	unsigned int shader_index = SHADER_PHONG;
	int point_light_index = 0;
	int spot_light_index = 0;

	glUseProgram(shaders[shader_index]->id);
	shaders[shader_index]->setUniform("point_lights_count", current_theatre->point_lights_count);
	shaders[shader_index]->setUniform("spot_lights_count", current_theatre->spot_lights_count);

	for(Actor *actor : current_theatre->troupe)
	{
		Mesh *mesh = &actor->mesh;

		if(mesh->vao_index != current_vao_index)
		{
			current_vao_index = mesh->vao_index;
			glBindVertexArray(VAOs[current_vao_index]);
		}

		glm::mat4 model_matrix = glm::mat4(1.0f);

		if(actor->wantsToBeRendered()) // If the Mesh has no owner, this stops the engine from crashing
		{
			// Note: Quaternions (and angular movement) should use slerp instead of lerp
			std::lock_guard guard(state_mutex);

			RenderState current_state		=	mesh->owner->current_state_buffer[mesh->owner->state_index];
			RenderState previous_state		=	mesh->owner->previous_state_buffer[mesh->owner->state_index];

			glm::vec3 interpolated_position	=	current_state.render_position;
			glm::quat interpolated_quat		=	current_state.render_quaternion;
			glm::vec3 interpolated_scale	=	current_state.render_scale;

			if(do_interpolation) // Eventually, I want to change interpolation to be more like GZDoom, and this will be how I test that
			{
				for(unsigned int i = 0 ; i < 3 ; i++)
					interpolated_position[i] = std::lerp(previous_state.render_position[i], current_state.render_position[i], interpolation_time);

				for(unsigned int i = 0 ; i < 4 ; i++)
					interpolated_quat[i] = std::lerp(previous_state.render_quaternion[i], current_state.render_quaternion[i], interpolation_time);

				for(unsigned int i = 0 ; i < 3 ; i++)
					interpolated_scale[i] = std::lerp(previous_state.render_scale[i], current_state.render_scale[i], interpolation_time);
			}

			model_matrix = glm::translate(model_matrix, interpolated_position);
			model_matrix *= glm::toMat4(interpolated_quat);
			model_matrix = glm::scale(model_matrix, interpolated_scale);

			glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IBO);

			switch(current_vao_index) // I may want to use a method that doesn't require me to manually write down and update every single vertex attribute format
			{
				case(VAO_HANDMADE):
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
					glEnableVertexAttribArray(0);

					glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
					glEnableVertexAttribArray(1);

					glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
					glEnableVertexAttribArray(2);

					break;
			}

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, mesh->material.texture_diffuse);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mesh->material.texture_specular);

			shaders[shader_index]->setUniform("material.texture_diffuse", 0);
			shaders[shader_index]->setUniform("material.texture_specular", 1);
			shaders[shader_index]->setUniform("material.color", mesh->material.color);
			shaders[shader_index]->setUniform("is_light", false);
		}

		shaders[shader_index]->setUniform("model_matrix", model_matrix);
		shaders[shader_index]->setUniform("view_matrix", current_player->getViewMatrix());
		shaders[shader_index]->setUniform("projection_matrix", projection_matrix);
		shaders[shader_index]->setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(model_matrix))));
		shaders[shader_index]->setUniform("view_position", current_player->position_global);
		
		/*
			NOTE: This will change almost immediately. I need to decide if I'm sticking with going through a vector of Meshes, switching to a vector of Actors,
			or something else entirely (JSON? RenderCmds?). I also need to make sure that R_Render doesn't crash if there are no lights, make a better system for
			acquiring lighting data and using it during the render (i.e: sorting lights to the top of whatever list I use to render), and finally, make the shader
			work with more than one light (which might mean going back to LearnOpenGL_Advanced for a bit before getting this code un-fucked). However, this code is
			only hard-coded a little bit and still runs, so I'm okay with pushing it as an update with some temporary solutions that will get changed very soon.
		*/

		if(actor->actor_type == ACTOR_LIGHT)
		{
			Light *current_light = static_cast<Light *>(actor);
			shaders[shader_index]->setUniform("is_light", true);
			std::string which_light;

			switch(current_light->light_type)
			{
				case(LIGHT_POINT):
					which_light = "point_lights[" + std::to_string(point_light_index++) + "].";
					break;
				case(LIGHT_DIRECTIONAL):
					which_light = "directional_light.";
					shaders[shader_index]->setUniform(which_light + "direction", static_cast<LightDirectional *>(current_light)->direction);
					break;
				case(LIGHT_SPOT):
					which_light = "spot_lights[" + std::to_string(spot_light_index++) + "].";
					shaders[shader_index]->setUniform(which_light + "inner_cutoff", static_cast<LightSpot *>(current_light)->getCutoffAngles()[0]);
					shaders[shader_index]->setUniform(which_light + "outer_cutoff", static_cast<LightSpot *>(current_light)->getCutoffAngles()[1]);
					shaders[shader_index]->setUniform(which_light + "direction", static_cast<LightSpot *>(current_light)->direction);
					break;
			}

			shaders[shader_index]->setUniform(which_light + "position", current_light->position_global);
			shaders[shader_index]->setUniform(which_light + "strength", current_light->light_strength);
			shaders[shader_index]->setUniform(which_light + "color", current_light->light_color);
			shaders[shader_index]->setUniform(which_light + "specular", current_light->light_color);
			shaders[shader_index]->setUniform(which_light + "range", current_light->range);
			shaders[shader_index]->setUniform(which_light + "intensity", current_light->intensity);
			shaders[shader_index]->setUniform(which_light + "falloff", current_light->falloff);

			shaders[shader_index]->setUniform("material.color", (current_light->light_color * current_light->light_strength));
		}

		shaders[shader_index]->setUniform("environment.ambient_light", current_environment->getAmbientLight());
		shaders[shader_index]->setUniform("material.specular_sharpness", mesh->material.specular_sharpness);
		shaders[shader_index]->setUniform("material.specular_strength", mesh->material.specular_strength);
		shaders[shader_index]->setUniform("mat_fullbright", mesh->material.mat_fullbright);

		if(actor->wantsToBeRendered())
			glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}

	R_RenderFlats(projection_matrix, glm::mat4(1.0f), current_environment, shader_index);
}

void R_RenderFlats(glm::mat4 projection_matrix, glm::mat4 model_matrix, Environment *current_environment, unsigned int shader_index)
{
	glBindBuffer(GL_ARRAY_BUFFER, current_theatre->stage.VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, current_theatre->stage.IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, current_theatre->stage.material.texture_diffuse);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, current_theatre->stage.material.texture_specular);

	shaders[shader_index]->setUniform("material.texture_color", 0);
	shaders[shader_index]->setUniform("material.texture_specular", 1);

	shaders[shader_index]->setUniform("model_matrix", model_matrix);
	shaders[shader_index]->setUniform("view_matrix", current_player->getViewMatrix());
	shaders[shader_index]->setUniform("projection_matrix", projection_matrix);
	shaders[shader_index]->setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(model_matrix))));
	shaders[shader_index]->setUniform("view_position", current_player->position_global);

	glDrawElements(GL_TRIANGLES, current_theatre->stage.indices.size(), GL_UNSIGNED_INT, 0);
}