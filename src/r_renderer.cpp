#include "g_spaces.hpp"
#include "r_common.hpp"
#include "g_math.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <typeinfo>

std::array<GLuint, VAOS_AMOUNT> vertex_array_objects;
std::vector<GLuint> shaders;
std::vector<Mesh *> meshes;

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
	VAO_ID_ModifiedBubbleSort(meshes);

	int current_vao = -1;
	glGenVertexArrays(VAOS_AMOUNT, &vertex_array_objects[0]);

	int storage_commands_size = meshes.size();

	for(int i = 0 ; i < storage_commands_size ; i++)
	{
		if(meshes[i]->vao_id != current_vao)
		{
			current_vao++;
			glBindVertexArray(vertex_array_objects[current_vao]);
		}

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

void R_Render(GLShader *current_shader, std::mutex *state_mutex, double interpolation_time, glm::mat4 projection, glm::mat4 camera_view)
{
	current_shader->use();
	current_shader->setMatrix("projection", projection);
	current_shader->setMatrix("camera_view", camera_view);

	VAO_ID_ModifiedBubbleSort(current_space->actors);

	int current_vao_index = -1;
	for(Actor *actor : current_space->actors)
	{
		if(actor->vao_id != current_vao_index)
		{
			current_vao_index++;
			glBindVertexArray(vertex_array_objects[current_vao_index]);
		}
		std::lock_guard<std::mutex> guard(*state_mutex);

		/*
		Pseudo Code
			Lerp(previous_state[state_index].position, current_state[state_index].position, elapsed/update_tick_length)
		*/
		glm::vec3 current_position = actor->current_state_buffer[actor->state_index].render_position;
		glm::vec3 previous_position = actor->previous_state_buffer[actor->state_index].render_position;
		glm::vec3 interpolated_position = glm::vec3(0.0f);

		interpolated_position.x = std::lerp(previous_position.x, current_position.x, interpolation_time);
		interpolated_position.y = std::lerp(previous_position.y, current_position.y, interpolation_time);
		interpolated_position.z = std::lerp(previous_position.z, current_position.z, interpolation_time);

		if(strcmp(typeid(*actor).name(), "11MoverTester") == 0)
		{
			std::cout << std::endl << std::endl << "----> Current Position: " << glm::to_string(current_position) << "\n----> Last Position: " << glm::to_string(previous_position) << std::endl << std::endl;
			std::cout << "----> Interpolated Position: " << glm::to_string(interpolated_position) << std::endl << std::endl;
		}

		glm::mat4 model_position = glm::mat4(1.0f);
		model_position = glm::translate(model_position, interpolated_position);

		current_shader->setMatrix("model", model_position);
		glDrawElements(GL_TRIANGLES, actor->mesh.indices_amount, GL_UNSIGNED_INT, 0);
	}
}