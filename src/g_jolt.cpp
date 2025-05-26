#include "g_jolt.hpp"
#include <gmath.hpp>
#include <glm/glm.hpp>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

using namespace JPH;
using namespace JPH::literals;

const JPH::Shape* J_CreateAShape(int shape, std::tuple<glm::vec3, float, float> shape_args)
{
	switch(shape)
	{
	case graphx::jolt::shapes::BOX:
		return new JPH::BoxShape(gmath::convertMath<JPH::Vec3>(std::get<0>(shape_args)));
	case graphx::jolt::shapes::SPHERE:
		return new JPH::SphereShape(std::get<1>(shape_args));
	case graphx::jolt::shapes::CAPSULE:
		return new JPH::CapsuleShape(std::get<2>(shape_args), std::get<1>(shape_args));
	case graphx::jolt::shapes::CYLINDER:
		return new JPH::CylinderShape(std::get<2>(shape_args), std::get<1>(shape_args));
	};

	return new JPH::BoxShape;
};

PhysicsSystem jolt_physics_system;

void J_RemoveAndDestroyBody(BodyID body_id)
{
	if(body_id.IsInvalid() || !jolt_physics_system.GetBodyInterface().IsActive(body_id) || !jolt_physics_system.GetBodyInterface().IsAdded(body_id))
		return;
	jolt_physics_system.GetBodyInterface().RemoveBody(body_id);
	jolt_physics_system.GetBodyInterface().DestroyBody(body_id);
}