#ifndef GRAPHX_THEATRE_FILE_FORMAT
#define GRAPHX_THEATRE_FILE_FORMAT
#include "g_actors.hpp"
// #include "g_jolt.hpp"
#include <map>
#include <any>
#include <set>
#include <tuple>
#include <string>
#include <vector>
#include <unordered_map>
extern int current_theatre_uid;
extern std::unordered_map<int, Theatre> all_theatres;
extern std::unordered_map<std::string, int> graphx_class_names;
extern std::unordered_map<std::string, std::any> cpp_definitions;

namespace graphx_classes
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
	static constexpr int ENVIRONMENT		= 11;
	static constexpr int MATERIAL			= 12;
	static constexpr int MESH				= 13;
	static constexpr int SPRITE				= 14;
	static constexpr int COLLIDER			= 15;

	static constexpr int ACTORS[2] = {ACTOR, LIGHTTESTERMOVER};
	static constexpr int DEVICES[2] = {ENVIRONMENT, COLLIDER};
};

typedef std::map<int, std::pair<std::string, std::string>>														gObjectStore;
typedef std::multimap<int, std::pair<std::string, std::string>>													gSourceRefStore;
typedef std::multimap<int, std::pair<std::string, int>>															gTheatreRefStore;
typedef std::multimap<int, std::pair<std::string, std::string>>													gRawDataStore;
typedef std::multimap<int, std::pair<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>>>	gSandwichStore;
typedef std::tuple<std::string, gObjectStore, gSourceRefStore, gTheatreRefStore, gRawDataStore, gSandwichStore>	gTheatreStorage;
typedef std::unordered_map<std::string, std::any> 																gSettings;
typedef std::pair<int, std::string>																				gSandwichPair;
typedef std::map<int, Actor*(*)()>																				gActorMap;
typedef std::map<int, Device*(*)()>																				gDeviceMap;

extern gActorMap actor_map;
extern gDeviceMap device_map;

extern gSettings actor_settings;
extern gSettings physics_actor_settings;
extern gSettings rigidbody_actor_settings;
extern gSettings camera_settings;
extern gSettings graphxplayer_settings;
extern gSettings light_settings;
extern gSettings light_directional_settings;
extern gSettings light_spot_settings;
extern gSettings light_flashlight_settings;
extern gSettings light_tester_mover_settings;

extern std::unordered_map<int, std::pair<int, gSettings>> settings_map;

template<typename T, typename A> A *createNewObject() { return new T; }
template<typename T> std::any getVariableFrom(T *object_pointer, std::string variable_name);

std::any getNumber(std::vector<std::string> string_input, char type);
std::any extractData(std::string data_in_here);
std::vector<gSettings> getSettingsTemplate(std::string class_name);
void createNewClass(std::string class_name, int object_uid, std::vector<gSettings> class_settings, Theatre *parent_theatre);
int loadTheatre(std::string embedded_theatre);
gTheatreStorage theatreParser(std::string theatre_data);
std::string getTheatreStructure(gTheatreStorage theatre_storage);
int getClassHash(std::string class_name);
Theatre *loadTheatre(std::string embedded_theatre);
#endif
