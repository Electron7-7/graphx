// Hello, production branch!
// :3

#include "sanity.hpp"
#include "graphx_namespace.hpp"
#include "g_actors.hpp"
#include "g_theatre.hpp"
#include "g_jolt.hpp"
#include "g_imgui.hpp"
#include "sanity_printouts.hpp"
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
#include <thread>
#include <mutex>

std::mutex actor_state_mutex;

static int TICKRATE = 120;

// Todo: put this in `graphx_namespace.hpp`
int current_tick_since_second = 0;
long current_tick_since_start = 0;
double last_tick_timestamp = 0;
bool do_jolt_assert = false;
bool is_wireframe = false;

double cursor_last_x = 0.0;
double cursor_last_y = 0.0;

void APIENTRY openGLDebugMessageCallback(GLenum, GLenum, GLuint, GLenum, GLsizei, const GLchar*, const void*);
void frameBufferSizeCallback(GLFWwindow* window, int width, int height);
void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void gameTick(GLFWwindow *window);

// Quick helper macro for getting the length of one game tick
#define TICKLENGTH (1.0f / TICKRATE)

// The Jolt Physics boilerplate code was really annoying to scroll through, so I isolated it
#include "jolt_boilerplate.hpp"

int main()
{
	graphx::rendering::graphx_api = graphx::rendering::GRAPHX_OPENGL;

	//------------------
	// OpenGL/GLFW Setup | Todo: MOVE ALL OF THIS INTO R_GL_Initialize AT SOME POINT
	//------------------
	glfwInit();
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
	GLFWwindow *main_window = W_CreateWindow(graphx::rendering::main_window_width, graphx::rendering::main_window_height);
	const GLFWvidmode *primary_monitor_video_mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	int primary_monitor_xposition = 0;
	int primary_monitor_yposition = 0;
	glfwGetMonitorPos(glfwGetPrimaryMonitor(), &primary_monitor_xposition, &primary_monitor_yposition);
	glfwSetWindowPos(main_window, static_cast<int>(((primary_monitor_video_mode->width - graphx::rendering::main_window_width) / 2) + primary_monitor_xposition), static_cast<int>(((primary_monitor_video_mode->height - graphx::rendering::main_window_height) / 2) + primary_monitor_yposition));
#ifdef GRAPHX_DEBUG
	glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL); // When using lldb, I enable this line to keep the mouse cursor from getting stuck disabled
#else
	glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
