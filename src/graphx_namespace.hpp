#ifndef GRAPHX_NAMESPACE
#define GRAPHX_NAMESPACE
#include <map>
#include <any>
#include <array>
#include <string>
#include <vector>
#include <unordered_map>

#define GRAPHXTHEATRE_EXTENSION std::string(".gt")

namespace graphx
{
	namespace classes
	{
		static constexpr int THEATRE			= 0;
		static constexpr int ACTOR				= 1;
		static constexpr int PHYSICSACTOR		= 2;
		static constexpr int RIGIDBODYACTOR		= 3;
		static constexpr int STATICBODYACTOR	= 11;
		static constexpr int CAMERA				= 4;
		static constexpr int GRAPHXPLAYER		= 5;
		static constexpr int LIGHT				= 6;
		static constexpr int LIGHTDIRECTIONAL	= 7;
		static constexpr int LIGHTSPOT			= 8;
		static constexpr int LIGHTFLASHLIGHT	= 9;
		static constexpr int LIGHTTESTERMOVER	= 10;

		static constexpr int DEVICE				= 500;
		static constexpr int ENVIRONMENT		= 501;
		static constexpr int MATERIAL			= 502;
		static constexpr int MESH				= 503;
		static constexpr int SPRITE				= 504;
		static constexpr int COLLIDER			= 505;

		static constexpr int ACTORS[2] = {0, 499};
		static constexpr int DEVICES[2] = {500, 999};
		static constexpr std::array<int, 5> LIGHTS =
		{
			LIGHT,
			LIGHTSPOT,
			LIGHTFLASHLIGHT,
			LIGHTDIRECTIONAL,
			LIGHTTESTERMOVER,
		};
	}

	typedef std::map<int, std::pair<std::string, std::string>> gObjectStore;
	typedef std::multimap<int, std::pair<std::string, std::string>> gSourceRefStore;
	typedef std::multimap<int, std::pair<std::string, int>> gTheatreRefStore;
	typedef std::multimap<int, std::pair<std::string, std::string>> gRawDataStore;
	typedef std::multimap<int, std::pair<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>>> gSandwichStore;
	typedef std::tuple<std::string, gObjectStore, gSourceRefStore, gTheatreRefStore, gRawDataStore, gSandwichStore> gTheatreStorage;

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

	typedef std::pair<int, std::string> gSandwichPair;
	typedef std::tuple<std::vector<float>, std::vector<unsigned int>, int> gMeshData;
	typedef std::vector<std::string> gRawData;

	static std::map<int, std::string> classnames =
	{
		{classes::THEATRE, "Theatre"},
		{classes::ACTOR, "Actor"},
		{classes::PHYSICSACTOR, "PhysicsActor"},
		{classes::RIGIDBODYACTOR, "RigidBodyActor"},
		{classes::STATICBODYACTOR, "StaticBodyActor"},
		{classes::CAMERA, "Camera"},
		{classes::GRAPHXPLAYER, "GraphXPlayer"},
		{classes::LIGHT, "Light"},
		{classes::LIGHTDIRECTIONAL, "LightDirectional"},
		{classes::LIGHTSPOT, "LightSpot"},
		{classes::LIGHTFLASHLIGHT, "LightFlashlight"},
		{classes::LIGHTTESTERMOVER, "LightTesterMover"},
		{classes::DEVICE, "Device"},
		{classes::ENVIRONMENT, "Environment"},
		{classes::MATERIAL, "Material"},
		{classes::MESH, "Mesh"},
		{classes::SPRITE, "Sprite"},
		{classes::COLLIDER, "Collider"}
	};
}
#endif