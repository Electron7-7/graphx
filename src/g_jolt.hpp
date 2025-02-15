#ifndef GRAPHX_JOLT
#define GRAPHX_JOLT
#include "graphx_namespace.hpp"
#include "r_common.hpp"
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

typedef std::tuple<glm::vec3, float, float> jolt_shape_args;

struct Collider : public Device
{
	glm::vec3			position = glm::vec3(0.0f);
	glm::vec3			scale = glm::vec3(1.0f);
	glm::quat			quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	JPH::EMotionType	motion_type = JPH::EMotionType::Dynamic;
	JPH::ObjectLayer	object_layer = Layers::MOVING;
	JPH::EActivation	activation = JPH::EActivation::Activate;
	float				friction = 1.0f;

	int					shape = ColliderShapes::BOX;
	jolt_shape_args		shape_arguments;

	Collider();

	void createBody();
	const JPH::BodyID &getBodyID();
	JPH::BodyCreationSettings *getBodySettings();

	void loadSettings(graphx::gSettings new_settings = {{"FUCKYOU", {}}}) override;
	void prepForDestruction() override;

protected:
	JPH::BodyID body_id;
	JPH::BodyCreationSettings body_settings;
};

extern JPH::PhysicsSystem jolt_physics_system;

void J_RemoveAndDestroyBody(JPH::BodyID body_id);
const JPH::Shape *createAShape(int shape, jolt_shape_args shape_args);
#endif