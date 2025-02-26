#ifndef GRAPHX_SETTINGS
#define GRAPHX_SETTINGS
#include "sanity.hpp"
#include "graphx_namespace.hpp"

// Forward Declarations
struct Device;
class Actor;

extern std::string empty_settings_identifier;
extern graphx::gSettings empty_settings;

template<typename T> void getSetting(T *variable, std::any set_value, const std::type_info &std_any_type) 
{
	if (std_any_type == typeid(void))
	{
		return;
	}

	else
	{
		PRINTDEBUG(typeid(*variable).name())
		PRINTDEBUG(std_any_type.name())
	}
}

template<typename T> void checkSetting(T &variable, std::any set_value)
{
	getSetting<T>(&variable, set_value, set_value.type());
}

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
			variable = (new_value[0] == "true" || "True");
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
#endif