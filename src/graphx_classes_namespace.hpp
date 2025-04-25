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
	namespace classes
	{
		// Feel free to expand these limits if needed
		#define ACTOR_LIMIT  100
		#define DEVICE_LIMIT 200
		#define AL ACTOR_LIMIT // This helps make the list below look a lot nicer

		//                      variable         name                    ID    class constructor function         debugging color (optional)
		//--------------------------------------------------------------------------------------------------------------------------------------
		inline constexpr gClass ACTOR            ( "Actor",              1,    &createNewActor<Actor>,            glm::vec3(0.4f, 0.6f, 0.8f) );
		inline constexpr gClass PHYSICSACTOR     ( "PhysicsActor",       2,    &createNewActor<PhysicsActor>,     glm::vec3(1.0f, 0.2f, 0.1f) );
		inline constexpr gClass STATICBODYACTOR  ( "StaticBodyActor",    3,    &createNewActor<StaticBodyActor>,  glm::vec3(1.0f, 0.6f, 0.4f) );
		inline constexpr gClass RIGIDBODYACTOR   ( "RigidBodyActor",     4,    &createNewActor<RigidBodyActor>,   glm::vec3(1.0f, 0.6f, 0.1f) );
		inline constexpr gClass CAMERA           ( "Camera",             5,    &createNewActor<Camera>,           glm::vec3(0.0f, 0.0f, 0.0f) );
		inline constexpr gClass GRAPHXPLAYER     ( "GraphXPlayer",       6,    &createNewActor<GraphXPlayer>,     glm::vec3(0.0f, 0.0f, 0.0f) );
		inline constexpr gClass RAMIEL           ( "Ramiel",             7,    &createNewActor<Ramiel>,           glm::vec3(0.3f, 0.1f, 1.0f) );
		inline constexpr gClass LABEL            ( "Label",              8,    &createNewActor<Label>,            glm::vec3(0.0f, 0.0f, 0.0f) );

		// ALL LIGHT DERIVED CLASSES MUST USE NEGATIVE TYPE IDS IN ORDER FOR graphx::classes::isLight TO WORK
		inline constexpr gClass LIGHT            ( "Light",             -1,    &createNewActor<Light>,            glm::vec3(1.0f, 1.0f, 1.0f) );
		inline constexpr gClass LIGHTDIRECTIONAL ( "LightDirectional",  -2,    &createNewActor<LightDirectional>, glm::vec3(1.0f, 1.0f, 1.0f) );
		inline constexpr gClass LIGHTSPOT        ( "LightSpot",         -3,    &createNewActor<LightSpot>,        glm::vec3(0.5f, 1.0f, 0.5f) );
		inline constexpr gClass LIGHTFLASHLIGHT  ( "LightFlashlight",   -4,    &createNewActor<LightFlashlight>,  glm::vec3(1.0f, 1.0f, 1.0f) );
		inline constexpr gClass LIGHTTESTERMOVER ( "LightTesterMover",  -5,    &createNewActor<LightTesterMover>, glm::vec3(1.0f, 1.0f, 1.0f) );

		inline constexpr gClass DEVICE           ( "Device",      AL+    1,    &createNewDevice<Device>,          glm::vec3(0.0f, 0.0f, 0.0f) );
		inline constexpr gClass ENVIRONMENT      ( "Environment", AL+    2,    &createNewDevice<Environment>,     glm::vec3(0.0f, 0.0f, 0.0f) );
		inline constexpr gClass MATERIAL         ( "Material",    AL+    3,    &createNewDevice<Material>,        glm::vec3(0.0f, 0.0f, 0.0f) );
		inline constexpr gClass MESH             ( "Mesh",        AL+    4,    &createNewDevice<Mesh>,            glm::vec3(0.0f, 0.0f, 0.0f) );
		inline constexpr gClass SPRITE           ( "Sprite",      AL+    5,    &createNewDevice<Sprite>,          glm::vec3(0.0f, 0.0f, 0.0f) );
		inline constexpr gClass COLLIDER         ( "Collider",    AL+    6,    &createNewDevice<Collider>,        glm::vec3(0.0f, 0.0f, 0.0f) );
		inline constexpr gClass TEXTURE          ( "Texture",     AL+    7,    &createNewDevice<Texture>,         glm::vec3(0.0f, 0.0f, 0.0f) );

		// Don't forget to add your gClass variable to the `valid_classes` array!
		inline constexpr std::array<const gClass*, (ACTOR_LIMIT + DEVICE_LIMIT)> valid_classes =
		{
			// Actors
			&ACTOR,
			&PHYSICSACTOR,
			&STATICBODYACTOR,
			&RIGIDBODYACTOR,
			&CAMERA,
			&GRAPHXPLAYER,
			&RAMIEL,
			&LABEL,
			&LIGHT,
			&LIGHTDIRECTIONAL,
			&LIGHTSPOT,
			&LIGHTFLASHLIGHT,
			&LIGHTTESTERMOVER,
			// Devices
			&DEVICE,
			&ENVIRONMENT,
			&MATERIAL,
			&MESH,
			&SPRITE,
			&COLLIDER,
			&TEXTURE,
		};

		inline bool isValidClass(const gClass& type)
		{
			for(auto valid_class = valid_classes.begin() ; *valid_class != INVALID_TYPE ; valid_class++)
				if(*valid_class == type)
					return true;
			return false;
		}

		inline const gClass& getClassType(const gClass& type)
		{
			for(auto valid_class = valid_classes.begin() ; *valid_class != INVALID_TYPE ; valid_class++)
				if(*valid_class == type)
					return **valid_class;
			return INVALID_TYPE;
		}

		inline const gClass& getBaseType(const gClass& type) noexcept
		{
			if(INVALID_TYPE == type)
				return INVALID_TYPE;

			const int type_id = abs(type.id); // Because Lights use negative values

			if(type_id <= ACTOR_LIMIT && ACTOR <= type_id)
				return ACTOR;

			if(type_id <= DEVICE_LIMIT && DEVICE <= type_id)
				return DEVICE;

			return INVALID_TYPE;
		}

		inline const bool isLight(const gClass& type) noexcept
		{
			// return (getBaseType(type) == ACTOR && type < 0); // Not necessary unless I decide to let Devices have negative ids
			return(type < 0);
		}
	}
}
#endif