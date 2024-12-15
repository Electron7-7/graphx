#include "g_theatre.hpp"
#include "r_common.hpp"
#include "g_math.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

std::array<GLuint, VAOS_AMOUNT> vertex_array_objects;
std::vector<GLuint> shaders;
std::vector<Mesh *> meshes;
std::atomic_bool time_to_render = false;
std::atomic_bool time_to_store_buffers = false;
std::atomic_bool do_interpolation = true; // For testing when I change the interpolation method to be more like GZDoom

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

void R_AddBufferToStore(Actor *new_actor)
{
	std::cout << "Adding buffer to store" << std::endl;
	meshes.push_back(&new_actor->mesh);
	R_StoreBuffers(false);
}

void R_StoreBuffers(bool changing_to_new_theatre)
{
	if(changing_to_new_theatre)
	{
		for(auto &actor : current_theatre->actors)
		{
			meshes.push_back(&actor->mesh);
		}

		time_to_render = true;
		time_to_store_buffers = false;
	}

	gmath::VAO_ID_ModifiedBubbleSort(meshes);

	int current_vao = -1;

	int meshes_size = meshes.size();

	for(int i = 0 ; i < meshes_size ; i++)
	{
		if(meshes[i]->vao_id != current_vao)
		{
			current_vao++;
			glBindVertexArray(vertex_array_objects[current_vao]);
		}

		meshes[i]->generateTexture(); // Quickly generate the texture in the render thread

		glGenBuffers(1, &meshes[i]->VBO);
		glGenBuffers(1, &meshes[i]->EBO);

		glBindBuffer(GL_ARRAY_BUFFER, meshes[i]->VBO);
		glBufferData(GL_ARRAY_BUFFER, meshes[i]->vertices.size() * sizeof(float), &meshes[i]->vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshes[i]->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, meshes[i]->indices.size() * sizeof(unsigned int), &meshes[i]->indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	std::vector<Mesh *>().swap(meshes);	// This vector is only used once per level load, so free up any allocated memory
}

void R_Render(std::mutex &state_mutex, GLShader &current_shader, double interpolation_time, glm::mat4 projection, glm::mat4 camera_view)
{
	/*
	Pseudo Code for Lerp
		Lerp(previous_state[state_index].position, current_state[state_index].position, elapsed/update_tick_length)
	*/

	current_shader.use();
	current_shader.setMatrix("projection", projection);
	current_shader.setMatrix("camera_view", camera_view);

	gmath::VAO_ID_ModifiedBubbleSort(current_theatre->actors);

	int current_vao_index = -1;
	for(Actor *actor : current_theatre->actors)
	{
		if(actor->vao_id != current_vao_index)
		{
			current_vao_index++;
			glBindVertexArray(vertex_array_objects[current_vao_index]);
		}

		std::lock_guard guard(state_mutex);
		// Meshes only have one texture right now, but when they don't, I'll need to make this iterative; as it stands, this is
		// extremely hard-coded.
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, actor->mesh.m_texture);
		current_shader.setInt("texture_one", 0);

		// Quick note for later: angular movement (orientation) should use slerp instead of lerp (quaternions are best)
		glm::vec3 previous_position = actor->previous_state_buffer[actor->state_index].render_position;
		glm::vec3 current_position = actor->current_state_buffer[actor->state_index].render_position;
		glm::vec3 interpolated_position;
		
		for(int i = 0 ; i < 3 ; i++) // I don't like how hardcoded this is
			interpolated_position[i] = std::lerp(previous_position[i], current_position[i], interpolation_time);

		if(!do_interpolation)
			interpolated_position = current_position;

		glm::mat4 model_position = glm::mat4(1.0f);
		model_position = glm::translate(model_position, interpolated_position);

		current_shader.setMatrix("model", model_position);
		glDrawElements(GL_TRIANGLES, actor->mesh.indices_amount, GL_UNSIGNED_INT, 0);
	}
}