// sanity.hpp - Various pre-processor includes for the sake of my sanity
/*
12/11/24 Note: This is probably what makes compiling take so fucking long, since every god damned header and source file includes this... oh wait...
no, I don't think that this should cause compiling to take longer since the #ifndef means that these headers only actually get copied by the preprocessor
once... I think, at least. This could just be a misnomer/not how that works(?) I need to make sure.
*/
#ifndef GRAPHX_SANITY
#define GRAPHX_SANITY

#define PRINT_MARKER (std::cout << std::endl << "[=======================================]" << std::endl)
#define PRINT(thing) (std::cout << std::endl << thing << std::endl << std::endl)

#include <iostream>

#include <images.h>
#include <shaders.hpp>

#include <stb_image.h>

#define GLM_ENABLE_EXPERIMENTAL
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/fwd.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/ext.hpp>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/PhysicsSystem.h>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyActivationListener.h>

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
#endif