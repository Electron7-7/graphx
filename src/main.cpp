// Hello, production branch!
// :3

#define STB_IMAGE_IMPLEMENTATION
#include "sanity.hpp"
#include "r_common.hpp"
#include "g_actors.hpp"
#include "g_common.hpp"
#include "g_jolt.hpp"
#include "t_common.hpp"
#include "theatres.hpp"
#include <Jolt/Jolt.h>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>
#include <iostream>
#include <cstdarg>
#include <thread>
#include <mutex>

// Theatre *current_theatre_deprecated;
// Environment default_environment(true);

std::mutex actor_state_mutex;

glm::vec2 main_window_size(1280, 720);
glm::vec2 mouse_last(main_window_size / 2.0f);

static int TICKRATE = 120;

int current_tick_since_second = 0;
long current_tick_since_start = 0;
double last_tick_timestamp = 0;
bool test_flashlight_bool = false;
bool red_flashlight_color_bool = false;

void GLAPIENTRY _debug_callback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, GLchar const* message, void const* user_param);
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
	// glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetInputMode(main_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetCursorPosCallback(main_window, mouseCallback);
	glfwSetKeyCallback(main_window, keyCallback);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_DEBUG_OUTPUT);
	glDebugMessageCallback(_debug_callback, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, nullptr, GL_FALSE); // Disable notifications
	// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // Wireframe mode
	
	glGenVertexArrays(VAOS_AMOUNT, &VAOs[0]);

	GLShader phong_shader(phong_vertex_glsl, phong_fragment_glsl);
	shaders.insert(shaders.end(), {&phong_shader});

	std::thread game_logic_main_thread(testGameTick, main_window);

	while(!glfwWindowShouldClose(main_window))
	{
		W_SwapAndClear(main_window, getCurrentEnvironment()->getAmbientLight());
		glfwPollEvents();

		if(time_to_store_buffers)
			R_StoreBuffers();

		if(time_to_render)
		{
			// De-jank all of this shit below
			glm::mat4 projection_matrix = glm::perspective(glm::radians(45.0f), (float)main_window_size[0] / (float)main_window_size[1], 0.1f, 100.0f);
			float interpolation_time = ((glfwGetTime() - last_tick_timestamp) / TICKLENGTH);
			R_Render(actor_state_mutex, interpolation_time, projection_matrix);
		}
	}

	game_logic_main_thread.join();
	glfwTerminate();
	return 0;
}

//-------------------------------------------------------
// JOLT PHYSICS ENGINE BOILERPLATE
// (totally just copy-pasting the HelloWorld.cpp example)
//-------------------------------------------------------
JPH_SUPPRESS_WARNINGS

static void GraphXJoltTrace(const char *inFMT, ...)
{
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	std::cout << buffer << std::endl;
}

#ifdef JPH_ENABLE_ASSERTS
static bool GraphXJoltAssertFailed(const char *inExpression, const char *inMessage, const char *inFile, JPH::uint inLine)
{
	std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage !=nullptr? inMessage: "") << std::endl;
	return true;
}
#endif

class GraphXObjectLayerPairFilter : public JPH::ObjectLayerPairFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inObject1, JPH::ObjectLayer inObject2) const override
	{
		switch(inObject1)
		{
			case Layers::NON_MOVING:
				return inObject2 == Layers::MOVING;
			case Layers::MOVING:
				return true;
			default:
				JPH_ASSERT(false);
				return false;
		}
	}
};

class GraphXBroadPhaseLayerInterface final : public JPH::BroadPhaseLayerInterface
{
public:
	GraphXBroadPhaseLayerInterface()
	{
		mObjectToBroadPhase[Layers::NON_MOVING] = BroadPhaseLayers::NON_MOVING;
		mObjectToBroadPhase[Layers::MOVING] = BroadPhaseLayers::MOVING;
	}

	virtual JPH::uint GetNumBroadPhaseLayers() const override
	{
		return BroadPhaseLayers::NUM_LAYERS;
	}

	virtual JPH::BroadPhaseLayer GetBroadPhaseLayer(JPH::ObjectLayer inLayer) const override
	{
		JPH_ASSERT(inLayer < Layers::NUM_LAYERS);
		return mObjectToBroadPhase[inLayer];
	}
#if defined(JPH_EXTERNAL_PROFILE) || defined(JPH_PROFILE_ENABLED)
	virtual const char *GetBroadPhaseLayerName(JPH::BroadPhaseLayer inLayer) const override
	{
		switch ((JPH::BroadPhaseLayer::Type)inLayer)
		{
			case(JPH::BroadPhaseLayer::Type)BroadPhaseLayers::NON_MOVING:
				return "NON_MOVING";
			case(JPH::BroadPhaseLayer::Type)BroadPhaseLayers::MOVING:
				return "MOVING";
			default:
				JPH_ASSERT(false);
				return "INVALID";
		}
	}
#endif

private:
	JPH::BroadPhaseLayer mObjectToBroadPhase[Layers::NUM_LAYERS];
};

class GraphXObjectVsBroadPhaseLayerFilter : public JPH::ObjectVsBroadPhaseLayerFilter
{
public:
	virtual bool ShouldCollide(JPH::ObjectLayer inLayer1, JPH::BroadPhaseLayer inLayer2) const override
	{
		switch(inLayer1)
		{
			case Layers::NON_MOVING:
				return inLayer2 == BroadPhaseLayers::MOVING;
			case Layers::MOVING:
				return true;
			default:
				JPH_ASSERT(false);
				return false;
		}
	}
};

