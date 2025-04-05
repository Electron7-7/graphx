#ifndef GRAPHX_CLASSES_NAMESPACE
#define GRAPHX_CLASSES_NAMESPACE
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
	// If you need more than 1000 IDs for Actors and/or Devices (or you want to change the ranges), you'll need to change gClass::ACTOR_ID_LIMIT & gClass::DEVICE_ID_LIMIT
	namespace classes
	{
		//                             variable            name                  ID    class constructor function
		//-----------------------------------------------------------------------------------------------------------------
		inline static constexpr gClass ACTOR            ( "Actor",               1,    &createNewActor<Actor>            );
		inline static constexpr gClass PHYSICSACTOR     ( "PhysicsActor",        2,    &createNewActor<PhysicsActor>     );
		inline static constexpr gClass STATICBODYACTOR  ( "StaticBodyActor",     3,    &createNewActor<StaticBodyActor>  );
		inline static constexpr gClass RIGIDBODYACTOR   ( "RigidBodyActor",      4,    &createNewActor<RigidBodyActor>   );
		inline static constexpr gClass CAMERA           ( "Camera",              5,    &createNewActor<Camera>           );
		inline static constexpr gClass GRAPHXPLAYER     ( "GraphXPlayer",        6,    &createNewActor<GraphXPlayer>     );
		inline static constexpr gClass RAMIEL           ( "Ramiel",              7,    &createNewActor<Ramiel>           );
		inline static constexpr gClass LABEL            ( "Label",               8,    &createNewActor<Label>            );

		// ALL LIGHT DERIVED CLASSES MUST USE NEGATIVE TYPE IDS IN ORDER FOR graphx::classes::isLight TO WORK
		inline static constexpr gClass LIGHT            ( "Light",              -1,    &createNewActor<Light>            );
		inline static constexpr gClass LIGHTDIRECTIONAL ( "LightDirectional",   -2,    &createNewActor<LightDirectional> );
		inline static constexpr gClass LIGHTSPOT        ( "LightSpot",          -3,    &createNewActor<LightSpot>        );
		inline static constexpr gClass LIGHTFLASHLIGHT  ( "LightFlashlight",    -4,    &createNewActor<LightFlashlight>  );
		inline static constexpr gClass LIGHTTESTERMOVER ( "LightTesterMover",   -5,    &createNewActor<LightTesterMover> );

		inline static constexpr gClass DEVICE           ( "Device",           1000,    &createNewDevice<Device>          );
		inline static constexpr gClass ENVIRONMENT      ( "Environment",      1001,    &createNewDevice<Environment>     );
		inline static constexpr gClass MATERIAL         ( "Material",         1002,    &createNewDevice<Material>        );
		inline static constexpr gClass MESH             ( "Mesh",             1003,    &createNewDevice<Mesh>            );
		inline static constexpr gClass SPRITE           ( "Sprite",           1004,    &createNewDevice<Sprite>          );
		inline static constexpr gClass COLLIDER         ( "Collider",         1005,    &createNewDevice<Collider>        );
		inline static constexpr gClass TEXTURE          ( "Texture",          1006,    &createNewDevice<Texture>         );

		// Don't forget to add your gClass variable to the `valid_classes` array!
		inline static constexpr std::array<gClass, (gClass::ACTOR_ID_LIMIT + gClass::DEVICE_ID_LIMIT)> valid_classes =
		{
			ACTOR,
			PHYSICSACTOR,
			STATICBODYACTOR,
			RIGIDBODYACTOR,
			CAMERA,
			GRAPHXPLAYER,
			RAMIEL,
			LIGHT,
			LIGHTDIRECTIONAL,
			LIGHTSPOT,
			LIGHTFLASHLIGHT,
			LIGHTTESTERMOVER,
			DEVICE,
			ENVIRONMENT,
			MATERIAL,
			MESH,
			SPRITE,
			COLLIDER,
			TEXTURE,
		};

		inline const gClass &getBaseType(const gClass &type) noexcept
		{
			if(gClass::INVALID_TYPE == type)
				return gClass::INVALID_TYPE;

			int type_id = abs(type.id);

			if(type_id <= gClass::ACTOR_ID_LIMIT && ACTOR <= type_id)
				return ACTOR;

			if(type_id <= gClass::DEVICE_ID_LIMIT && DEVICE <= type_id)
				return DEVICE;

			return gClass::INVALID_TYPE;
		}

		inline const bool isLight(const gClass &type) noexcept
		{
			if(getBaseType(type) != ACTOR)
				return false;

			if(type > 0)
				return false;

			return true;
		}
	};

	inline const gClass gClass::INVALID_TYPE = gClass(); // The default constructor for `gClass` is `INVALID_TYPE`
	inline const std::array<gClass, (gClass::ACTOR_ID_LIMIT + gClass::DEVICE_ID_LIMIT)> &gClass::classes = classes::valid_classes;
}
#endif