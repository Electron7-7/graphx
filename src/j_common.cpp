#include "sanity.hpp"
#include "g_jolt.hpp"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

using namespace JPH;
using namespace JPH::literals;
using namespace std;

PhysicsSystem jolt_physics_system;

void GraphXJoltTrace(const char *inFMT, ...)
{
	va_list list;
	va_start(list, inFMT);
	char buffer[1024];
	vsnprintf(buffer, sizeof(buffer), inFMT, list);
	va_end(list);

	std::cout << buffer << std::endl;
}

bool GraphXJoltAssertFailed(const char *inExpression, const char *inMessage, const char *inFile, JPH::uint inLine)
{
	std::cout << inFile << ":" << inLine << ": (" << inExpression << ") " << (inMessage !=nullptr? inMessage: "") << std::endl;
	return true;
}

void J_InitJolt()
{
	RegisterDefaultAllocator();
	Factory::sInstance = new Factory();
	RegisterTypes();

	Trace = GraphXJoltTrace;
	JPH_IF_ENABLE_ASSERTS(AssertFailed = GraphXJoltAssertFailed;)

	const uint cMaxBodies = 2048;
	const uint cNumBodyMutexes = 0;
	const uint cMaxBodyPairs = 2048;
	const uint cMaxContactConstraints = 2048;

	GraphXBroadPhaseLayerInterface broad_phase_layer_interface;
	GraphXObjectVsBroadPhaseLayerFilter object_vs_broadphase_layer_filter;
	GraphXObjectLayerPairFilter object_vs_object_layer_filter;
	jolt_physics_system.Init(cMaxBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, broad_phase_layer_interface, object_vs_broadphase_layer_filter, object_vs_object_layer_filter);

#ifdef GRAPHX_DEBUG
	GraphXBodyActivationListener body_activation_listener;
	jolt_physics_system.SetBodyActivationListener(&body_activation_listener);

	GraphXContactListener contact_listener;
	jolt_physics_system.SetContactListener(&contact_listener);
#endif
}

void J_StopJolt()
{
	UnregisterTypes();

	delete Factory::sInstance;
	Factory::sInstance = NULL;
}

void J_RemoveAndDestroyBody(BodyID body_id)
{
	jolt_physics_system.GetBodyInterface().RemoveBody(body_id);
	jolt_physics_system.GetBodyInterface().DestroyBody(body_id);
}

//
// Code for making the floor and wall (toss if not needed)
//
/*	BodyInterface &body_interface = physics_system.GetBodyInterface();

	BoxShapeSettings floor_shape_settings(Vec3(20.0f, 1.0f, 20.0f));
	floor_shape_settings.SetEmbedded();
	ShapeSettings::ShapeResult floor_shape_result = floor_shape_settings.Create();
	ShapeRefC floor_shape = floor_shape_result.Get();
	BodyCreationSettings floor_settings(floor_shape, RVec3(Real3(0.0f, 0.0f, 0.0f)), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	Body *floor = body_interface.CreateBody(floor_settings);
	body_interface.AddBody(floor->GetID(), EActivation::DontActivate);

	BoxShapeSettings wall_shape_settings(Vec3(20.0f, 20.0f, 1.0f));
	wall_shape_settings.SetEmbedded();
	ShapeSettings::ShapeResult wall_shape_result = wall_shape_settings.Create();
	ShapeRefC wall_shape = wall_shape_result.Get();
	BodyCreationSettings wall_settings(wall_shape, RVec3(Real3(0.0f, 0.0f, -21.5f)), Quat::sIdentity(), EMotionType::Static, Layers::NON_MOVING);
	Body *wall = body_interface.CreateBody(wall_settings);
	body_interface.AddBody(wall->GetID(), EActivation::DontActivate);*/