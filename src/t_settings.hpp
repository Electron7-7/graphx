#ifndef GRAPHX_SETTINGS
#define GRAPHX_SETTINGS
#define GLM_ENABLE_EXPERIMENTAL
#include "sanity_printouts.hpp"
#include "graphx_namespace.hpp"
#include "t_common.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <Jolt/Jolt.h>

#ifdef COMPILER_FORWARD_DECLARATIONS // Forward declarations
class Actor;
struct Device;
#endif

#define DEFAULT_VALUE_SETTING 99

#define GRAB_SETTING_ERR_DEVICE_POINTER     -4
#define GRAB_SETTING_ERR_ACTOR_POINTER      -8
#define GRAB_SETTING_ERR_RAW_DATA_SINGLE    -15
#define GRAB_SETTING_ERR_RAW_DATA_TWO       -16
#define GRAB_SETTING_ERR_RAW_DATA_THREE     -23
#define GRAB_SETTING_ERR_RAW_DATA_FOUR      -42
#define GRAB_SETTING_ERR_CPP_REFERENCE      -69
#define GRAB_SETTING_ERR_SANDWICH_BUN       -420
#define GRAB_SETTING_ERR_EXTERNAL_REFERENCE -808

#define GRAB_SETTING_ERR_MSG_TEMPLATE std::string("getSetting called with non-matching variable and setting!")

extern std::string empty_settings_identifier;
extern graphx::gSettings empty_settings;

template<typename T> int getSetting(T& variable, graphx::gSetting& setting)
{
	std::any set_value = setting.second;
	int setting_type = setting.first;

	if((setting_type == -1 || setting_type == 0) || (!set_value.has_value() || set_value.type() == typeid(void) || set_value.type() == typeid(nullptr)))
	{
		setting = graphx::gSetting(DEFAULT_VALUE_SETTING, variable);
		return 0; // No setting to get
	}

	if(setting_type == DEFAULT_VALUE_SETTING)
	{
		variable = std::any_cast<T>(set_value);
		return 0;
	}

	if(setting_type == RAW_DATA)
	{
		graphx::interpreter::gRawData raw_data = std::any_cast<graphx::interpreter::gRawData>(set_value);
		std::string raw_data_lower = raw_data[0];
		std::transform(raw_data_lower.begin(), raw_data_lower.end(), raw_data_lower.begin(), [](unsigned char c)
		{
			return std::tolower(c);
		});

		switch(raw_data.size())
		{
		case 1:
			if constexpr(std::is_same_v<T, bool>)
			{
				variable = raw_data_lower.compare("false");
				return 0;
			}

			else if constexpr(std::is_arithmetic_v<T>)
			{ // Should only fire if T is number, since the previous if statmenet will fire when T is a bool
				variable = std::stod(raw_data[0]);
				return 0;
			}

			else if constexpr(std::is_same_v<T, std::string> || std::is_same_v<T, char> || std::is_same_v<T, char *>)
			{
				variable = raw_data[0];
				return 0;
			}

			else
			{
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << set_value.type().name() << " (gRawData)\n\tVariable type: " << typeid(variable).name())
				return GRAB_SETTING_ERR_RAW_DATA_SINGLE;
			}

			break;
		case 2:
			if constexpr(std::is_same_v<T, glm::vec2>)
			{
				variable = glm::vec2(std::stof(raw_data[0]), std::stof(raw_data[1]));
				return 0;
			}
			else
			{
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << set_value.type().name() << " (gRawData)\n\tVariable type: " << typeid(variable).name())
				return GRAB_SETTING_ERR_RAW_DATA_TWO;
			}
			break;
		case 3:
			if constexpr(std::is_same_v<T, glm::vec3>)
			{
				variable = glm::vec3(std::stof(raw_data[0]), std::stof(raw_data[1]), std::stof(raw_data[2]));
				return 0;
			}
			else if constexpr(std::is_same_v<T, JPH::Vec3>)
			{
				variable = JPH::Vec3(std::stof(raw_data[0]), std::stof(raw_data[1]), std::stof(raw_data[2]));
				return 0;
			}
			else
			{
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << set_value.type().name() << " (gRawData)\n\tVariable type: " << typeid(variable).name())
				return GRAB_SETTING_ERR_RAW_DATA_THREE;
			}
			break;
		case 4:
			if constexpr(std::is_same_v<T, glm::quat>)
			{
				variable = glm::quat(std::stof(raw_data[3]), std::stof(raw_data[0]), std::stof(raw_data[1]), std::stof(raw_data[2]));
				return 0;
			}
			else if constexpr(std::is_same_v<T, JPH::Quat>)
			{
				variable = JPH::Quat(std::stof(raw_data[0]), std::stof(raw_data[1]), std::stof(raw_data[2]), std::stof(raw_data[3]));
				return 0;
			}
			else
			{
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << set_value.type().name() << " (gRawData)\n\tVariable type: " << typeid(variable).name())
				return GRAB_SETTING_ERR_RAW_DATA_FOUR;
			}
			break;
		}
	}

	else if(setting_type == THEATRE_REFERENCE)
	{
		if constexpr(std::is_base_of_v<Device, std::remove_pointer_t<T>>)
		{
			if(set_value.type() != typeid(Device *))
			{
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << set_value.type().name() << " (Theatre Reference or Sandwich)\n\tVariable type: " << typeid(variable).name())
				return GRAB_SETTING_ERR_DEVICE_POINTER;
			}

			variable = static_cast<T>(std::any_cast<Device *>(set_value));
			return 0;
		}

		else if constexpr(std::is_base_of_v<Actor, std::remove_pointer_t<T>>)
		{
			if(set_value.type() != typeid(Actor *))
			{
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << set_value.type().name() << " (Theatre Reference or Sandwich)\n\tVariable type: " << typeid(variable).name())
				return GRAB_SETTING_ERR_ACTOR_POINTER;
			}

			variable = static_cast<T>(std::any_cast<Actor *>(set_value));
			return 0;
		}
	}

	else if(setting_type == SANDWICH)
	{
		if constexpr(std::is_base_of_v<Device, std::remove_pointer_t<T>>)
		{
			variable = static_cast<T>(std::any_cast<Device *>(set_value));
			return 0;
		}
		
		else if constexpr(std::is_base_of_v<Actor, std::remove_pointer_t<T>>)
		{
			variable = static_cast<T>(std::any_cast<Actor *>(set_value));
			return 0;
		}

		PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << set_value.type().name() << " (Sandwich Bun)\n\tVariable type: " << typeid(variable).name())
		return GRAB_SETTING_ERR_SANDWICH_BUN;
	}

	else if(setting_type == CPP_REFERENCE)
	{
		if(set_value.type() != typeid(variable))
		{
			PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << set_value.type().name() << " (C++ Reference)\n\tVariable type: " << typeid(variable).name())
			return GRAB_SETTING_ERR_CPP_REFERENCE;
		}

		else
		{
			variable = std::any_cast<T>(set_value);
			return 0;
		}
	}

	else if(setting_type == EXTERNAL_REFERENCE)
	{
		if(set_value.type() != typeid(variable))
		{
			PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << set_value.type().name() << " (External Reference)\n\tVariable type: " << typeid(variable).name())
			return GRAB_SETTING_ERR_EXTERNAL_REFERENCE;
		}

		else
		{
			variable = std::any_cast<T>(set_value);
			return 0;
		}
	}

	PRINTERR("grabSetting called but none of the if/else statements returned! This shouldn't be possible, so if you see this error message, the real problem is probably not related to grabSetting (or there's a return missing somewhere in here...)")
	return 0;
}
#endif