// rendering.cpp
// Type: Object-Orientated
// Description: Various classes for rendering
#include "sanity.hpp"
#include "r_common.hpp"
#include <iostream>
#include <fstream>
#include <sstream>

//
// Shader
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
// Debug Camera (not for in-game use)
//
DebugCamera::DebugCamera(glm::vec3 init_position, glm::vec3 init_up, float init_yaw, float init_pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY)
{
	position = init_position;
	world_up = init_up;
	yaw = init_yaw;
	pitch = init_pitch;

	updateCameraVectors();
}

glm::mat4 DebugCamera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

void DebugCamera::doMovement(int direction[2], float delta_time)
{
	position += front * static_cast<float>(direction[0] * movement_speed * delta_time);
	position += right * static_cast<float>(direction[1] * movement_speed * delta_time);
}

void DebugCamera::doMouseMovement(float offset[2], GLboolean constrain_pitch)
{
	offset[0] *= mouse_sensitivity;
	offset[1] *= mouse_sensitivity;

	yaw += offset[0];
	pitch += offset[1];

	if(constrain_pitch)
	{
		if(std::abs(pitch) > 89.0f)
			pitch = 89.0f * ((pitch > 0) - (pitch < 0));
	}
	updateCameraVectors();
}

void DebugCamera::updateCameraVectors()
{
	glm::vec3 new_front;
	new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	new_front.y = sin(glm::radians(pitch));
	new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	
	front = glm::normalize(new_front);
	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}

//
// Cube
//
void Cube::makeCube()
{
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Idea: set GL_FALSE to GL_TRUE and use ints instead of floats for more efficient storage?
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	unsigned int c_texture = T_GenerateTexture("src/images/COMP04_5.png");
	glBindTexture(GL_TEXTURE_2D, c_texture);
}

void Cube::drawCube()
{
	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
}
