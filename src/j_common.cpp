#include "sanity.hpp"
#include "g_jolt.hpp"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Core/JobSystemThreadPool.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>

using namespace JPH;
using namespace JPH::literals;

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

bool GraphXJoltAssertFailed(const char *inExpression, const char *inMessage, const char *inFile, uint inLine)
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

JPH::BodyID J_AddAndCreateBody(jolt_collider_options *collider_options)
{
	JPH::ShapeSettings::ShapeResult collider_shape_result = collider_options->shape_settings->Create();
	if(collider_shape_result.HasError())
		PRINT(collider_shape_result.GetError());
	JPH::ShapeRefC collider_shape = collider_shape_result.Get();
	JPH::BodyCreationSettings collider_settings(collider_shape, collider_options->position, collider_options->quaternion, collider_options->motion_type, collider_options->object_layer);
	JPH::BodyID collider_id = jolt_physics_system.GetBodyInterface().CreateAndAddBody(collider_settings, JPH::EActivation::Activate);
	return collider_id;
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