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

std::mutex actor_state_mutex;

GraphXPlayer player("Player", glm::vec3(0.0f, 0.0f, 3.0f));

Tester tester("tester", Mesh(VAO_TESTING, CUBE_VERTS, CUBE_INDICES));
FlipperTester flipper_tester("flipper_tester", Mesh(VAO_TESTING, CUBE_VERTS, CUBE_INDICES));
MoverTester mover_tester("mover_tester", Mesh(VAO_TESTING, PYRAMID_VERTS, PYRAMID_INDICES));

std::vector<Actor *> dirty_load = {&tester, &flipper_tester, &mover_tester};
Space test_space(dirty_load);

void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in);
void testGameLogic(GLFWwindow *the_main_window);

int main_window_size[2] = { 1280, 720 };
float mouse_last[2] = { main_window_size[0] / 2.0f, main_window_size[1] / 2.0f };

static double TICKRATE = 70.0;
static double tickrate_ms = 1.0 / TICKRATE;

int main()
{
	glfwInit();
	GLFWwindow *main_window = W_CreateWindow(main_window_size[0], main_window_size[1]);
	glfwSetWindowPos(main_window, static_cast<int>((1920 - main_window_size[0]) / 2), static_cast<int>((1080 - main_window_size[1]) / 2)); // HARDCODED NATIVE RESOLUTION!!! CHANGE THIS!!!

	glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(main_window, mouseCallback);

	GLShader generic_shader("src/shaders/default_vertex_shader.glsl", "src/shaders/default_fragment_shader.glsl");

	glEnable(GL_DEPTH_TEST);

	mouse_last[0] = main_window_size[0] / 2.0f;
	mouse_last[1] = main_window_size[1] / 2.0f;

	loadNewSpace(&test_space);

	R_StoreBuffers();

	std::thread game_logic_thread(testGameLogic, main_window);
	
	tester.position_global = glm::vec3(-3.0f, 0.0f, -6.0f);
	mover_tester.position_global = glm::vec3(0.0f, -3.0f, -6.0f);

	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	while(!glfwWindowShouldClose(main_window))
	{
		W_SwapAndClear(main_window);
		processInput(main_window);
		generic_shader.use();

		glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)main_window_size[0] / (float)main_window_size[1], 0.1f, 100.0f);
		glm::mat4 camera_view = player.getViewMatrix();

		generic_shader.setMatrix("projection", projection);
		generic_shader.setMatrix("camera_view", camera_view);

		R_Render(generic_shader);

		glfwPollEvents();
	}

	glfwTerminate();
	game_logic_thread.join();
	return 0;
}

void testGameLogic(GLFWwindow *the_main_window)
{
	double last_time = glfwGetTime(), timer = last_time;
	double delta_time = 0, now_time = 0;
	int updates = 0;

	while(!glfwWindowShouldClose(the_main_window))
	{
		now_time = glfwGetTime();
		delta_time += (now_time - last_time) / tickrate_ms;
		last_time = now_time;

		while(delta_time >= 1.0f)
		{
			// Call the Tick() function of each Actor in std::vector<Actor> actors_in_current_space
			// Should also handle the buffering and swapping of Actor states(? or should Actors handle this?)
			for(Actor *actor : current_space->actors)
			{
				actor->Tick();
				actor->updateStates(&actor_state_mutex);
			}

			updates++;
			delta_time--;
		}

		if (glfwGetTime() - timer > 1.0) {
            timer++;
            std::cout << " Updates:" << updates << std::endl;
            updates = 0;
        }
	}
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