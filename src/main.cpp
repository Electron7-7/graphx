// Hello, production branch!
// :3

#define STB_IMAGE_IMPLEMENTATION
#include "sanity.hpp"
// #include "graphx_namespace.hpp"
#include "r_common.hpp"
#include "g_common.hpp"
#include "g_actors.hpp"
#include "g_jolt.hpp"
#include "g_imgui.hpp"
#include "t_common.hpp"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <theatres.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <cstdarg>
#include <thread>
#include <mutex>

std::mutex actor_state_mutex;

glm::vec2 main_window_size(1280, 720);
glm::vec2 mouse_last(main_window_size / 2.0f);

static int TICKRATE = 120;

int current_tick_since_second = 0;
long current_tick_since_start = 0;
double last_tick_timestamp = 0;
bool test_flashlight_bool = false;
bool red_flashlight_color_bool = false;
bool do_jolt_assert = false;
bool debug_console_open = false;

float camera_near = 0.1f;
float camera_far = 1000.0f;

void processInput(GLFWwindow *window);
void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void testGameTick(GLFWwindow *window);

#define TICKLENGTH (1.0f / TICKRATE)
#define PER_SECOND(interval) (current_tick_since_second % (TICKRATE/interval) == 0)

int main()
{
	glfwInit();
	GLFWwindow *main_window = W_CreateWindow(main_window_size[0], main_window_size[1]);
	const GLFWvidmode *primary_monitor_video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int primary_monitor_xposition = 0;
	int primary_monitor_yposition = 0;
	glfwGetMonitorPos(glfwGetPrimaryMonitor(), &primary_monitor_xposition, &primary_monitor_yposition);
	glfwSetWindowPos(main_window, static_cast<int>(((primary_monitor_video_mode->width - main_window_size[0]) / 2) + primary_monitor_xposition), static_cast<int>(((primary_monitor_video_mode->height - main_window_size[1]) / 2) + primary_monitor_yposition));
#ifdef GRAPHX_DEBUG
	glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // When using lldb, I enable this line to keep the mouse cursor from getting stuck disabled
#else
	glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
	glfwSetCursorPosCallback(main_window, mouseCallback);
	glfwSetKeyCallback(main_window, keyCallback);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);
	// glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE); // Disable notifications
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	
	glGenVertexArrays(VAOS_AMOUNT, &VAOs[0]);

	GLShader phong_shader(phong_vertex_glsl, phong_fragment_glsl);
	shaders.insert(shaders.end(), {&phong_shader});

	std::thread game_logic_main_thread(testGameTick, main_window);

	//------------------------
	// Start ImGui Boilerplate
	//------------------------

	GraphXConsole graphx_debug_console;

	graphx_debug_console.active = (glfwGetInputMode(main_window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL);
	graphx_debug_console.current_theatre = &current_theatre;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

#ifdef WIN32
	ImGui_ImplWin32_Init();
#endif
	ImGui_ImplGlfw_InitForOpenGL(main_window, true);
	ImGui_ImplOpenGL3_Init();

	//----------------------
	// End ImGui Boilerplate
	//----------------------

	while(!glfwWindowShouldClose(main_window))
	{
		W_SwapAndClear(main_window, getCurrentEnvironment()->getAmbientLight());
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		graphx_debug_console.updateFrame(main_window);

		if(graphx_debug_console.justClosed())
			toggleCursor(main_window, false);

		if(time_to_store_buffers)
			R_StoreBuffers();

		if(time_to_render)
		{
			// De-jank all of this shit below
			glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), (float)main_window_size[0] / (float)main_window_size[1], camera_near, camera_far);
			float interpolation_time = ((glfwGetTime() - last_tick_timestamp) / TICKLENGTH);
			R_Render(actor_state_mutex, interpolation_time, projection_matrix);
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	game_logic_main_thread.join();
	glfwTerminate();
	return 0;
}

// The Jolt Physics boilerplate code was really annoying to scroll through, so I isolated it
#include "jolt_boilerplate.hpp"