#endif
	glfwSetCursorPosCallback(main_window, mouseCallback);
	glfwSetFramebufferSizeCallback(main_window, frameBufferSizeCallback);
	glfwSetKeyCallback(main_window, keyCallback);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	glDebugMessageCallback(openGLDebugMessageCallback, nullptr);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_FRAMEBUFFER_SRGB);

	PRINTDEBUG("GL_VERSION: " << glGetString(GL_VERSION))
	PRINTDEBUG("GL_SHADING_LANGUAGE_VERSION: " << glGetString(GL_SHADING_LANGUAGE_VERSION))

	R_InitializeRenderingAPI();

	//----------------
	// FreeType2 Setup
	//----------------
	F_InitializeFreeType();
	F_LoadFont("src/fonts/Arial.ttf", "Arial");
	F_LoadFont("src/fonts/Verdana.ttf", "Verdana");
	F_LoadFont("src/fonts/Tr2n.ttf", "Tr2n");
	F_LoadFont("src/fonts/Audiowide-Regular.ttf", "Audiowide");

	std::thread game_logic_main_thread(gameTick, main_window);

	//------------
	// ImGui Setup
	//------------
	GraphXConsole graphx_debug_console;

	graphx_debug_console.active = (glfwGetInputMode(main_window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

#ifndef GRAPHX_DEBUG
	ImGui::GetIO().IniFilename = nullptr;
#endif
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

	ImGui_ImplGlfw_InitForOpenGL(main_window, true);
	ImGui_ImplOpenGL3_Init();

	//----------
	// Main Loop
	//----------
	while(!glfwWindowShouldClose(main_window))
	{
		W_SwapAndClear(main_window, glm::vec4(0.7f, 0.5f, 0.5f, 1.0f));
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		graphx_debug_console.updateFrame(main_window);

		if(graphx_debug_console.justClosed())
			toggleCursor(main_window, false);

		if(time_to_store_buffers)
			R_BufferMeshesAndTextures();

		if(time_to_render)
		{
			float interpolation_time = ((glfwGetTime() - last_tick_timestamp) / TICKLENGTH);
			R_Render(actor_state_mutex, interpolation_time);
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	//------------------------------------------------------------------
	// Sparse and malnourished cleanup code (it barely does anything...)
	//------------------------------------------------------------------
	FT_Done_FreeType(freetype); // Todo: make a release function for FreeType
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	game_logic_main_thread.join();
	glfwTerminate();
	return 0;
}

//------------------
// GameTick Function
//------------------
void gameTick(GLFWwindow *main_window)
{
	//-----------
	// Jolt Setup
	//-----------
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

	//--------------------
	// GraphXTheatre Setup
	//--------------------
	checkForAndLoadExternalTheatres();

	// Hard-coded first Theatre loading, for now. Eventually, Theatre loading won't rely on this function, or the arrow key callbacks
	loadMainTheatre(0);

	//---------------
	// Tickrate Setup
	//---------------
	double last_time = glfwGetTime();
	double current_tick_length = 0;
	double now_time = 0;

	//---------------
	// Game Tick Loop
	//---------------
	while(!glfwWindowShouldClose(main_window))
	{
		now_time = glfwGetTime();
		current_tick_length += (now_time - last_time) / TICKLENGTH;
		last_time = now_time;

		while(current_tick_length >= 1.0f && !loading_new_main_theatre)
		{
			current_tick_since_second++;
			current_tick_since_start++;

			std::vector<Actor*> troupe = getCurrentTheatre()->getTroupe(); // Todo: change the troupe to be a pointer/reference to the objects map?
			for(Actor *actor : troupe)
			{
				if(!ImGui::GetIO().WantCaptureKeyboard)
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

//--------------------------------
// Main GLFW Key Callback Function
//--------------------------------
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if(ImGui::GetIO().WantCaptureKeyboard)
		return;

	getCurrentTheatre()->delegateKeyInput(window, key, scancode, action, mods);

	if(key == GLFW_KEY_1 && action == GLFW_PRESS)
	{
		graphx::rendering::lighting_switch_diffuse = !graphx::rendering::lighting_switch_diffuse;
		if(graphx::rendering::lighting_switch_diffuse)
			PRINTNOTE("Diffuse Lighting Component: Enabled")
		else
			PRINTNOTE("Diffuse Lighting Component: Disabled")
	}

	if(key == GLFW_KEY_2 && action == GLFW_PRESS)
	{
		graphx::rendering::lighting_switch_specular = !graphx::rendering::lighting_switch_specular;
		if(graphx::rendering::lighting_switch_specular)
			PRINTNOTE("Specular Lighting Component: Enabled")
		else
			PRINTNOTE("Specular Lighting Component: Disabled")
	}

	if(key == GLFW_KEY_3 && action == GLFW_PRESS)
	{
		graphx::rendering::lighting_switch_ambient = !graphx::rendering::lighting_switch_ambient;
		if(graphx::rendering::lighting_switch_ambient)
			PRINTNOTE("Ambient Lighting Component: Enabled")
		else
			PRINTNOTE("Ambient Lighting Component: Disabled")
	}

	if(key == GLFW_KEY_4 && action == GLFW_PRESS)
	{
		switch(debug_render_switches - 1)
		{
		case USE_FULLBRIGHT:
			debug_render_switches = USE_FULLBRIGHT;
			PRINTNOTE("Shader Debug Focus: mat_fullbright")
			break;
		case USE_NORMALS:
			debug_render_switches = USE_NORMALS;
			PRINTNOTE("Shader Debug Focus: Normals")
			break;
		case USE_DEFAULT:
			debug_render_switches = USE_DEFAULT;
			PRINTNOTE("Shader Debug Focus: Regular Shader Output")
			break;
		default:
			debug_render_switches = USE_VERTEX_COLORS;
			PRINTNOTE("Shader Debug Focus: Vertex Colors")
			break;
		}
	}

	if(key == GLFW_KEY_5 && action == GLFW_PRESS)
	{
		switch(debug_render_switches + 1)
		{
		case USE_FULLBRIGHT:
			debug_render_switches = USE_FULLBRIGHT;
			PRINTNOTE("Shader Debug Focus: mat_fullbright ;3")
			break;
		case USE_NORMALS:
			debug_render_switches = USE_NORMALS;
			PRINTNOTE("Shader Debug Focus: Normals")
			break;
		case USE_VERTEX_COLORS:
			debug_render_switches = USE_VERTEX_COLORS;
			PRINTNOTE("Shader Debug Focus: Vertex Colors")
			break;
		default:
			debug_render_switches = USE_DEFAULT;
			PRINTNOTE("Shader Debug Focus: Regular Shader Output")
			break;
		}
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

	if(key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		enable_default_shader = !enable_default_shader;
	}

	if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS)
	{
		if(loading_new_main_theatre)
			return;
		for(auto it = embedded_theatres.begin() ; it != embedded_theatres.end() ; it++)
		{
			if(it->first == graphx::current::theatre.getUID())
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
		for(auto it = embedded_theatres.begin() ; it != embedded_theatres.end() ; it++)
		{
			if(it->first == graphx::current::theatre.getUID())
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

	if(key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		PRINTNOTE("Resetting PhysicsActors to initial transformation!")
		std::vector<Actor*> troupe = getCurrentTheatre()->getTroupe();
		for(Actor* actor : troupe)
			if(actor->isPhysicsActor())
				static_cast<PhysicsActor *>(actor)->reset_to_initial_orientation_for_testing();
	}

	if(key == GLFW_KEY_TAB && action == GLFW_PRESS && (glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED))
	{
		toggleCursor(window, true);
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

//----------------------------
// Cursor/ImGui Focus Toggling
//----------------------------
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

//----------------------------------
// Main GLFW Mouse Callback Function
//----------------------------------
void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in)
{
	if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL || ImGui::GetIO().WantCaptureMouse)
		return;

	if(loading_new_main_theatre)
		return;

	getCurrentTheatre(false)->delegateMouseInput(window, x_position_in, y_position_in);
}

//----------------------------------------------
// Main GLFW Frame-Buffer Size Callback Function
//----------------------------------------------
void frameBufferSizeCallback(GLFWwindow *window, int width, int height)
{
	graphx::rendering::main_window_width = width;
	graphx::rendering::main_window_height = height;

	glViewport(0, 0, width, height);
}

//----------------------------------------------------------------------------------
// OpenGL Debug Message Callback Function
// https://gist.github.com/liam-middlebrook/c52b069e4be2d87a6d2f (with minor tweaks)
//----------------------------------------------------------------------------------
void APIENTRY openGLDebugMessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* data)
{
	std::string _source;
    std::string _type;
    std::string _severity;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
        _source = "API";
        break;

        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
        _source = "WINDOW SYSTEM";
        break;

        case GL_DEBUG_SOURCE_SHADER_COMPILER:
        _source = "SHADER COMPILER";
        break;

        case GL_DEBUG_SOURCE_THIRD_PARTY:
        _source = "THIRD PARTY";
        break;

        case GL_DEBUG_SOURCE_APPLICATION:
        _source = "APPLICATION";
        break;

        case GL_DEBUG_SOURCE_OTHER:
        _source = "UNKNOWN";
        break;

        default:
        _source = "UNKNOWN";
        break;
    }

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
        _type = "ERROR";
        break;

        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
        _type = "DEPRECATED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
        _type = "UDEFINED BEHAVIOR";
        break;

        case GL_DEBUG_TYPE_PORTABILITY:
        _type = "PORTABILITY";
        break;

        case GL_DEBUG_TYPE_PERFORMANCE:
        _type = "PERFORMANCE";
        break;

        case GL_DEBUG_TYPE_OTHER:
        _type = "OTHER";
        break;

        case GL_DEBUG_TYPE_MARKER:
        _type = "MARKER";
        break;

        default:
        _type = "UNKNOWN";
        break;
    }

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
        _severity = "HIGH";
        break;

        case GL_DEBUG_SEVERITY_MEDIUM:
        _severity = "MEDIUM";
        break;

        case GL_DEBUG_SEVERITY_LOW:
        _severity = "LOW";
        break;

        case GL_DEBUG_SEVERITY_NOTIFICATION:
        _severity = "NOTIFICATION";
        break;

        default:
        _severity = "UNKNOWN";
        break;
    }

    printf("%d: %s of %s severity, raised from %s: %s\n", id, _type.c_str(), _severity.c_str(), _source.c_str(), message);
}