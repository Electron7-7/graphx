#include "r_common.hpp"
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

void R_StoreBuffers()
{
	glGenVertexArrays(VAOS_AMOUNT, &VAOs[0]);

	unsigned int current_vao_index = VAOS_AMOUNT + 1;

	for(Mesh *mesh : current_theatre->meshes)
	{
		if(mesh->vao_index != current_vao_index)
		{
			current_vao_index = mesh->vao_index;
			glBindVertexArray(VAOs[current_vao_index]);
		}

		mesh->generateTexture(); // Quickly generate the texture in the render thread

		glGenBuffers(1, &mesh->VBO);
		glGenBuffers(1, &mesh->IBO);

		glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
		glBufferData(GL_ARRAY_BUFFER, mesh->vertices.size() * sizeof(float), &mesh->vertices[0], GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->indices.size() * sizeof(unsigned int), &mesh->indices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	time_to_store_buffers = false;
	time_to_render = true;
}

void R_Render(std::mutex &state_mutex, double interpolation_time, glm::mat4 projection_matrix, GraphXPlayer *current_player, Environment *current_environment)
{
	unsigned int current_vao_index = VAOS_AMOUNT + 1; // Make sure we always switch to and bind the first used VAO
	unsigned int current_shader_index = 0;

	for(Mesh *mesh : current_theatre->meshes)
	{
		if(mesh->vao_index != current_vao_index)
		{
			current_vao_index = mesh->vao_index;
		}

		glm::mat4 model_matrix = glm::mat4(1.0f);

		if(mesh->owner != NULL) // If the Mesh has no owner, this stops the engine from crashing
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
			model_matrix = glm::scale(model_matrix, interpolated_scale);
			// add model_matrix rotation here
		}

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

		shaders[current_shader_index]->use();

		shaders[current_shader_index]->setMat4("model_matrix", model_matrix);
		shaders[current_shader_index]->setMat4("view_matrix", current_player->getViewMatrix());
		shaders[current_shader_index]->setMat4("projection_matrix", projection_matrix);

		shaders[current_shader_index]->setMat3("normal_matrix", glm::mat3(glm::transpose(glm::inverse(current_player->getViewMatrix()))));

		// Meshes only have one texture for now
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mesh->m_texture);
		shaders[current_shader_index]->setInt("texture_one", 0);
		
		static LightActorGeneric *current_light;
		bool current_mesh_is_light = false;
		shaders[current_shader_index]->setBool("is_light", false);

		/*
			NOTE: This will change almost immediately. I need to decide if I'm sticking with going through a vector of Meshes, switching to a vector of Actors,
			or something else entirely (JSON? RenderCmds?). I also need to make sure that R_Render doesn't crash if there are no lights, make a better system for
			acquiring lighting data and using it during the render (i.e: sorting lights to the top of whatever list I use to render), and finally, make the shader
			work with more than one light (which might mean going back to LearnOpenGL_Advanced for a bit before getting this code un-fucked). However, this code is
			only hard-coded a little bit and still runs, so I'm okay with pushing it as an update with some temporary solutions that will get changed very soon.
		*/

		if(mesh->owner != NULL && mesh->owner->type == ACTOR_LIGHT)
		{
			current_mesh_is_light = true;
			current_light = static_cast<LightActorGeneric *>(mesh->owner);
			shaders[current_shader_index]->setVec3("albedo", mesh->material.albedo); // Change to be the Material's Albedo
			shaders[current_shader_index]->setBool("is_light", true);
			// glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
			// continue;
		}

		if(!current_mesh_is_light)
		{
			shaders[current_shader_index]->setVec3("light_color", current_light->emission_color);
			shaders[current_shader_index]->setVec3("light_position", glm::vec3(current_player->getViewMatrix() * glm::vec4(current_light->position_global, 1.0f)));
		}

		shaders[current_shader_index]->setVec3("albedo", mesh->material.albedo);
		shaders[current_shader_index]->setVec3("ambient_light", current_environment->getAmbientLight());
		shaders[current_shader_index]->setVec3("view_position", current_player->position_global);
		shaders[current_shader_index]->setFloat("specular_strength", mesh->material.specular_strength);
		shaders[current_shader_index]->setInt("specular_sharpness", mesh->material.specular_sharpness);

		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}
}