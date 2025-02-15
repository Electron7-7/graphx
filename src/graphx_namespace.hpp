#ifndef GRAPHX_NAMESPACE
#define GRAPHX_NAMESPACE
#include <map>
#include <unordered_map>
#include <any>
#include <string>
#include <vector>
namespace graphx
{
	namespace classes
	{
		static constexpr int THEATRE			= 0;
		static constexpr int ACTOR				= 1;
		static constexpr int PHYSICSACTOR		= 2;
		static constexpr int RIGIDBODYACTOR		= 3;
		static constexpr int CAMERA				= 4;
		static constexpr int GRAPHXPLAYER		= 5;
		static constexpr int LIGHT				= 6;
		static constexpr int LIGHTDIRECTIONAL	= 7;
		static constexpr int LIGHTSPOT			= 8;
		static constexpr int LIGHTFLASHLIGHT	= 9;
		static constexpr int LIGHTTESTERMOVER	= 10;
		static constexpr int DEVICE				= 11;
		static constexpr int ENVIRONMENT		= 12;
		static constexpr int MATERIAL			= 13;
		static constexpr int MESH				= 14;
		static constexpr int SPRITE				= 15;
		static constexpr int COLLIDER			= 16;

		static constexpr int ACTORS[2] = {ACTOR, LIGHTTESTERMOVER};
		static constexpr int DEVICES[2] = {DEVICE, COLLIDER};
	}

	typedef std::map<int, std::pair<std::string, std::string>>														gObjectStore;
	typedef std::multimap<int, std::pair<std::string, std::string>>													gSourceRefStore;
	typedef std::multimap<int, std::pair<std::string, int>>															gTheatreRefStore;
	typedef std::multimap<int, std::pair<std::string, std::string>>													gRawDataStore;
	typedef std::multimap<int, std::pair<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>>>	gSandwichStore;
	typedef std::tuple<std::string, gObjectStore, gSourceRefStore, gTheatreRefStore, gRawDataStore, gSandwichStore>	gTheatreStorage;
	typedef std::unordered_map<std::string, std::any>																gSettings;
	typedef std::pair<int, std::string>																				gSandwichPair;
	typedef std::tuple<std::vector<float>, std::vector<unsigned int>, int>											gMeshData;
	typedef std::vector<std::string>																				gRawData;
}
#endif