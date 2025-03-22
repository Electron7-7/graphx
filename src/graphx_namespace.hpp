#ifndef GRAPHX_NAMESPACE
#define GRAPHX_NAMESPACE
#include <glm/glm.hpp>
#include <map>
#include <any>
#include <string>
#include <vector>
#include <unordered_map>

#define GRAPHXTHEATRE_EXTENSION std::string(".gt")

namespace graphx
{
	namespace error
	{
		namespace rendercmd
		{
			static constexpr int MISSING_VBO_NAME           = 1 << 0; // 1
			static constexpr int MISSING_MESH_DATA_SIZE     = 1 << 1; // 2
			static constexpr int MISSING_MESH_DATA_OFFSET   = 1 << 2; // 4
			static constexpr int MISSING_BOTH_RENDER_STATES = 1 << 3; // 8

		};
	};

	namespace identifiers
	{
		namespace primitive
		{
			static constexpr int FOO      = -1;
			static constexpr int LINE     = 0;
			static constexpr int TRIANGLE = 1;
			static constexpr int TEXT     = -1; // Text not supported yet!
		};

		namespace buffer_type
		{
			static constexpr int IN_USE      =  1;
			static constexpr int NOT_IN_USE  = -1;
			static constexpr int NOT_CHECKED =  0;
		};
	};

	namespace classes
	{
		static constexpr int INVALID_TYPE       = -481516;

		static constexpr int THEATRE			= 0;
		static constexpr int ACTOR				= 1;
		static constexpr int PHYSICSACTOR		= 2;
		static constexpr int STATICBODYACTOR	= 3;
		static constexpr int RIGIDBODYACTOR		= 4;
		static constexpr int CAMERA				= 5;
		static constexpr int GRAPHXPLAYER		= 6;
		static constexpr int RAMIEL				= 7;
		// ALL LIGHT DERIVED CLASS IDS MUST BE NEGATIVE IN ORDER FOR graphx::classes::isLight TO WORK
		static constexpr int LIGHT				= -1;
		static constexpr int LIGHTDIRECTIONAL	= -2;
		static constexpr int LIGHTSPOT			= -3;
		static constexpr int LIGHTFLASHLIGHT	= -4;
		static constexpr int LIGHTTESTERMOVER	= -5;

		static constexpr int DEVICE				= 1000;
		static constexpr int ENVIRONMENT		= 1001;
		static constexpr int MATERIAL			= 1002;
		static constexpr int MESH				= 1003;
		static constexpr int SPRITE				= 1004;
		static constexpr int COLLIDER			= 1005;
		static constexpr int TEXTURE			= 1006;

		// Feel free to expand these limits if needed; just remember to update the above values accordingly
		static constexpr int ACTOR_ID_LIMIT    = 999;
		static constexpr int DEVICE_ID_LIMIT   = 1999;

		// Don't forget to add new classes to this map after giving them an ID!
		static inline const std::map<std::string, int> classnames =
		{
			{ "Theatre",          THEATRE          },

			{ "Actor",            ACTOR            },
			{ "PhysicsActor",     PHYSICSACTOR     },
			{ "StaticBodyActor",  STATICBODYACTOR  },
			{ "RigidBodyActor",   RIGIDBODYACTOR   },
			{ "Camera",           CAMERA           },
			{ "GraphXPlayer",     GRAPHXPLAYER     },
			{ "Ramiel",           RAMIEL           },
			{ "Light",            LIGHT            },
			{ "LightDirectional", LIGHTDIRECTIONAL },
			{ "LightSpot",        LIGHTSPOT        },
			{ "LightFlashlight",  LIGHTFLASHLIGHT  },
			{ "LightTesterMover", LIGHTTESTERMOVER },

			{ "Device",           DEVICE           },
			{ "Environment",      ENVIRONMENT      },
			{ "Material",         MATERIAL         },
			{ "Mesh",             MESH             },
			{ "Sprite",           SPRITE           },
			{ "Collider",         COLLIDER         },
			{ "Texture",          TEXTURE          },
		};

		static inline constexpr int getBaseType(int type) noexcept
		{
			type = std::abs(type);

			if(type == THEATRE)
				return THEATRE;

			if(type <= ACTOR_ID_LIMIT && type >= ACTOR)
				return ACTOR;

			if(type <= DEVICE_ID_LIMIT && type >= DEVICE)
				return DEVICE;

			return INVALID_TYPE;
		}

		static inline constexpr bool isLight(int type) noexcept
		{
			if(type == INVALID_TYPE || getBaseType(type) != ACTOR)
				return false;

			if(type > 0)
				return false;

			return true;
		}
	};

	typedef std::map<int, std::pair<std::string, std::string>> gObjectStore;
	typedef std::multimap<int, std::pair<std::string, std::string>> gSourceRefStore;
	typedef std::multimap<int, std::pair<std::string, int>> gTheatreRefStore;
	typedef std::multimap<int, std::pair<std::string, std::string>> gRawDataStore;
	typedef std::multimap<int, std::pair<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>>> gSandwichStore;
	typedef std::tuple<std::string, gObjectStore, gSourceRefStore, gTheatreRefStore, gRawDataStore, gSandwichStore> gTheatreStorage;
	typedef std::pair<int, std::string> gSandwichPair;
	typedef std::vector<std::string> gRawData;

	// gKey, gValue, gStringSetting, and gStringSettings are for the interpreter/parser only
	// and shouldn't be used by anything else (except for Theatre::graphx_theatre_settings)
	typedef std::string gKey;
	typedef std::pair<int, std::string> gValue;
	typedef std::pair<gKey, gValue> gStringSetting;
	typedef std::vector<std::vector<gStringSetting>> gStringSettings;

	// The int in gSetting identifies the type (found in t_common.hpp); the four types are:
	// 1: RAW_DATA (a vector of strings, using the typedef "gRawData")
	// 2: CPP_REFERENCE (see "cpp_definitions" in "t_interpreter.cpp")
	// 3: THEATRE_REFERENCE (a pointer to a pre-existing Actor/Device in the current Theatre)
	// 4: EXTERNAL_REFERENCE (an external file's pathname, passed via string)
	// 5: SANDWICH_BUN (a unique copy of a pre-existing Actor/Device in the current Theatre)
	typedef std::pair<int, std::any> gSetting;
	typedef std::unordered_map<std::string, gSetting> gSettings;
}
#endif
