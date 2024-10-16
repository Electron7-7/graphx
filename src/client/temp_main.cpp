#include "include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include "common.cpp"

void framebufferSizeCallback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

unsigned int window_width = 800, window_height = 600;

const char *bill_cipher_vertex_shader_source = "#version 460 core\n"
	"layout (location = 0) in vec3 aPos;\n"
	"void main()\n"
	"{\n"
	"	gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
	"}\0";

const char *bill_cipher_fragment_shader_source = "#version 460 core\n"
	"out vec4 FragColor;\n"
	"void main()\n"
	"{\n"
	"	FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
	"}\n\0";

int main(int argc, char** argv)
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window = glfwCreateWindow(window_width, window_height, "Fucking Graphics", NULL, NULL);
	
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
	
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	unsigned int bill_cipher_vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(bill_cipher_vertex_shader, 1, &bill_cipher_vertex_shader_source, NULL);
	glCompileShader(bill_cipher_vertex_shader);

	shaderErrorHandler(bill_cipher_vertex_shader);

	unsigned int bill_cipher_fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(bill_cipher_fragment_shader, 1, &bill_cipher_fragment_shader_source, NULL);
	glCompileShader(bill_cipher_fragment_shader);

	shaderErrorHandler(bill_cipher_fragment_shader);

	unsigned int bill_cipher_shader_program = glCreateProgram();
	glAttachShader(bill_cipher_shader_program, bill_cipher_vertex_shader);
	glAttachShader(bill_cipher_shader_program, bill_cipher_fragment_shader);
	glLinkProgram(bill_cipher_shader_program);

	shaderErrorHandler(bill_cipher_shader_program, true);

	glDeleteShader(bill_cipher_vertex_shader);
	glDeleteShader(bill_cipher_fragment_shader);

	float bill_cipher_vertices[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		0.0f, 0.5f, 0.0f
	};

	unsigned int VBO, VAO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(bill_cipher_vertices), bill_cipher_vertices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	while(!glfwWindowShouldClose(window))
	{
		processInput(window);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(bill_cipher_shader_program);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(bill_cipher_shader_program);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}