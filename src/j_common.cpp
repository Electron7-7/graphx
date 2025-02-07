#include "sanity.hpp"
#include "g_math.hpp"
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

const JPH::Shape *createAShape(int shape, jolt_shape_args shape_args)
{
	switch(shape)
	{
	case ColliderShapes::BOX:
		return new JPH::BoxShape(convertMath<JPH::Vec3>(std::get<0>(shape_args)));
	case ColliderShapes::SPHERE:
		return new JPH::SphereShape(std::get<1>(shape_args));
	case ColliderShapes::CAPSULE:
		return new JPH::CapsuleShape(std::get<2>(shape_args), std::get<1>(shape_args));
	case ColliderShapes::CYLINDER:
		return new JPH::CylinderShape(std::get<2>(shape_args), std::get<1>(shape_args));
	};

	return new JPH::BoxShape;
};

PhysicsSystem jolt_physics_system;

void J_RemoveAndDestroyBody(BodyID body_id)
{
	jolt_physics_system.GetBodyInterface().RemoveBody(body_id);
	jolt_physics_system.GetBodyInterface().DestroyBody(body_id);
}

//
// Collider
//
Collider::Collider()
{
	device_type = DEVICE_COLLIDER;
}

JPH::BodyCreationSettings *Collider::getBodySettings()
{
	return &body_settings;
}

JPH::BodyID *Collider::getBodyID()
{
	return &body_id;
}

void Collider::createBody()
{
	JPH::RVec3 body_position = convertMath<JPH::Vec3>(position);
	JPH::Quat body_quaternion = convertMath<JPH::Quat>(quaternion);

	body_settings = JPH::BodyCreationSettings(createAShape(shape, shape_arguments), body_position, body_quaternion, motion_type, object_layer);
	body_id = jolt_physics_system.GetBodyInterface().CreateAndAddBody(body_settings, activation);
}