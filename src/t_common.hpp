#ifndef GRAPHX_THEATRE_FILE_FORMAT
#define GRAPHX_THEATRE_FILE_FORMAT
#include "g_common.hpp"
#include <map>
#include <any>
#include <tuple>
#include <string>
#include <vector>
#include <unordered_map>
extern int current_theatre_uid;
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
	static constexpr int DEVICE				= 11;
	static constexpr int ENVIRONMENT		= 12;
	static constexpr int MATERIAL			= 13;
	static constexpr int MESH				= 14;
	static constexpr int SPRITE				= 15;
	static constexpr int COLLIDER			= 16;

	static constexpr int ACTORS[2] = {ACTOR, LIGHTTESTERMOVER};
	static constexpr int DEVICES[2] = {DEVICE, COLLIDER};
};

typedef std::map<int, std::pair<std::string, std::string>>														gObjectStore;
typedef std::multimap<int, std::pair<std::string, std::string>>													gSourceRefStore;
typedef std::multimap<int, std::pair<std::string, int>>															gTheatreRefStore;
typedef std::multimap<int, std::pair<std::string, std::string>>													gRawDataStore;
typedef std::multimap<int, std::pair<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>>>	gSandwichStore;
typedef std::tuple<std::string, gObjectStore, gSourceRefStore, gTheatreRefStore, gRawDataStore, gSandwichStore>	gTheatreStorage;
typedef std::unordered_map<std::string, std::any>																gSettings;
typedef std::pair<int, std::string>																				gSandwichPair;
typedef std::map<int, Actor*(*)()>																				gActorMap;
typedef std::map<int, Device*(*)()>																				gDeviceMap;
typedef std::tuple<std::vector<float>, std::vector<unsigned int>, int>											gMeshData;
typedef std::vector<std::string>																				gRawData;

extern gActorMap actor_map;
extern gDeviceMap device_map;

extern gSettings null_settings;

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
extern gSettings environment_settings;
extern gSettings material_settings;
extern gSettings mesh_settings;
extern gSettings sprite_settings;
extern gSettings collider_settings;

extern std::unordered_map<int, std::pair<int, gSettings>> settings_map;

template<typename T, typename A> A *createNewObject() { return new T; }
template<typename T> std::any getVariableFrom(T *object_pointer, std::string variable_name);
template<typename T> void setDevicePointer(T &variable, std::any set_value)
{
	if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
	{
		return;
	}
	else
	{
		variable = static_cast<T>(std::any_cast<Device *>(set_value));
	}
}
template<typename T> void setActorPointer(T &variable, std::any set_value)
{
	if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
	{
		return;
	}
	else
	{
		variable = static_cast<T>(std::any_cast<Actor *>(set_value));
	}
}

template<typename T> void setRawData(T &variable, std::any set_value)
{
	if constexpr(std::is_same_v<T, std::string>)
	{
		if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
			return;
		else
		{
			gRawData new_value = std::any_cast<gRawData>(set_value);
			variable = static_cast<T>(new_value[0]);
		}
	}

	else if constexpr(std::is_same_v<T, bool>)
	{
		if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
			return;
		else
		{
			gRawData new_value = std::any_cast<gRawData>(set_value);
			variable = (new_value[0] == "true");
		}
	}

	else if constexpr(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
	{
		if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
			return;
		else
		{
			gRawData new_value = std::any_cast<gRawData>(set_value);
			variable = static_cast<T>(std::stod(new_value[0]));
		}
	}

	else if constexpr(std::is_same_v<T, glm::vec2>)
	{
		if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
			return;
		else
		{
			gRawData new_value = std::any_cast<gRawData>(set_value);
			variable = glm::vec2(std::stof(new_value[0]), std::stof(new_value[1]));
		}
	}

	else if constexpr(std::is_same_v<T, glm::vec3>)
	{
		if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
			return;
		else
		{
			gRawData new_value = std::any_cast<gRawData>(set_value);
			variable = glm::vec3(std::stof(new_value[0]), std::stof(new_value[1]), std::stof(new_value[2]));
		}
	}

	else if constexpr(std::is_same_v<T, glm::quat>)
	{
		if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
			return;
		else
		{
			gRawData new_value = std::any_cast<gRawData>(set_value);
			variable = glm::quat(std::stof(new_value[0]), std::stof(new_value[1]), std::stof(new_value[2]), std::stof(new_value[3]));
		}
	}
}

template<typename T> void setVariable(T &variable, std::any set_value)
{
	if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
		return;
	else
		variable = std::any_cast<T>(set_value);
}

gSettings getSettingsTemplate(std::string class_name);
gRawData extractData(std::string data_in_here);
void createNewClass(std::string class_name, int object_uid, gSettings class_settings, Theatre &parent_theatre);
gTheatreStorage theatreParser(std::string theatre_data);
std::string getTheatreStructure(gTheatreStorage theatre_storage);
int getClassHash(std::string class_name);
Theatre *loadTheatre(std::string embedded_theatre);
#endif
