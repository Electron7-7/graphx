// File Naming Conventions:
// 1. Source code file names should clearly represent the contents of the file (i.e: code in "g_rendering_F.cpp" deals with rendering)
// 2. All file names are prepended with a single letter which acts as a "grouping tag" (i.e: in "g_rendering_F.cpp", "g" means "game")
// 3. All source files are split between "Functional" code and "Object-Orientated" code but will share header files, for convenience
// 		3a. Functional source files are appended with "_F"
// 		3b. Object-Orientated source files are appended with "_OO"
// 4. Header code may be one file specific to one pair of source files, or many files relating to one pair of source files
// 		4a. When naming the latter, the "grouping tags" should represent the name of the source files (i.e: in "r_common.hpp", "r" means "rendering")
//
// Ex:
// 		The source code for rendering is split between g_rendering_F.cpp and g_rendering_OO.cpp
// 		The header code for rendering is split between many files, including "r_common.hpp" and "r_renderer.hpp"
//		The source code for game actors is split between g_actors_F.cpp and g_actors_OO.cpp
//		The header code for game actors is all in g_actors.hpp

#define STB_IMAGE_IMPLEMENTATION
#include "sanity.hpp"
#include "r_common.hpp"
#include "g_actors.hpp"
#include "l_state.hpp"
#include <iostream>

GraphXPlayer player("Player", glm::vec3(0.0f, 0.0f, 3.0f));

u_int16_t main_window_size[2] = { 1280, 720 };
float mouse_last[2];

void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in);


int main(int argc, char* *argv)
{
	glfwInit();
	GLFWwindow *main_window = W_CreateWindow(main_window_size[0], main_window_size[1]);
	glfwSetWindowPos(main_window, static_cast<int>((1920 - main_window_size[0]) / 2), static_cast<int>((1080 - main_window_size[1]) / 2)); // HARDCODED NATIVE RESOLUTION!!! CHANGE THIS!!!

	glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(main_window, mouseCallback);

	GLShader generic_shader("src/shaders/default_vertex_shader.glsl", "src/shaders/default_fragment_shader.glsl");
	Tester tester;

	unsigned int element_buffer, vertex_array, vertex_buffer;
	glGenVertexArrays(1, &vertex_array);
	glGenBuffers(1, &element_buffer);
	glGenBuffers(1, &vertex_buffer);

	glBindVertexArray(vertex_array);

	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);

	tester.makeCube();

	glEnable(GL_DEPTH_TEST);

	mouse_last[0] = main_window_size[0] / 2.0f;
	mouse_last[1] = main_window_size[1] / 2.0f;

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	while(!glfwWindowShouldClose(main_window))
	{
		W_SwapAndClear(main_window);
		processInput(main_window);

		glm::mat4 tester_location = glm::mat4(1.0f);
		tester_location = glm::translate(tester_location, tester.position_global);
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)main_window_size[0] / (float)main_window_size[1], 0.1f, 100.0f);
		glm::mat4 camera_view = player.getViewMatrix();

		generic_shader.setMatrix("model", tester_location);
		generic_shader.setMatrix("projection", projection);
		generic_shader.setMatrix("camera_view", camera_view);
		generic_shader.use();

		glBindVertexArray(vertex_array);

		tester.flipPosition();
		tester.drawCube();

		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &vertex_array);
	glDeleteBuffers(1, &vertex_buffer);
	glDeleteBuffers(1, &element_buffer);

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) ==  GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	int input_vector[2] =
	{
		glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S),
		glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A)
	};

	player.doMovement(input_vector);
}

void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in)
{
	float x_position = static_cast<float>(x_position_in), y_position = static_cast<float>(y_position_in);
	float mouse_offset[2] = { x_position - mouse_last[0], mouse_last[1] - y_position };

	mouse_last[0] = x_position;
	mouse_last[1] = y_position;

	player.doMouseMovement(mouse_offset);
}