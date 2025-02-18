#include "g_jolt.hpp"
#include "g_math.hpp"
#include "t_common.hpp"
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

void Collider::loadSettings(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;

	Device::loadSettings(new_settings);

	setVariable(motion_type, new_settings["MotionType"]);
	setVariable(object_layer, new_settings["ObjectLayer"]);
	setVariable(activation, new_settings["Activation"]);
	setVariable(shape, new_settings["Shape"]);
	setRawData(forever_alone, new_settings["ForeverAlone"]);
	setRawData(position, new_settings["Position"]);
	setRawData(euler_angles, new_settings["Rotation"]);
	setRawData(local_position, new_settings["LocalPosition"]);
	setRawData(local_euler_angles, new_settings["LocalRotation"]);
	setRawData(scale, new_settings["Scale"]);
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
	JPH::RVec3 body_position = convertMath<JPH::Vec3>(position) + convertMath<JPH::Vec3>(local_position);
	glm::vec3 euler_radians = glm::radians(euler_angles);
	glm::vec3 local_euler_radians = glm::radians(local_euler_angles);
	JPH::Quat body_quaternion = JPH::Quat::sEulerAngles(convertMath<JPH::Vec3>(euler_radians)) * JPH::Quat::sEulerAngles(convertMath<JPH::Vec3>(local_euler_radians));

	body_settings = JPH::BodyCreationSettings(createAShape(shape, shape_arguments), body_position, body_quaternion, motion_type, object_layer);
	body_id = jolt_physics_system.GetBodyInterface().CreateAndAddBody(body_settings, activation);
	jolt_physics_system.GetBodyInterface().SetFriction(body_id, friction);
}

void Collider::initialize(Theatre *parent_theare)
{
	if(forever_alone)
		createBody();
}

void Collider::prepForDestruction()
{
	Device::prepForDestruction();

	if(!body_id.IsInvalid())
		J_RemoveAndDestroyBody(body_id);
}