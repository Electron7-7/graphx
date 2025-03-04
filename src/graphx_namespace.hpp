#ifndef GRAPHX_NAMESPACE
#define GRAPHX_NAMESPACE
#include <map>
#include <any>
#include <array>
#include <string>
#include <vector>
#include <unordered_map>

#ifndef GRAPHX_ACTOR_DEFINED
class Actor; // Forward Declaration
#endif
#ifndef GRAPHX_DEVICE_DEFINED
struct Device; // Forward Declaration
#endif

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

	typedef std::string gKey;
	typedef std::pair<int, std::string> gValue;
	typedef std::pair<gKey, gValue> gStringSetting;
	typedef std::vector<std::vector<gStringSetting>> gStringSettings;

	typedef std::pair<int, std::any> gSetting;
	typedef std::unordered_map<std::string, gSetting> gSettings;

	// typedef std::any gSetting;
	// typedef std::unordered_map<std::string, gSetting> gSettings;

	typedef std::pair<long, Actor *> gActorUID;
	typedef std::pair<long, Device *> gDeviceUID;

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