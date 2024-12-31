#include "r_common.hpp"
#include "g_theatre.hpp"
#include <exception>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

std::array<GLuint, VAOS_AMOUNT> VAOs;
std::vector<GLShader *> shaders;
std::atomic_bool time_to_render = false;
std::atomic_bool time_to_store_buffers = false;
std::atomic_bool do_interpolation = true;				// For testing when I change the interpolation method to be more like GZDoom

//
// GLShader
//
GLShader::GLShader(const char *vertex_path, const char *fragment_path)
{
	std::string vertex_code;
	std::string fragment_code;
	std::ifstream v_shader_file;
	std::ifstream f_shader_file;

	v_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	f_shader_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		v_shader_file.open(vertex_path);
		f_shader_file.open(fragment_path);
		std::stringstream v_shader_stream, f_shader_stream;

		v_shader_stream << v_shader_file.rdbuf();
		f_shader_stream << f_shader_file.rdbuf();

		v_shader_file.close();
		f_shader_file.close();

		vertex_code = v_shader_stream.str();
		fragment_code = f_shader_stream.str();
	}

	catch(std::ifstream::failure e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
	}

	const char *v_shader_code = vertex_code.c_str();
	const char *f_shader_code = fragment_code.c_str();

	unsigned int vertex, fragment;
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v_shader_code, NULL);
	glCompileShader(vertex);
	shaderErrorHandler(vertex, GLSHADER_TYPE_VERTEX);

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f_shader_code, NULL);
	glCompileShader(fragment);
	shaderErrorHandler(fragment, GLSHADER_TYPE_FRAGMENT);

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	shaderErrorHandler(ID, GLSHADER_TYPE_PROGRAM);

	glDeleteShader(vertex);
	glDeleteShader(fragment);
};

void GLShader::use()
{
	glUseProgram(ID);
}

void GLShader::setBool(const std::string &name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void GLShader::setInt(const std::string &name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void GLShader::setFloat(const std::string &name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void GLShader::setMatrix(const std::string &name, glm::mat4 value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void GLShader::shaderErrorHandler(int thing, int type)
{
	int success;
	char info_log[512];
	std::string name = "VERTEX";

	switch(type)
	{
		case GLSHADER_TYPE_FRAGMENT:
			name = "FRAGMENT";
		case GLSHADER_TYPE_VERTEX:
			glGetShaderiv(thing, GL_COMPILE_STATUS, &success);
			if(!success)
			{
				glGetShaderInfoLog(thing, 512, NULL, info_log);
				std::cerr << "ERROR::SHADER::" << name << "::COMPILATION::FAILED\n" << info_log << std::endl;
				return;
			}
			break;
		case GLSHADER_TYPE_PROGRAM:
			glGetProgramiv(thing, GL_LINK_STATUS, &success);
			if(!success)
			{
				glGetProgramInfoLog(thing, 512, NULL, info_log);
				std::cerr << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << info_log << std::endl;
				return;		
			}
			break;
	}
}

//
// Window Functions
//
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

void W_SwapAndClear(GLFWwindow *w_window, float clear_color_r, float clear_color_g, float clear_color_b, float clear_color_a)
{
	glfwSwapBuffers(w_window);
	glClearColor(clear_color_r, clear_color_g, clear_color_b, clear_color_a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void R_StoreBuffers()
{
	glGenVertexArrays(VAOS_AMOUNT, &VAOs[0]);

	unsigned int current_vao_index = VAO_DEFAULT + 1;

	for(Mesh *mesh : current_theatre->meshes)
	{
		if(mesh->vao_index != current_vao_index)
		{
			current_vao_index = mesh->vao_index;
			glBindVertexArray(VAOs[current_vao_index]);
		}

		if(mesh->vao_index == VAO_TEXTURE) // Temporary while I only have one texture coordinate vertex attribute type
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

void R_Render(std::mutex &state_mutex, double interpolation_time, glm::mat4 projection, glm::mat4 camera_view)
{
	unsigned int current_vao_index = VAOS_AMOUNT + 1; // Make sure we always switch to and bind the first used VAO

	for(Mesh *mesh : current_theatre->meshes)
	{
		if(mesh->vao_index != current_vao_index)
		{
			current_vao_index = mesh->vao_index;
			glBindVertexArray(VAOs[current_vao_index]);
		}

		glm::mat4 model_matrix = glm::mat4(1.0f);

		if(mesh->owner != NULL) // If the Mesh has no owner, this stops the engine from crashing
		{
			// Note: Quaternions (and angular movement) should use slerp instead of lerp
			std::lock_guard guard(state_mutex);

			RenderState current_state		=	mesh->owner->current_state_buffer[mesh->owner->state_index];
			RenderState previous_state		=	mesh->owner->previous_state_buffer[mesh->owner->state_index];

			glm::vec3 interpolated_position	=	current_state.render_position;
			glm::vec3 interpolated_scale	=	current_state.render_scale;

			if(do_interpolation) // Eventually, I want to change interpolation to be more like GZDoom, and this will be how I test that
			{
				for(unsigned int i = 0 ; i < 3 ; i++)
					interpolated_position[i] = std::lerp(previous_state.render_position[i], current_state.render_position[i], interpolation_time);

				for(unsigned int i = 0 ; i < 3 ; i++)
					interpolated_scale[i] = std::lerp(previous_state.render_scale[i], current_state.render_scale[i], interpolation_time);
			}

			model_matrix = glm::translate(model_matrix, interpolated_position);
			model_matrix = glm::scale(model_matrix, interpolated_scale);
		}
		shaders[current_vao_index]->use();
		shaders[current_vao_index]->setMatrix("projection", projection);
		shaders[current_vao_index]->setMatrix("camera_view", camera_view);

		unsigned int attribute_stride; // Makes this less hardcoded once I have more than two shaders, lmfao

		switch (current_vao_index)
		{
			case VAO_COLOR:
				attribute_stride = 6;
				break;

			case VAO_TEXTURE:
				attribute_stride = 5;

				// Meshes only have one texture for now
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mesh->m_texture);
				shaders[current_vao_index]->setInt("texture_one", 0);
				break;
		}

		glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->IBO);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, attribute_stride * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		// The second value is a shitty (but mildly clever?) hack!
		glVertexAttribPointer(1, (attribute_stride - 3), GL_FLOAT, GL_FALSE, attribute_stride * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		shaders[current_vao_index]->setMatrix("model", model_matrix);
		glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT, 0);
	}
}

void T_LoadTheatre(Theatre *new_theatre)
{
	current_theatre = new_theatre;
	time_to_store_buffers = true;
}