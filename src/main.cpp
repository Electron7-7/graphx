// Hello, production branch!
// :3
#define STB_IMAGE_IMPLEMENTATION
#include "r_common.hpp"
#include "g_actors.hpp"
#include "g_spaces.hpp"
#include "default_cube.graphxmodel"
#include "default_pyramid.graphxmodel"
#include <vector>
#include <iostream>
#include <thread>
#include <cstdlib>
#include <mutex>
#include <atomic>

GraphXPlayer player("Player", glm::vec3(0.0f, 0.0f, 3.0f));
Tester tester("tester", Mesh(VAO_TESTING, CUBE_VERTS, CUBE_INDICES));
FlipperTester flipper_tester("flipper_tester", Mesh(VAO_TESTING, CUBE_VERTS, CUBE_INDICES));
MoverTester mover_tester("mover_tester", Mesh(VAO_TESTING, PYRAMID_VERTS, PYRAMID_INDICES));

std::vector<int> main_window_size =
{
	1280,
	720
};
std::vector<float> mouse_last =
{
	main_window_size[0] / 2.0f,
	main_window_size[1] / 2.0f
};

static double TICKRATE = 5.0;
static double tickrate_ms = 1.0 / TICKRATE;	// Maybe turn this into a function to make the tickrate more easily changeable?
double last_tick_timestamp;
int updates;
int frames;

void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in);

int main()
{
	glfwInit();
	GLFWwindow *main_window = W_CreateWindow(main_window_size[0], main_window_size[1]);
	glfwSetWindowPos(main_window, static_cast<int>((1920 - main_window_size[0]) / 2), static_cast<int>((1080 - main_window_size[1]) / 2)); // HARDCODED NATIVE RESOLUTION!!! CHANGE THIS!!!
	glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(main_window, mouseCallback);
	glEnable(GL_DEPTH_TEST);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode

	std::vector<Actor *> dirty_load = {&tester, &flipper_tester, &mover_tester};
	Space test_space(dirty_load);
	loadNewSpace(&test_space);

	tester.position_global = glm::vec3(-3.0f, 0.0f, -6.0f);
	mover_tester.position_global = glm::vec3(0.0f, -3.0f, -6.0f);

	R_StoreBuffers();

	GLShader generic_shader("src/shaders/default_vertex_shader.glsl", "src/shaders/default_fragment_shader.glsl");
	double timer = glfwGetTime();

	double last_time = glfwGetTime();
	double delta_time = 0;
	double now_time = 0;

	while(!glfwWindowShouldClose(main_window))
	{

		now_time = glfwGetTime();
		delta_time += (now_time - last_time) / tickrate_ms;
		last_time = now_time;

		while(delta_time >= 1.0f)
		{
			processInput(main_window);

			for(Actor *actor : current_space->actors)
			{
				// Call the Tick() function of each Actor in std::vector<Actor> actors_in_current_space
				// Should also handle the buffering and swapping of Actor states(? or should Actors handle this?)
				actor->Tick();
				actor->updateStates();
			}
			last_tick_timestamp = glfwGetTime();
			updates++;
			delta_time--;
		}

		W_SwapAndClear(main_window);
		glfwPollEvents(); // Todo: figure out a way to move this out of here(?)
		// De-jank all of this below function shit
		double interpolation_time = ((glfwGetTime() - last_tick_timestamp) / 0.1);
		R_Render(generic_shader, interpolation_time, glm::perspective(glm::radians(45.0f), (float)main_window_size[0] / (float)main_window_size[1], 0.1f, 100.0f), player.getViewMatrix());
		frames++;

		// Reset after one second
		if (glfwGetTime() - timer > 1.0) {
			timer++;
			std::cout << "FPS: " << frames << "\nTPS: " << updates << std::endl;
			updates = 0;
			frames = 0;
		}
	}

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
	std::vector<float> m_position =
	{
		static_cast<float>(x_position_in),
		static_cast<float>(y_position_in)
	};
	std::vector<float> mouse_offset =
	{
		m_position[0] - mouse_last[0],
		mouse_last[1] - m_position[1]
	};

	mouse_last = m_position;

	player.doMouseMovement(mouse_offset);
}