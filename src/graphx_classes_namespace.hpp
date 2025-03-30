#ifndef GRAPHX_CLASSES_NAMESPACE
#define GRAPHX_CLASSES_NAMESPACE
#include "graphx_namespace.hpp"
#include "g_actors.hpp"
#include "r_common.hpp"
namespace graphx
{
	namespace classes
	{
		inline const gClass INVALID_TYPE; // The default constructor for `gClass` is `INVALID_TYPE`

		inline const gClass ACTOR            ( "Actor",               1, &createNewActor<Actor>            );
		inline const gClass PHYSICSACTOR     ( "PhysicsActor",        2, &createNewActor<PhysicsActor>     );
		inline const gClass STATICBODYACTOR  ( "StaticBodyActor",     3, &createNewActor<RigidBodyActor>   );
		inline const gClass RIGIDBODYACTOR   ( "RigidBodyActor",      4, &createNewActor<StaticBodyActor>  );
		inline const gClass CAMERA           ( "Camera",              5, &createNewActor<Camera>           );
		inline const gClass GRAPHXPLAYER     ( "GraphXPlayer",        6, &createNewActor<GraphXPlayer>     );
		inline const gClass RAMIEL           ( "Ramiel",              7, &createNewActor<Light>            );

		// ALL LIGHT DERIVED CLASSES MUST USE NEGATIVE TYPE IDS IN ORDER FOR graphx::classes::isLight TO WORK
		inline const gClass LIGHT            ( "Light",              -1, &createNewActor<LightDirectional> );
		inline const gClass LIGHTDIRECTIONAL ( "LightDirectional",   -2, &createNewActor<LightSpot>        );
		inline const gClass LIGHTSPOT        ( "LightSpot",          -3, &createNewActor<LightFlashlight>  );
		inline const gClass LIGHTFLASHLIGHT  ( "LightFlashlight",    -4, &createNewActor<LightTesterMover> );
		inline const gClass LIGHTTESTERMOVER ( "LightTesterMover",   -5, &createNewActor<Ramiel>           );

		inline const gClass DEVICE           ( "Device",           1000, &createNewDevice<Device>          );
		inline const gClass ENVIRONMENT      ( "Environment",      1001, &createNewDevice<Environment>     );
		inline const gClass MATERIAL         ( "Material",         1002, &createNewDevice<Material>        );
		inline const gClass MESH             ( "Mesh",             1003, &createNewDevice<Mesh>            );
		inline const gClass SPRITE           ( "Sprite",           1004, &createNewDevice<Sprite>          );
		inline const gClass COLLIDER         ( "Collider",         1005, &createNewDevice<Collider>        );
		inline const gClass TEXTURE          ( "Texture",          1006, &createNewDevice<Texture>         );

		// Feel free to expand these limits if needed; just remember to update the above values accordingly
		inline const int ACTOR_ID_LIMIT    = 999;
		inline const int DEVICE_ID_LIMIT   = 1999;

		inline const gClass &getBaseType(const gClass &type) noexcept
		{
			if(type <= ACTOR_ID_LIMIT && type >= ACTOR)
				return ACTOR;

			if(type <= DEVICE_ID_LIMIT && type >= DEVICE)
				return DEVICE;

			return INVALID_TYPE;
		}

		inline const bool isLight(const gClass &type) noexcept
		{
			if(type == INVALID_TYPE || getBaseType(type) != ACTOR)
				return false;

			if(type > 0)
				return false;

			return true;
		}
	};
}
#endif