#ifndef GRAPHX_THEATRE_FILE_FORMAT
#define GRAPHX_THEATRE_FILE_FORMAT
#include "r_common.hpp"
#include "graphx_namespace.hpp"
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
			graphx::gRawData new_value = std::any_cast<graphx::gRawData>(set_value);
			variable = static_cast<T>(new_value[0]);
		}
	}

	else if constexpr(std::is_same_v<T, bool>)
	{
		if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
			return;
		else
		{
			graphx::gRawData new_value = std::any_cast<graphx::gRawData>(set_value);
			variable = (new_value[0] == "true");
		}
	}

	else if constexpr(std::is_arithmetic_v<T> && !std::is_same_v<T, bool>)
	{
		if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
			return;
		else
		{
			graphx::gRawData new_value = std::any_cast<graphx::gRawData>(set_value);
			variable = static_cast<T>(std::stod(new_value[0]));
		}
	}

	else if constexpr(std::is_same_v<T, glm::vec2>)
	{
		if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
			return;
		else
		{
			graphx::gRawData new_value = std::any_cast<graphx::gRawData>(set_value);
			variable = glm::vec2(std::stof(new_value[0]), std::stof(new_value[1]));
		}
	}

	else if constexpr(std::is_same_v<T, glm::vec3>)
	{
		if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
			return;
		else
		{
			graphx::gRawData new_value = std::any_cast<graphx::gRawData>(set_value);
			variable = glm::vec3(std::stof(new_value[0]), std::stof(new_value[1]), std::stof(new_value[2]));
		}
	}

	else if constexpr(std::is_same_v<T, glm::quat>)
	{
		if(set_value.type().name() == typeid(nullptr).name() || !set_value.has_value())
			return;
		else
		{
			graphx::gRawData new_value = std::any_cast<graphx::gRawData>(set_value);
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

extern int current_theatre_uid;
extern std::unordered_map<std::string, int> graphx_class_names;
extern std::unordered_map<std::string, std::any> cpp_definitions;
extern std::map<int, Actor*(*)()> actor_map;
extern std::map<int, Device*(*)()> device_map;

graphx::gTheatreStorage theatreParser(std::string theatre_data);
graphx::gRawData 		extractData(std::string data_in_here);
std::string 			getTheatreStructure(graphx::gTheatreStorage theatre_storage);
Theatre 			   *loadTheatre(std::string embedded_theatre, long theatre_uid);
void 					createNewClass(std::string class_name, int object_uid, graphx::gSettings class_settings, Theatre &parent_theatre);
int 					getClassHash(std::string class_name);
#endif
