#ifndef GRAPHX_CLASSES_NAMESPACE
#define GRAPHX_CLASSES_NAMESPACE
#include "graphx_namespace.hpp"
#include "g_actors.hpp"
#include "r_common.hpp"
namespace graphx
{
	inline const gClass gClass::INVALID_TYPE = gClass(); // The default constructor for `gClass` is `INVALID_TYPE`

	namespace classes
	{
		inline constexpr gClass ACTOR            ( "Actor",               1, &createNewActor<Actor>            );
		inline constexpr gClass PHYSICSACTOR     ( "PhysicsActor",        2, &createNewActor<PhysicsActor>     );
		inline constexpr gClass STATICBODYACTOR  ( "StaticBodyActor",     3, &createNewActor<RigidBodyActor>   );
		inline constexpr gClass RIGIDBODYACTOR   ( "RigidBodyActor",      4, &createNewActor<StaticBodyActor>  );
		inline constexpr gClass CAMERA           ( "Camera",              5, &createNewActor<Camera>           );
		inline constexpr gClass GRAPHXPLAYER     ( "GraphXPlayer",        6, &createNewActor<GraphXPlayer>     );
		inline constexpr gClass RAMIEL           ( "Ramiel",              7, &createNewActor<Light>            );

		// ALL LIGHT DERIVED CLASSES MUST USE NEGATIVE TYPE IDS IN ORDER FOR graphx::classes::isLight TO WORK
		inline constexpr gClass LIGHT            ( "Light",              -1, &createNewActor<LightDirectional> );
		inline constexpr gClass LIGHTDIRECTIONAL ( "LightDirectional",   -2, &createNewActor<LightSpot>        );
		inline constexpr gClass LIGHTSPOT        ( "LightSpot",          -3, &createNewActor<LightFlashlight>  );
		inline constexpr gClass LIGHTFLASHLIGHT  ( "LightFlashlight",    -4, &createNewActor<LightTesterMover> );
		inline constexpr gClass LIGHTTESTERMOVER ( "LightTesterMover",   -5, &createNewActor<Ramiel>           );

		inline constexpr gClass DEVICE           ( "Device",           1000, &createNewDevice<Device>          );
		inline constexpr gClass ENVIRONMENT      ( "Environment",      1001, &createNewDevice<Environment>     );
		inline constexpr gClass MATERIAL         ( "Material",         1002, &createNewDevice<Material>        );
		inline constexpr gClass MESH             ( "Mesh",             1003, &createNewDevice<Mesh>            );
		inline constexpr gClass SPRITE           ( "Sprite",           1004, &createNewDevice<Sprite>          );
		inline constexpr gClass COLLIDER         ( "Collider",         1005, &createNewDevice<Collider>        );
		inline constexpr gClass TEXTURE          ( "Texture",          1006, &createNewDevice<Texture>         );

		inline const gClass &getBaseType(const gClass &type) noexcept
		{
			if(type == gClass::INVALID_TYPE)
				return gClass::INVALID_TYPE;

			if(type <= gClass::ACTOR_ID_LIMIT && type >= ACTOR)
				return ACTOR;

			if(type <= gClass::DEVICE_ID_LIMIT && type >= DEVICE)
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
}
#endif