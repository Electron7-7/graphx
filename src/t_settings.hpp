#ifndef GRAPHX_SETTINGS
#define GRAPHX_SETTINGS
#include "sanity.hpp"
#include "graphx_namespace.hpp"
#include "t_common.hpp"
#include <algorithm>
#include <Jolt/Jolt.h>

// Forward Declarations
class Actor;
struct Device;

extern std::string empty_settings_identifier;
extern graphx::gSettings empty_settings;

#define GRAB_SETTING_ERR_BOTH_POINTER    -420
#define GRAB_SETTING_ERR_DEVICE_POINTER  -4
#define GRAB_SETTING_ERR_ACTOR_POINTER   -8
#define GRAB_SETTING_ERR_RAW_DATA_SINGLE -15
#define GRAB_SETTING_ERR_RAW_DATA_TWO    -16
#define GRAB_SETTING_ERR_RAW_DATA_THREE  -23
#define GRAB_SETTING_ERR_RAW_DATA_FOUR   -42
#define GRAB_SETTING_ERR_CPP_REFERENCE   -69

#define GRAB_SETTING_ERR_MSG_TEMPLATE std::string("getSetting called with non-matching variable and setting!")

template<typename T> int getSetting(T &variable, graphx::gSetting setting)
{
	if(!setting.second.has_value() || setting.second.type() == typeid(void) || setting.second.type() == typeid(nullptr))
	{
		return 0;
	}

	else if(setting.first == RAW_DATA)
	{
		graphx::gRawData raw_data = std::any_cast<graphx::gRawData>(setting.second);
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

			else if constexpr(std::is_arithmetic_v<T>) // Should only fire if T is number, since the previous if will fire when T is a bool
			{
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
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << setting.second.type().name() << " (gRawData)\n\tVariable type: " << typeid(variable).name())
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
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << setting.second.type().name() << " (gRawData)\n\tVariable type: " << typeid(variable).name())
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
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << setting.second.type().name() << " (gRawData)\n\tVariable type: " << typeid(variable).name())
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
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << setting.second.type().name() << " (gRawData)\n\tVariable type: " << typeid(variable).name())
				return GRAB_SETTING_ERR_RAW_DATA_FOUR;
			}
			break;
		}
	}

	else if(setting.first == THEATRE_REFERENCE)
	{
		if constexpr(std::is_base_of_v<Device, std::remove_pointer_t<T>>)
		{
			if(setting.second.type() != typeid(Device *))
			{
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << setting.second.type().name() << " (Theatre Reference or Sandwich)\n\tVariable type: " << typeid(variable).name())
				return GRAB_SETTING_ERR_DEVICE_POINTER;
			}

			variable = static_cast<T>(std::any_cast<Device *>(setting.second));
			return 0;
		}

		else if constexpr(std::is_base_of_v<Actor, std::remove_pointer_t<T>>)
		{
			if(setting.second.type() != typeid(Actor *))
			{
				PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << setting.second.type().name() << " (Theatre Reference or Sandwich)\n\tVariable type: " << typeid(variable).name())
				return GRAB_SETTING_ERR_ACTOR_POINTER;
			}

			variable = static_cast<T>(std::any_cast<Actor *>(setting.second));
			return 0;
		}

		else
		{
			PRINTERR("getSetting called with a Theatre reference setting (Actor/Device pointer) but non-matching variable (variable supplied is not an Actor/Device or subclass of Actor/Device)!")
			return GRAB_SETTING_ERR_BOTH_POINTER;
		}
	}

	else if(setting.first == CPP_REFERENCE)
	{
		if(setting.second.type() != typeid(variable))
		{
			PRINTERR(GRAB_SETTING_ERR_MSG_TEMPLATE << "\n\tSetting type: " << setting.second.type().name() << " (C++ Reference)\n\tVariable type: " << typeid(variable).name())
			return GRAB_SETTING_ERR_CPP_REFERENCE;
		}

		else
		{
			variable = std::any_cast<T>(setting.second);
			return 0;
		}
	}

	PRINTERR("grabSetting called but none of the if/else statements returned! This shouldn't be possible, so if you see this error message, the real problem is probably not related to grabSetting (or my code is just very very VERY bad, which is always a possibility)")
	return -1;
}
#endif