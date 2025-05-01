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
public:
	gSetting(std::any = std::any());

protected:
	std::any setting;
};

struct gSettingRawData: public gSetting
{
	gSettingRawData(const gRawData&);

	template<typename T> int getSetting(T& variable) const;
};

struct gSettingCppReference: public gSetting
{
	gSettingCppReference(const std::string&); // instead of the value, store the map key

	template<typename T> int getSetting(T& variable) const;
};

struct gSettingTheatreReference: public gSetting
{
	gSettingTheatreReference(std::shared_ptr<Actor>, const bool = false);
	gSettingTheatreReference(std::shared_ptr<Device>, const bool = false);

	template<typename T> int getSetting(std::shared_ptr<T>& variable) const;

private:
	bool is_sandwich = false; // Probably not needed
};

struct gSettingExternalReference: public gSetting
{
	gSettingExternalReference(const std::string&);
	gSettingExternalReference(const std::filesystem::path&);

	template<typename T> int getSetting(T& variable) const;
};
#endif