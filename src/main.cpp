// Hello, production branch!
// :3

#define STB_IMAGE_IMPLEMENTATION
#include "sanity.hpp"
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

static int TICKRATE = 120;

int current_tick_since_second = 0;
long current_tick_since_start = 0;
double last_tick_timestamp = 0;
bool do_jolt_assert = false;
bool debug_console_open = false;
bool is_wireframe = false;

double cursor_last_x = 0.0;
double cursor_last_y = 0.0;

void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void testGameTick(GLFWwindow *window);

#define TICKLENGTH (1.0f / TICKRATE)
#define PER_SECOND(interval) (current_tick_since_second % (TICKRATE/interval) == 0)

int main()
{
	graphx_api = GRAPHX_OPENGL;

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
	glEnable(GL_FRAMEBUFFER_SRGB);
	// glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE); // Disable notifications
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode

	glGenVertexArrays(VAOS_AMOUNT, &VAOs[0]);

	GLShader blinn_phong_shader(blinn_phong_vertex_glsl, blinn_phong_fragment_glsl);
	GLShader phong_shader(phong_vertex_glsl, phong_fragment_glsl);
	shaders.insert(shaders.end(), {&blinn_phong_shader, &phong_shader});

	std::thread game_logic_main_thread(testGameTick, main_window);

	//------------------------
	// Start ImGui Boilerplate
	//------------------------

	GraphXConsole graphx_debug_console;

	graphx_debug_console.active = (glfwGetInputMode(main_window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL);
	graphx_debug_console.current_theatre = &current_theatre;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::GetIO().IniFilename = nullptr;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::GetIO().IniFilename = NULL; // Be rid of imgui.ini (for now)

	ImGui_ImplGlfw_InitForOpenGL(main_window, true);
	ImGui_ImplOpenGL3_Init();

	//----------------------
	// End ImGui Boilerplate
	//----------------------

	while(!glfwWindowShouldClose(main_window))
	{
		glm::vec3 swap_color = current_theatre.getSwapColor();
		W_SwapAndClear(main_window, swap_color);
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
			float interpolation_time = ((glfwGetTime() - last_tick_timestamp) / TICKLENGTH);
			R_Render(actor_state_mutex, interpolation_time);
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

	// Hard-coded first Theatre loading, for now
	// Eventually, Theatre loading won't rely on this function and the arrow key callbacks
	loadMainTheatre(0);

	double last_time = glfwGetTime();
	double current_tick_length = 0;
	double now_time = 0;

	while(!glfwWindowShouldClose(main_window))
	{
		now_time = glfwGetTime();
		current_tick_length += (now_time - last_time) / TICKLENGTH;
		last_time = now_time;

		while(current_tick_length >= 1.0f && !loading_new_main_theatre)
		{
			current_tick_since_second++;
			current_tick_since_start++;

			for(Actor *actor : getCurrentTheatre()->troupe)
			{
				actor->processInput(main_window);
				actor->tick(current_tick_since_start);
				actor->updateStates(actor_state_mutex);
			}

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
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if(ImGui::GetIO().WantCaptureKeyboard)
		return;

	getCurrentTheatre()->delegateKeyInput(window, key, scancode, action, mods);

	if(key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		shader_debug_value = 1;
		PRINTNOTE("Shader Debug Focus Lighting Component: Diffuse")
	}

	if(key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		shader_debug_value = 2;
		PRINTNOTE("Shader Debug Focus Lighting Component: Specular")
	}
	if(key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		shader_debug_value = 3;
		PRINTNOTE("Shader Debug Focus Lighting Component: Ambient")
	}

	if(key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		shader_debug_value = 4;
		PRINTNOTE("Shader Debug Focus Lighting Component: All (Diffuse + Specular + Ambient)")
	}

	if(key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		shader_debug_value = 5;
		PRINTNOTE("Shader Debug Focus Lighting Component: Normals")
	}

	if(key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_PRESS)
	{
		if(shader_index >= (shaders.size() - 1))
			shader_index = 0;
		else
			shader_index++;

		switch(shader_index)
		{
		case SHADER_BLINN_PHONG:
			PRINTNOTE("Using Shader: Blinn-Phong")
			break;
		case SHADER_PHONG:
			PRINTNOTE("Using Shader: Phong")
			break;
		}
	}

	if(key == GLFW_KEY_LEFT_BRACKET && action == GLFW_PRESS)
	{
		if(shader_index <= 0)
			shader_index = shaders.size() - 1;
		else
			shader_index--;

		switch(shader_index)
		{
		case SHADER_BLINN_PHONG:
			PRINTNOTE("Using Shader: Blinn-Phong")
			break;
		case SHADER_PHONG:
			PRINTNOTE("Using Shader: Phong")
			break;
		}
	}

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
			PRINTNOTE("Ambient Lighting Disabled")
		else
			PRINTNOTE("Ambient Lighting Enabled")
	}

	if(key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		PRINTNOTE("Resetting PhysicsActors to initial transformation!")
		for(Actor *actor : getCurrentTheatre()->troupe)
			if(actor->isPhysicsActor())
				static_cast<PhysicsActor *>(actor)->reset_to_initial_orientation_for_testing();
	}

	if(key == GLFW_KEY_TAB && action == GLFW_PRESS && (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED))
	{
		toggleCursor(window, true);
	}

	if(key == GLFW_KEY_6 && action == GLFW_PRESS)
	{
		is_wireframe = !is_wireframe;
		if(is_wireframe)
		{
			PRINTNOTE("Polygon Mode: Wireframe")
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			return;
		}

		PRINTNOTE("Polygon Mode: Normal")
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}

	if(key == GLFW_KEY_J && action == GLFW_PRESS)
	{
		do_jolt_assert = !do_jolt_assert;
		if(do_jolt_assert)
			PRINTDEBUG("Jolt assert printouts enabled")
		else
			PRINTDEBUG("Jolt assert printouts disabled")
	}
}

void toggleCursor(GLFWwindow *window, bool show_cursor)
{
	if(!show_cursor)
	{
		PRINTDEBUG("Cursor Mode: Disabled (hidden + locked at center)")
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetCursorPos(window, cursor_last_x, cursor_last_y);
		return;
	}

	PRINTDEBUG("Cursor Mode: Normal (cursor visible & camera ignoring movement)")
	glfwGetCursorPos(window, &cursor_last_x, &cursor_last_y);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in)
{
	if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL || ImGui::GetIO().WantCaptureMouse)
		return;

	if(loading_new_main_theatre)
		return;

	getCurrentTheatre()->delegateMouseInput(window, x_position_in, y_position_in);
}