#ifndef GRAPHX_CLASSES_NAMESPACE
#define GRAPHX_CLASSES_NAMESPACE
// Not the same as `graphx_namespace_classes.cpp`
// This is where all new Actor/Device derivations get their "type" defined
#include "graphx_namespace.hpp"
#include "g_actors.hpp"
#include "r_common.hpp"
/**
 * Yeah, it's a bit finnicky to put a whole bunch of static constexpr global indentifier variables in a header file, BUT
 * I have two (relatively) good reasons for doing this:
 * 
 * 1. I wanted the process of adding a new Actor/Device to the engine to be as easy and simple as possible, so I made sure
 * that any code you'd have to write would be isolated to one file.
 * 
 * 2. I wanted to keep my header files relatively bloat-free, and since `createNewActor` & `createNewDevice` needed to be in
 * `g_actors.hpp` & `r_common.hpp` instead of the forward-declaration headers, I opted to move all this code into it's own
 * separate header file (I don't give a fuck about the source files, those bastards can be as big and round as they please).
*/
namespace graphx
{
	inline constexpr gClass gClass::INVALID_TYPE = gClass();  // The default constructor is INVALID_TYPE

	namespace classes
	{
		// Feel free to expand these limits if needed
		#define ACTOR_LIMIT  100
		#define DEVICE_LIMIT 200

		extern const std::array<const gClass*, (ACTOR_LIMIT + DEVICE_LIMIT)> valid_classes;

		extern const gClass ACTOR;
		extern const gClass PHYSICSACTOR;
		extern const gClass STATICBODYACTOR;
		extern const gClass RIGIDBODYACTOR;
		extern const gClass CAMERA;
		extern const gClass GRAPHXPLAYER;
		extern const gClass RAMIEL;
		extern const gClass LABEL;
		extern const gClass LIGHT;
		extern const gClass LIGHTDIRECTIONAL;
		extern const gClass LIGHTSPOT;
		extern const gClass LIGHTFLASHLIGHT;
		extern const gClass LIGHTTESTERMOVER;
		extern const gClass DEVICE;
		extern const gClass ENVIRONMENT;
		extern const gClass MATERIAL;
		extern const gClass MESH;
		extern const gClass SPRITE;
		extern const gClass COLLIDER;
		extern const gClass TEXTURE;

		const gClass& getClassType(const gClass&) noexcept;
		const gClass& getBaseType(const gClass&) noexcept;
		const bool isValid(const gClass&) noexcept;
		const bool isLight(const gClass&) noexcept;
	}
}
#endif