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

PhysicsSystem jolt_physics_system;

void J_RemoveAndDestroyBody(const BodyID& body_id)
{
	jolt_physics_system.GetBodyInterface().RemoveBody(body_id);
	jolt_physics_system.GetBodyInterface().DestroyBody(body_id);
}