#ifndef GRAPHX_CLASSES_NAMESPACE
#define GRAPHX_CLASSES_NAMESPACE
#include "graphx_namespace.hpp"
#include "g_actors.hpp"
#include "r_common.hpp"
namespace graphx
{
	inline const gClass gClass::INVALID_TYPE = gClass(); // The default constructor for `gClass` is `INVALID_TYPE`

	// If you need more than 1000 IDs for Actors and/or Devices (or you want to change the ranges),
	// you'll need to change gClass::ACTOR_ID_LIMIT & gClass::DEVICE_ID_LIMIT.

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

		inline static constexpr std::array<gClass, (gClass::ACTOR_ID_LIMIT + gClass::DEVICE_ID_LIMIT)> classes =
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

	inline const std::array<gClass, (gClass::ACTOR_ID_LIMIT + gClass::DEVICE_ID_LIMIT)> *gClass::classes = &classes::classes;
}
#endif