void testGameTick(GLFWwindow *main_window)
{
	JPH::RegisterDefaultAllocator();
	JPH::Factory::sInstance = new JPH::Factory();
	JPH::RegisterTypes();

	JPH::Trace = GraphXJoltTrace;
	JPH_IF_ENABLE_ASSERTS(JPH::AssertFailed = GraphXJoltAssertFailed;)

#ifdef GRAPHX_DEBUG
	GraphXBodyActivationListener body_activation_listener;
	jolt_physics_system.SetBodyActivationListener(&body_activation_listener);

	GraphXContactListener contact_listener;
	jolt_physics_system.SetContactListener(&contact_listener);
#endif

	JPH::TempAllocatorImpl jolt_temp_allocator(10 * 1024 * 1024);
	JPH::JobSystemThreadPool jolt_job_system(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, std::thread::hardware_concurrency() - 1);

	const JPH::uint cMaxBodies = 2048;
	const JPH::uint cNumBodyMutexes = 0;
	const JPH::uint cMaxBodyPairs = 2048;
	const JPH::uint cMaxContactConstraints = 2048;

	GraphXBroadPhaseLayerInterface broad_phase_layer_interface;
	GraphXObjectVsBroadPhaseLayerFilter object_vs_broadphase_layer_filter;
	GraphXObjectLayerPairFilter object_vs_object_layer_filter;

	jolt_physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broad_phase_layer_interface, object_vs_broadphase_layer_filter, object_vs_object_layer_filter);

	checkForAndLoadExternalTheatres();

	loadMainTheatre(0); // Hard-coded Theatre loading; later I want to make a "level list" of some sort

	double last_time = glfwGetTime();
	double current_tick_length = 0;
	double now_time = 0;

	while(!glfwWindowShouldClose(main_window))
	{
		now_time = glfwGetTime();
		current_tick_length += (now_time - last_time) / TICKLENGTH;
		last_time = now_time;

		while(current_tick_length >= 1.0f)
		{
			current_tick_since_second++;
			current_tick_since_start++;

			processInput(main_window);

			// TICK(current_tick_since_second) // Prints current tick (looping from 0 to TICKRATE)
			for(Actor *actor : getCurrentTheatre()->troupe)
			{
				actor->tick(current_tick_since_start);
				actor->updateStates(actor_state_mutex);
			}

			LightFlashlight *player_flashlight = iKnowWhatActorIWant<LightFlashlight *>(std::string("Player_Flashlight"));

			player_flashlight->setLight(test_flashlight_bool);

			if(red_flashlight_color_bool)
				player_flashlight->light_color = glm::vec3(1.0f, 0.0f, 0.0f);
			else
				player_flashlight->light_color = glm::vec3(1.0f);

			if(!loading_new_main_theatre)
				jolt_physics_system.Update(TICKLENGTH, 1, &jolt_temp_allocator, &jolt_job_system);

			last_tick_timestamp = glfwGetTime();
			current_tick_length--;
		}

		if(current_tick_since_second >= TICKRATE)
			current_tick_since_second = 0;
	}

	time_to_render = false; // Because game logic can (and usually does) exit before the main loop

	getCurrentTheatre()->dropCurtains();

	JPH::UnregisterTypes();
	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = nullptr;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(ImGui::GetIO().WantCaptureKeyboard)
		return;

	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		if(loading_new_main_theatre)
			return;
		long current_theatre = getCurrentTheatre()->getUID();
		for(auto it = embedded_theatres.begin() ; it != embedded_theatres.end() ; it++)
		{
			if(it->first == current_theatre)
			{
				++it;
				if(it == embedded_theatres.end())
				{
					loadMainTheatre(0);
					return;
				}
				loadMainTheatre(it->first);
				return;
			}
		}
	}

	if(key == GLFW_KEY_LEFT && action == GLFW_PRESS)
	{
		if(loading_new_main_theatre)
			return;
		checkForAndLoadExternalTheatres();
		long current_theatre = getCurrentTheatre()->getUID();
		for(auto it = embedded_theatres.begin() ; it != embedded_theatres.end() ; it++)
		{
			if(it->first == current_theatre)
			{
				if(it == embedded_theatres.begin())
				{
					auto end_it = embedded_theatres.end();
					--end_it;
					loadMainTheatre(end_it->first);
					return;
				}

				--it;
				loadMainTheatre(it->first);
				return;
			}
		}
	}

	if(key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		getCurrentEnvironment()->ambient_lighting_enabled = !getCurrentEnvironment()->ambient_lighting_enabled;
		if(!getCurrentEnvironment()->ambient_lighting_enabled)
			PRINTDEBUG("Ambient Lighting Disabled")
		else
			PRINTDEBUG("Ambient Lighting Enabled")
	}

	if(key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		test_flashlight_bool = !test_flashlight_bool;
		if(test_flashlight_bool)
			PRINTDEBUG("Flashlight Off")
		else
			PRINTDEBUG("Flashlight On")
	}

	if(key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		red_flashlight_color_bool = !red_flashlight_color_bool;
		if(red_flashlight_color_bool)
			PRINTDEBUG("Flashlight Red")
		else
			PRINTDEBUG("Flashlight Not Red")
	}

	if(key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		PRINTDEBUG("Resetting PhysicsActors to initial transformation!")
		for(Actor *actor : getCurrentTheatre()->troupe)
			if(actor->isPhysicsActor())
				static_cast<PhysicsActor *>(actor)->reset_to_initial_orientation_for_testing();
	}

	if(key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
		{
			PRINTDEBUG("Cursor Mode: Disabled (hidden + locked at center)")
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			return;
		}

		PRINTDEBUG("Cursor Mode: Normal (cursor visible & camera ignoring movement)")
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}

#ifdef GRAPHX_DEBUG
	if(key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		do_jolt_assert = !do_jolt_assert;
		if(do_jolt_assert)
			PRINTDEBUG("Jolt assert printouts enabled")
		else
			PRINTDEBUG("Jolt assert printouts disabled")
	}
#endif
}

void toggleCursor(GLFWwindow *window, bool show_cursor)
{
	if(!show_cursor)
	{
		PRINTDEBUG("Cursor Mode: Disabled (hidden + locked at center)")
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		return;
	}

	PRINTDEBUG("Cursor Mode: Normal (cursor visible & camera ignoring movement)")
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

// This will be put in Actor once I abstract "glfwGetKey" and related functions
void processInput(GLFWwindow *window)
{
	if(ImGui::GetIO().WantCaptureKeyboard)
		return;

	int input_vector[2] =
	{
		glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S),
		glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A)
	};

	if(!loading_new_main_theatre)
		getCurrentPlayer()->doMovement(input_vector);
}

void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in)
{
	if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL || ImGui::GetIO().WantCaptureMouse)
		return;

	glm::vec2 mouse_position(static_cast<float>(x_position_in), static_cast<float>(y_position_in));
	glm::vec2 mouse_offset = mouse_position - mouse_last;
	mouse_last = mouse_position;
	
	if(!loading_new_main_theatre)
		getCurrentPlayer()->doMouseMovement(mouse_offset);
}

int WinMain() // Fuck off, Windows
{
	main();
	system("pause");
	return 0;
}