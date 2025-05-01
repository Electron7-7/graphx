#ifndef GRAPHX_SETTINGS
#include "sanity_printouts.hpp"
#include <any>
#include <type_traits>
#include <memory>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <filesystem>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#define GRAPHX_SETTINGS

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
class Actor;
struct Device;
#endif

#define DEFAULT_VALUE_SETTING 99

#define GRAB_SETTING_ERR_DEVICE_POINTER     -4
#define GRAB_SETTING_ERR_ACTOR_POINTER      -8
#define GRAB_SETTING_ERR_RAW_DATA           -15
#define GRAB_SETTING_ERR_CPP_REFERENCE      -16
#define GRAB_SETTING_ERR_SANDWICH_BUN       -23
#define GRAB_SETTING_ERR_EXTERNAL_REFERENCE -42

#define GRAB_SETTING_ERR_MSG_TEMPLATE std::string("getSetting called with non-matching variable and setting!")

typedef std::string                              gKey;
typedef std::pair<int, std::string>              gValue;
typedef std::pair<gKey, gValue>                  gStringSetting;
typedef std::vector<std::vector<gStringSetting>> gStringSettings;
typedef std::vector<std::string>                 gRawData;

struct gSetting
{
	enum class Category
	{
		UNDEFINED          = 0,
		RAW_DATA           = 1,
		CPP_REFERENCE      = 2,
		EXTERNAL_REFERENCE = 3,
		THEATRE_REFERENCE  = 4,
		SANDWICH           = 5
	};

	Category category = Category::UNDEFINED;
	std::any setting;

	gSetting();
	gSetting(std::any CppReference, const bool SettingIsCppReference, const Category CategoryOverride = Category::CPP_REFERENCE);
	gSetting(const gRawData& RawData, const Category CategoryOverride = Category::RAW_DATA);
	gSetting(std::shared_ptr<Actor> TheatreReference, const Category TheatreReferenceOrSandwichBun);
	gSetting(std::shared_ptr<Device> TheatreReference, const Category TheatreReferenceOrSandwichBun);
	gSetting(const std::string& ExternalReference, const Category CategoryOverride = Category::EXTERNAL_REFERENCE);
};

template<typename T> struct IsRawData : std::disjunction<std::is_arithmetic<T>, std::is_same<T, bool>, std::is_same<T, std::string>, std::is_same<T, glm::vec2>, std::is_same<T, glm::vec3>, std::is_same<T, glm::vec4>, std::is_same<T, glm::quat>> {};
template<typename T> struct IsTheatreReference : std::false_type {};
template<typename T> struct IsTheatreReference<std::shared_ptr<T>> : std::disjunction<std::is_base_of<Actor, T>, std::is_base_of<Device, T>> {};

double tryConvertStringToNumber(const std::string&, const bool);

// Default specialization: arithmatic scalar or glm vector/quaternion
template<typename T> int getRawData(T& variable, const gSetting& setting)
{
    if constexpr(IsRawData<T>::value)
    {
    	gRawData raw_data = std::any_cast<gRawData>(setting.setting);

        if constexpr(std::is_arithmetic_v<T>)
        { variable = tryConvertStringToNumber(raw_data[0], true); return 0; } // Get double instead of float to avoid errors (that's the idea, at least)

        else if constexpr(std::is_same_v<T, glm::vec2>)
        { getRawDataGLM(variable, 2); return 0; }

        else if constexpr(std::is_same_v<T, glm::vec3>)
        { getRawDataGLM(variable, 3); return 0; }

        else if constexpr(std::is_same_v<T, glm::vec4> || std::is_same_v<T, glm::quat>)
        { getRawDataGLM(variable, 4); return 0; }

        PRINTERR("in gSetting::getRawData: unknown error occured! (default specialization, end of if-else statements)")
        return GRAB_SETTING_ERR_RAW_DATA;
    }

    PRINTERR("gSetting::getRawData(T& variable) - T is not a valid type!")
    return GRAB_SETTING_ERR_RAW_DATA;
}

template<typename T> int getTheatreReference(T& variable, const gSetting& setting)
#endif