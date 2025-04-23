#ifndef GRAPHX_JOLT
#define GRAPHX_JOLT
#include <glm/fwd.hpp>
#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>

namespace Layers
{
	static constexpr JPH::ObjectLayer NON_MOVING = 0;
	static constexpr JPH::ObjectLayer MOVING = 1;
	static constexpr JPH::ObjectLayer NUM_LAYERS = 2;
}

namespace BroadPhaseLayers
{
	static constexpr JPH::BroadPhaseLayer NON_MOVING(0);
	static constexpr JPH::BroadPhaseLayer MOVING(1);
	static constexpr JPH::uint NUM_LAYERS(2);
}

namespace graphx
{
	namespace jolt
	{
		typedef std::tuple<glm::vec3, float, float> shape_arguments;

		namespace shapes
		{
			constexpr int BOX = 0;
			constexpr int SPHERE = 1;
			constexpr int CAPSULE = 2;
			constexpr int CYLINDER = 3;
		}
	}
}

extern JPH::PhysicsSystem jolt_physics_system;

const JPH::Shape* J_CreateAShape(int, std::tuple<glm::vec3, float, float>);
void J_RemoveAndDestroyBody(JPH::BodyID);
#endif