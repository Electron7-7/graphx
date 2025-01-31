#include "sanity.hpp"
#include "g_jolt.hpp"
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

using namespace JPH;
using namespace JPH::literals;

PhysicsSystem jolt_physics_system;

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