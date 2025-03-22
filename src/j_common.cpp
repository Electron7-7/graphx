#include "g_jolt.hpp"
#include "t_settings.hpp"
#include <gmath.hpp>
#include <Jolt/RegisterTypes.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

using namespace JPH;
using namespace JPH::literals;
using namespace graphx;

const JPH::Shape *createAShape(int shape, jolt_shape_args shape_args)
{
	switch(shape)
	{
	case ColliderShapes::BOX:
		return new JPH::BoxShape(gmath::convertMath<JPH::Vec3>(std::get<0>(shape_args)));
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
	my_type = graphx::classes::COLLIDER;
	name = "Untitled Collider";
}

Collider::~Collider()
{
	J_RemoveAndDestroyBody(body_id);
}

void Collider::loadSettings(graphx::gSettings new_settings)
{
	Device::loadSettings(new_settings);

	getSetting(motion_type, settings["MotionType"]);
	getSetting(object_layer, settings["ObjectLayer"]);
	getSetting(activation, settings["Activation"]);
	getSetting(shape, settings["Shape"]);
	getSetting(forever_alone, settings["ForeverAlone"]);
	getSetting(position, settings["Position"]);
	getSetting(euler_angles, settings["Rotation"]);
	getSetting(local_position, settings["LocalPosition"]);
	getSetting(local_euler_angles, settings["LocalRotation"]);
	getSetting(scale, settings["Scale"]);
}

JPH::BodyCreationSettings *Collider::getBodySettings()
{
	return &body_settings;
}

const JPH::BodyID &Collider::getBodyID()
{
	return body_id;
}

void Collider::createBody()
{
	shape_arguments = std::make_tuple(scale, glm::max(glm::max(scale[0], scale[1]), scale[2]), scale[1]);
	JPH::RVec3 body_position = gmath::convertMath<JPH::Vec3>(position) + gmath::convertMath<JPH::Vec3>(local_position);
	JPH::Quat body_quaternion = JPH::Quat::sEulerAngles(gmath::convertMath<JPH::Vec3>(glm::radians(euler_angles))) * JPH::Quat::sEulerAngles(gmath::convertMath<JPH::Vec3>(glm::radians(local_euler_angles)));

	body_settings = JPH::BodyCreationSettings(createAShape(shape, shape_arguments), body_position, body_quaternion, motion_type, object_layer);
	body_id = jolt_physics_system.GetBodyInterface().CreateAndAddBody(body_settings, activation);
	jolt_physics_system.GetBodyInterface().SetFriction(body_id, friction);
}

void Collider::destroyBody()
{
	if(!body_id.IsInvalid())
		J_RemoveAndDestroyBody(body_id);
}

void Collider::initialize()
{
	if(forever_alone)
		createBody();
}

void Collider::prepForDestruction()
{
	Device::prepForDestruction();

	if(jolt_physics_system.GetBodyInterface().IsAdded(body_id))
		J_RemoveAndDestroyBody(body_id);
}