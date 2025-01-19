// Hello, production branch!
// :3
#define STB_IMAGE_IMPLEMENTATION
#include "sanity.hpp"
#include "r_common.hpp"
#include "g_actors.hpp"
#include "g_theatre.hpp"
#include "theatres/lighting_testing.graphxtheatre"
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

GraphXPlayer player("Player", glm::vec3(0.0f, 3.0f, 0.0f));
Environment default_environment(true);

std::mutex actor_state_mutex;

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

int current_tick_since_second = 0;
long current_tick_since_start = 0;
double last_tick_timestamp = 0;
bool test_flashlight_bool = false;

void GLAPIENTRY _debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);
void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void testGameTick(GLFWwindow *window);

int main()
{
	glfwInit();
	GLFWwindow *main_window = W_CreateWindow(main_window_size[0], main_window_size[1]);
	const GLFWvidmode *primary_monitor_video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int primary_monitor_xposition = 0;
	int primary_monitor_yposition = 0;
	glfwGetMonitorPos(glfwGetPrimaryMonitor(), &primary_monitor_xposition, &primary_monitor_yposition);
	glfwSetWindowPos(main_window, static_cast<int>(((primary_monitor_video_mode->width - main_window_size[0]) / 2) + primary_monitor_xposition), static_cast<int>(((primary_monitor_video_mode->height - main_window_size[1]) / 2) + primary_monitor_yposition));
	glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(main_window, mouseCallback);
	glfwSetKeyCallback(main_window, keyCallback);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(_debug_callback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE); // Disable notifications
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	
	glGenVertexArrays(VAOS_AMOUNT, &VAOs[0]);
	
	GLShader phong_shader(SRC_DIR("src/shaders/phong_vertex.glsl").c_str(), SRC_DIR("src/shaders/phong_fragment.glsl").c_str());
	shaders.insert(shaders.end(), {&phong_shader});

	current_player = &player;

	std::thread game_logic_main_thread(testGameTick, main_window);

	while(!glfwWindowShouldClose(main_window))
	{
		W_SwapAndClear(main_window, default_environment.getAmbientLight());
		glfwPollEvents();

		if(time_to_store_buffers)
			R_StoreBuffers();

		if(time_to_render)
		{
			// De-jank all of this shit below
			glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), (float)main_window_size[0] / (float)main_window_size[1], 0.1f, 100.0f);
			double interpolation_time = ((glfwGetTime() - last_tick_timestamp) / tickrate_ms);
			R_Render(actor_state_mutex, interpolation_time, projection_matrix, &default_environment);
		}
	}

	game_logic_main_thread.join();
	glfwTerminate();
	return 0;
}

void testGameTick(GLFWwindow *main_window)
{
	current_theatre = &lighting_testing_theatre;
	time_to_store_buffers = true;

	double last_time = glfwGetTime();
	double tick_length = 0;
	double now_time = 0;

	while(!glfwWindowShouldClose(main_window))
	{
		now_time = glfwGetTime();
		tick_length += (now_time - last_time) / tickrate_ms;
		last_time = now_time;

		while(tick_length >= 1.0f)
		{
			current_tick_since_second++;
			current_tick_since_start++;

			processInput(main_window);

			for(Actor *actor : current_theatre->troupe)
			{
				// Call the Tick() function of each Actor in std::vector<Actor> actors_in_current_theatre
				// Should also handle the buffering and swapping of Actor states(? or should Actors handle this?)
				actor->Tick(current_tick_since_start);
				actor->updateStates(actor_state_mutex);
			}

			player_flashlight.setLight(test_flashlight_bool);

			last_tick_timestamp = glfwGetTime();
			tick_length--;
		}

		if(current_tick_since_second >= TICKRATE)
			current_tick_since_second = 0;
	}

	time_to_render = false; // Because game logic can (and usually does) exit before the main loop
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if(key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		default_environment.ambient_lighting_enabled = !default_environment.ambient_lighting_enabled;
		if(!default_environment.ambient_lighting_enabled)
			PRINT("Ambient Lighting Disabled");
		else
			PRINT("Ambient Lighting Enabled");
	}

	if(key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		test_flashlight_bool = !test_flashlight_bool;
		if(test_flashlight_bool)
			PRINT("Flashlight Off");
		else
			PRINT("Flashlight On");
	}
}

void processInput(GLFWwindow *window)
{
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

void GLAPIENTRY _debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param)
{
	auto const src_str = [source]()
	{
		switch (source)
		{
			case GL_DEBUG_SOURCE_API: return "API";
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM: return "WINDOW SYSTEM";
			case GL_DEBUG_SOURCE_SHADER_COMPILER: return "SHADER COMPILER";
			case GL_DEBUG_SOURCE_THIRD_PARTY: return "THIRD PARTY";
			case GL_DEBUG_SOURCE_APPLICATION: return "APPLICATION";
			case GL_DEBUG_SOURCE_OTHER: return "OTHER";
		}
		return "N/A";
	}();

	auto const type_str = [type]()
	{
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
		return "N/A";
	}();

	auto const severity_str = [severity]()
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_NOTIFICATION: return "NOTIFICATION";
			case GL_DEBUG_SEVERITY_LOW: return "LOW";
			case GL_DEBUG_SEVERITY_MEDIUM: return "MEDIUM";
			case GL_DEBUG_SEVERITY_HIGH: return "HIGH";
		}
		return "N/A";
	}();

	std::cout << src_str << ", " << type_str << ", " << severity_str << ", " << id << ": " << message << '\n';
}

int WinMain() // Fuck off, Windows
{
	main();
	system("pause");
	return 0;
}