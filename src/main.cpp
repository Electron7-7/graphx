// Hello, production branch!
// :3
#define STB_IMAGE_IMPLEMENTATION
#include "r_common.hpp"
#include "g_actors.hpp"
#include "g_theatre.hpp"
#include "cube.graphxmodel"
#include "pyramid.graphxmodel"
// #include "test_theatre.graphxtheatre"
#include <vector>
#include <thread>
#include <cstdlib>
#include <mutex>

std::mutex actor_state_mutex;

GraphXPlayer player("Player", glm::vec3(0.0f, 1.0f, 0.0f));

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

static double TICKRATE = 120.0;
static double tickrate_ms = 1.0 / TICKRATE;	// Maybe turn this into a function to make the tickrate more easily changeable?

double last_tick_timestamp;

void _debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in);
void testGameTick(GLFWwindow *window);

int main()
{
	glfwInit();
	GLFWwindow *main_window = W_CreateWindow(main_window_size[0], main_window_size[1]);
	glfwSetWindowPos(main_window, static_cast<int>((1920 - main_window_size[0]) / 2), static_cast<int>((1080 - main_window_size[1]) / 2)); // HARDCODED NATIVE RESOLUTION!!! CHANGE THIS!!!
	glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(main_window, mouseCallback);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(_debug_callback, nullptr);
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode

	std::thread game_logic_main_thread(testGameTick, main_window);

	GLShader generic_shader("src/shaders/default_vertex_shader.glsl", "src/shaders/default_fragment_shader.glsl");

	while(!glfwWindowShouldClose(main_window))
	{
		W_SwapAndClear(main_window);
		glfwPollEvents();

		if(time_to_store_buffers)
			R_StoreBuffers();

		if(time_to_render)
		{
			// De-jank all of this shit below
			glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)main_window_size[0] / (float)main_window_size[1], 0.1f, 100.0f);
			double interpolation_time = ((glfwGetTime() - last_tick_timestamp) / tickrate_ms);
			R_Render(actor_state_mutex, generic_shader, interpolation_time, projection, player.getViewMatrix());
		}
	}

	game_logic_main_thread.join();
	glfwTerminate();
	return 0;
}

void testGameTick(GLFWwindow *main_window)
{
	MoverTester mover_tester("mover_tester", Mesh(&mover_tester, BUFFER_ACTORS, PYRAMID_VERTS, PYRAMID_INDICES), glm::vec3(0.0f, 1.0f, -6.0f));
	Actor static_tester("static_tester", Mesh(&static_tester, BUFFER_ACTORS, CUBE_VERTS, CUBE_INDICES), glm::vec3(-2.0f, -2.0f, -6.0f));
	Theatre test_theatre("test_theatre", std::vector<Actor *> {&mover_tester, &static_tester});

	current_theatre = &test_theatre;
	time_to_store_buffers = true;

	int tick = 0;
	double last_time = glfwGetTime();
	double tick_length = 0;
	double now_time = 0;

	while(!glfwWindowShouldClose(main_window))
	{
		tick++;

		now_time = glfwGetTime();
		tick_length += (now_time - last_time) / tickrate_ms;
		last_time = now_time;

		while(tick_length >= 1.0f)
		{
			processInput(main_window);

			for(Actor *actor : current_theatre->actors)
			{
				// Call the Tick() function of each Actor in std::vector<Actor> actors_in_current_theatre
				// Should also handle the buffering and swapping of Actor states(? or should Actors handle this?)
				actor->Tick(tick);
				actor->updateStates(actor_state_mutex);
			}

			last_tick_timestamp = glfwGetTime();
			tick_length--;
		}

		if(tick >= TICKRATE)
			tick = 0;
	}

	time_to_render = false; // Because game logic can (and usually does) exit before the main loop
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

void _debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]() {
		switch (source)
		{
		case GL_DEBUG_SOURCE_API: return "API";
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
		case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
		case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
		case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
		case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
	}();

	auto const type_str = [type]() {
		switch (type)
		{
		case GL_DEBUG_TYPE_ERROR: return "ERROR";
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR: return "DEPRECATED_BEHAVIOR";
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: return "UNDEFINED_BEHAVIOR";
		case GL_DEBUG_TYPE_PORTABILITY: return "PORTABILITY";
		case GL_DEBUG_TYPE_PERFORMANCE: return "PERFORMANCE";
		case GL_DEBUG_TYPE_MARKER: return "MARKER";
		case GL_DEBUG_TYPE_OTHER: return "OTHER";
		}
	}();

	auto const severity_str = [severity]() {
		switch (severity) {
		case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
		case GL_DEBUG_SEVERITY_LOW: return "LOW";
		case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
		case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
	}();
	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}