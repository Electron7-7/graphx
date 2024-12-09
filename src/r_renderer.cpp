#include "sanity.hpp"
#include "r_common.hpp"
#include "g_actors.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

std::vector<GLuint> render_buffer_storage;
std::vector<GLuint> render_indices_amount_storage;
std::vector<RenderStorageCmd> render_storage_commands;

// u_int16_t main_window_size[2] = { 1280, 720 };
// float mouse_last[2] = { main_window_size[0] / 2.0f, main_window_size[1] / 2.0f };

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
GLFWwindow *W_CreateWindow(u_int16_t width, u_int16_t height, const char *title, bool make_context_current)
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
	for(int i = 0 ; i < render_storage_commands.size() ; i++)
	{
		glGenVertexArrays(1, &render_storage_commands[i].VAO);
		glGenBuffers(1, &render_storage_commands[i].VBO);
		glGenBuffers(1, &render_storage_commands[i].EBO);

		glBindVertexArray(render_storage_commands[i].VAO);
		glBindBuffer(GL_ARRAY_BUFFER, render_storage_commands[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, render_storage_commands[i].vertices.size() * sizeof(float), &render_storage_commands[i].vertices[0], GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, render_storage_commands[i].EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, render_storage_commands[i].indices.size() * sizeof(unsigned int), &render_storage_commands[i].indices[0], GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		render_buffer_storage.push_back(render_storage_commands[i].VAO);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
}

void R_Render(GLShader current_shader)
{
	for(int i = 0 ; i < render_buffer_storage.size() ; i++)
	{
		glBindVertexArray(render_buffer_storage[i]);

		glm::mat4 model_position = glm::mat4(1.0f);
		model_position = glm::translate(model_position, renderables[i]->position_global);
		current_shader.setMatrix("model", model_position);

		glDrawElements(GL_TRIANGLES, render_indices_amount_storage[i], GL_UNSIGNED_INT, 0);
	}
}