class GraphXContactListener : public JPH::ContactListener
{
	virtual JPH::ValidateResult OnContactValidate(const JPH::Body &inBody1, const JPH::Body &inBody2, JPH::RVec3Arg inBaseOffset, const JPH::CollideShapeResult &inCollisionResult) override
	{
		// std::cout << "Contact validate callback" << std::endl;
		return JPH::ValidateResult::AcceptAllContactsForThisBodyPair;
	}

	virtual void OnContactAdded(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
	{
		// std::cout << "A contact was added" << std::endl;
	}

	virtual void OnContactPersisted(const JPH::Body &inBody1, const JPH::Body &inBody2, const JPH::ContactManifold &inManifold, JPH::ContactSettings &ioSettings) override
	{
		// std::cout << "A contact was persisted" << std::endl;
	}

	virtual void OnContactRemoved(const JPH::SubShapeIDPair &inSubShapePair) override
	{
		// std::cout << "A contact was removed" << std::endl;
	}
};

class GraphXBodyActivationListener : public JPH::BodyActivationListener
{
public:
	virtual void OnBodyActivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override
	{
		// std::cout << "A body got activated" << std::endl;
	}

	virtual void OnBodyDeactivated(const JPH::BodyID &inBodyID, JPH::uint64 inBodyUserData) override
	{
		// std::cout << "A body went to sleep" << std::endl;
	}
};
//--------------------------------
// END OF JOLT PHYSICS BOILERPLATE
//--------------------------------

// #include "theatres/deprecated_header_format/collision_testing.oldtheatretemplate"

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

	// This will change to include loading external Theatres and to not just only load the first Theatre lmfao
	loadTheatre(embedded_theatres[0]); // So, when I add UI, this will go behind a "start game"/"load level"/etc
	getCurrentTheatre()->startPreshow();

	// current_theatre_deprecated = &collision_testing_theatre;
	// current_theatre_deprecated->actorEnter(&player);
	// current_theatre_deprecated->startPreshow();
	time_to_store_buffers = true;

	double last_time = glfwGetTime();
	double current_tick_length = 0;
	double now_time = 0;

	jolt_physics_system.OptimizeBroadPhase(); // Call this *after* adding bodies before calling Update for first time (e.g: loading a new/the first Theatre)

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

			for(Actor *actor : getCurrentTheatre()->troupe)
			{
				actor->tick(current_tick_since_start);
				actor->updateStates(actor_state_mutex);
			}

			// player_flashlight.setLight(test_flashlight_bool);
			
			// if(red_flashlight_color_bool)
				// player_flashlight.light_color = glm::vec3(1.0f, 0.0f, 0.0f);
			// else
				// player_flashlight.light_color = glm::vec3(1.0f);
			
			jolt_physics_system.Update(TICKLENGTH, 1, &jolt_temp_allocator, &jolt_job_system);

			last_tick_timestamp = glfwGetTime();
			current_tick_length--;
		}

		if(current_tick_since_second >= TICKRATE)
			current_tick_since_second = 0;
	}
	
	time_to_render = false; // Because game logic can (and usually does) exit before the main loop

	// current_theatre_deprecated->dropCurtains();
	getCurrentTheatre()->dropCurtains();


	JPH::UnregisterTypes();

	delete JPH::Factory::sInstance;
	JPH::Factory::sInstance = NULL;
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if(key == GLFW_KEY_G && action == GLFW_PRESS)
	{
		getCurrentEnvironment()->ambient_lighting_enabled = !getCurrentEnvironment()->ambient_lighting_enabled;
		if(!getCurrentEnvironment()->ambient_lighting_enabled)
			PRINTLN("Ambient Lighting Disabled")
		else
			PRINTLN("Ambient Lighting Enabled")
	}

	if(key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		test_flashlight_bool = !test_flashlight_bool;
		if(test_flashlight_bool)
			PRINTLN("Flashlight Off")
		else
			PRINTLN("Flashlight On")
	}

	if(key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		red_flashlight_color_bool = !red_flashlight_color_bool;
		if(red_flashlight_color_bool)
			PRINTLN("Flashlight Red")
		else
			PRINTLN("Flashlight Not Red")
	}

	if(key == GLFW_KEY_R && action == GLFW_PRESS)
	{
		PRINTLN("Resetting PhysicsActors to initial transformation!")
		for(Actor *actor : 	getCurrentTheatre()->troupe)
			if(actor->actor_type == ACTOR_PHYSICS)
				static_cast<PhysicsActor *>(actor)->reset_to_initial_orientation_for_testing();
	}

	if(key == GLFW_KEY_TAB && action == GLFW_PRESS)
	{
		if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
		{
			PRINTLN("Cursor Mode: Disabled (hidden + locked at center)")
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			return;
		}

		PRINTLN("Cursor Mode: Normal (cursor visible & camera ignoring movement)")
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}

// This will be put in Actor once I abstract "glfwGetKey" and related functions
void processInput(GLFWwindow *window)
{
	int input_vector[2] =
	{
		glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S),
		glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A)
	};

	current_player->doMovement(input_vector);
}

void mouseCallback(GLFWwindow *window, double x_position_in, double y_position_in)
{
	glm::vec2 mouse_position(static_cast<float>(x_position_in), static_cast<float>(y_position_in));
	glm::vec2 mouse_offset = mouse_position - mouse_last;
	mouse_last = mouse_position;
	
	if(glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL)
		return;
	
	current_player->doMouseMovement(mouse_offset);
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