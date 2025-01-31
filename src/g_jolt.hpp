#ifndef GRAPHX_JOLT
#define GRAPHX_JOLT
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
};

namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr JPH::uint NUM_LAYERS(2);
};

namespace ColliderShapes
{
	static constexpr int BOX = 0;
	static constexpr int SPHERE = 1;
	static constexpr int CAPSULE = 2;
	static constexpr int CYLINDER = 3;
};

/*struct jolt_collider_recipe
{
public:
	bool				take_shape_from_actor = false;

	int					shape;
	JPH::Vec3			position = JPH::Vec3(0.0f, 0.0f, 0.0f);
	JPH::Quat			quaternion = JPH::Quat::sIdentity();
	JPH::EMotionType	motion_type;
	JPH::ObjectLayer	object_layer;
	JPH::EActivation	activation;

	JPH::BodyCreationSettings body_settings;

	jolt_collider_recipe(int body_shape, JPH::EMotionType body_motion_type, JPH::ObjectLayer body_object_layer, JPH::EActivation body_activation);
	jolt_collider_recipe(JPH::Vec3Arg inHalfExtent, JPH::EMotionType body_motion_type, JPH::ObjectLayer body_object_layer, JPH::EActivation body_activation);
	jolt_collider_recipe(float inRadius, JPH::EMotionType body_motion_type, JPH::ObjectLayer body_object_layer, JPH::EActivation body_activation);
	jolt_collider_recipe(float inHalfHeight, float inRadius, int body_shape, JPH::EMotionType body_motion_type, JPH::ObjectLayer body_object_layer, JPH::EActivation body_activation);

	void setBodySettings(JPH::Vec3Arg inHalfExtent);
	void setBodySettings(float inRadius);
	void setBodySettings(float inHalfHeight, float inRadius);
	void setTransform(JPH::Vec3 new_position, JPH::Quat new_quaternion);
	void offsetTransform(JPH::Vec3 added_position, JPH::Quat added_quaternion);
};*/

extern JPH::PhysicsSystem jolt_physics_system;

void J_RemoveAndDestroyBody(JPH::BodyID body_id);
#endif