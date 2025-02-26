#include "r_common.hpp"
#include "g_actors.hpp"
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
	for(Actor *actor : getCurrentTheatre()->troupe)
	{
		if(!actor->wantsToBeBuffered() || actor->mesh->is_buffered)
			continue;

		unsigned int current_vao_index = VAOS_AMOUNT + 1;

		if(actor->mesh->vao_index != current_vao_index)
		{
			current_vao_index = actor->mesh->vao_index;
			glBindVertexArray(VAOs[current_vao_index]);
		}
		
		R_GL_BufferMeshData(actor->mesh);
	}

	current_troupe_changed = false;
}

void R_StoreBuffers()
{
	int current_vao_index = VAOS_AMOUNT + 1;

	for(Actor *actor : getCurrentTheatre()->troupe)
	{
		if(!actor->wantsToBeBuffered())
			continue;

		if(actor->mesh->vao_index != current_vao_index)
		{
			current_vao_index = actor->mesh->vao_index;
			glBindVertexArray(VAOs[current_vao_index]);
		}

		R_GL_BufferMeshData(actor->mesh);
	}

	R_GL_BufferMeshData(getCurrentTheatre()->stage);

	time_to_store_buffers = false;
	time_to_render = true;
}

void R_GL_BufferMeshData(Mesh *mesh)
{
	Material *material = mesh->material;
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

int shader_debug_value = 4;
unsigned int shader_index = SHADER_BLINN_PHONG;

void R_Render(std::mutex &state_mutex, float interpolation_time, glm::mat4 projection_matrix)
{
	if(current_troupe_changed)
		R_TroupeChanged();

	int current_vao_index = VAOS_AMOUNT + 1; // Make sure we always switch to and bind the first used VAO
	int point_light_index = 0;
	int spot_light_index = 0;

	glUseProgram(shaders[shader_index]->id);
	shaders[shader_index]->setUniform("point_lights_count", getCurrentTheatre()->point_lights_count);
	shaders[shader_index]->setUniform("spot_lights_count", getCurrentTheatre()->spot_lights_count);
	shaders[shader_index]->setUniform("shader_debug_value", shader_debug_value);

	for(Actor *actor : getCurrentTheatre()->troupe)
	{
		glm::mat4 model_matrix = glm::mat4(1.0f);

		Mesh *mesh = actor->mesh;

		if(mesh->vao_index != current_vao_index)
		{
			current_vao_index = mesh->vao_index;
			glBindVertexArray(VAOs[current_vao_index]);
		}

		if(actor->wantsToBeRendered())
		{
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
			glBindTexture(GL_TEXTURE_2D, mesh->material->texture_diffuse);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, mesh->material->texture_specular);

			shaders[shader_index]->setUniform("material.texture_diffuse", 0);
			shaders[shader_index]->setUniform("material.texture_specular", 1);
			shaders[shader_index]->setUniform("material.color", mesh->material->color);
			shaders[shader_index]->setUniform("is_light", false);
		}

		shaders[shader_index]->setUniform("model_matrix", model_matrix);
		shaders[shader_index]->setUniform("view_matrix", getCurrentPlayer()->getViewMatrix());
		shaders[shader_index]->setUniform("projection_matrix", projection_matrix);
		shaders[shader_index]->setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(model_matrix))));
		shaders[shader_index]->setUniform("view_position", getCurrentPlayer()->getViewPosition());

		/*
			NOTE: This will change almost immediately. I need to decide if I'm sticking with going through a vector of Meshes, switching to a vector of Actors,
			or something else entirely (JSON? RenderCmds?). I also need to make sure that R_Render doesn't crash if there are no lights, make a better system for
			acquiring lighting data and using it during the render (i.e: sorting lights to the top of whatever list I use to render), and finally, make the shader
			work with more than one light (which might mean going back to LearnOpenGL_Advanced for a bit before getting this code un-fucked). However, this code is
			only hard-coded a little bit and still runs, so I'm okay with pushing it as an update with some temporary solutions that will get changed very soon.
		*/

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

		shaders[shader_index]->setUniform("environment.ambient_light", getCurrentEnvironment()->getAmbientLight());

		if(actor->wantsToBeRendered())
		{
			shaders[shader_index]->setUniform("material.specular_sharpness", actor->mesh->material->specular_sharpness);
			shaders[shader_index]->setUniform("material.specular_strength", actor->mesh->material->specular_strength);
			shaders[shader_index]->setUniform("mat_fullbright", actor->mesh->material->mat_fullbright);
			glDrawElements(GL_TRIANGLES, actor->mesh->indices.size(), GL_UNSIGNED_INT, 0);
		}
	}

	R_RenderStage(projection_matrix, shader_index);
}

void R_RenderStage(glm::mat4 projection_matrix, unsigned int shader_index)
{
	glm::mat4 model_matrix = glm::mat4(1.0f);
	model_matrix = glm::translate(model_matrix, getCurrentTheatre()->stage_position);
	model_matrix *= glm::toMat4(getCurrentTheatre()->stage_quaternion);
	model_matrix = glm::scale(model_matrix, getCurrentTheatre()->stage_scale);

	glBindBuffer(GL_ARRAY_BUFFER, getCurrentTheatre()->stage->VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, getCurrentTheatre()->stage->IBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, getCurrentTheatre()->stage->material->texture_diffuse);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, getCurrentTheatre()->stage->material->texture_specular);

	shaders[shader_index]->setUniform("material.texture_color", 0);
	shaders[shader_index]->setUniform("material.texture_specular", 1);

	shaders[shader_index]->setUniform("model_matrix", model_matrix);
	shaders[shader_index]->setUniform("view_matrix", getCurrentPlayer()->getViewMatrix());
	shaders[shader_index]->setUniform("projection_matrix", projection_matrix);
	shaders[shader_index]->setUniform("normal_matrix", glm::mat3(glm::transpose(glm::inverse(model_matrix))));
	shaders[shader_index]->setUniform("view_position", getCurrentPlayer()->getViewPosition());

	glDrawElements(GL_TRIANGLES, getCurrentTheatre()->stage->indices.size(), GL_UNSIGNED_INT, 0);
}