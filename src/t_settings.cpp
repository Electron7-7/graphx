#include "t_settings.hpp"
#include "graphx_interpreter_lookups.hpp"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <algorithm>
#include <memory>

//---------------------------
// gSetting & derived classes
//---------------------------

//-------------
// Constructors
//-------------

// gSetting
gSetting::gSetting(std::any my_setting)
: setting(my_setting)
{}

// Raw Data
gSettingRawData::gSettingRawData(const gRawData& my_setting)
: gSetting(my_setting)
{}

gSettingRawData::gSettingRawData()
: gSetting()
{}

// Small function so I can keep shit in the initializer list for gSettingCppReference
std::any safeSetCppReference(const std::string& key)
{
    if(!cpp_definitions.contains(key))
        return std::any();
    return cpp_definitions.at(key);
}

// C++ Reference
gSettingCppReference::gSettingCppReference(const std::string& my_setting)
: gSetting(safeSetCppReference(my_setting))
{}

gSettingCppReference::gSettingCppReference()
: gSetting()
{}

// Theatre Reference
gSettingTheatreReference::gSettingTheatreReference(std::shared_ptr<Actor> my_setting, const bool is_sandwich_bun)
: gSetting(my_setting), is_sandwich(is_sandwich_bun)
{}

gSettingTheatreReference::gSettingTheatreReference(std::shared_ptr<Device> my_setting, const bool is_sandwich_bun)
: gSetting(my_setting), is_sandwich(is_sandwich_bun)
{}

gSettingTheatreReference::gSettingTheatreReference()
: gSetting()
{}

// External Reference
gSettingExternalReference::gSettingExternalReference(const std::string& my_setting)
: gSetting(my_setting)
{}

gSettingExternalReference::gSettingExternalReference(const std::filesystem::path& my_setting)
: gSetting(std::string(my_setting))
{}

gSettingExternalReference::gSettingExternalReference()
: gSetting()
{}

//-------------------------------------------
// Raw Data Template Function Specializations
//-------------------------------------------
template<typename T>
int gSettingRawData::getSetting(T& variable) const
{
    if(!setting.has_value()) return 0;

    static_assert(
        std::is_arithmetic_v<T> ||
        std::is_same_v<T, glm::vec2> ||
        std::is_same_v<T, glm::vec3> ||
        std::is_same_v<T, glm::vec4> ||
        std::is_same_v<T, glm::quat>
    );

    if constexpr(!std::is_arithmetic_v<T>) // Assumes the vector is the same size (or lower) as the variable!
    {
        std::vector<std::string> raw_data_copy = std::any_cast<gRawData>(setting);
        for(int i = 0; i < raw_data_copy.size(); i++)
            variable[i] = stringToNumber<float>(raw_data_copy[i]);
    }

    else if constexpr(std::is_arithmetic_v<T>)
    {
        variable = stringToNumber<T>(std::any_cast<gRawData>(setting)[0]);
        return 0;
    }

    PRINTERR("in gSettingRawData::getSetting: type is expected to be arithmetic or GLM vector/quaternion!")
    return GRAB_SETTING_ERR_RAW_DATA;
}

template<>
int gSettingRawData::getSetting(bool& variable) const
{
    if(!setting.has_value()) return 0;

    std::string raw_data_lower = std::any_cast<gRawData>(setting)[0];
    std::transform(raw_data_lower.begin(), raw_data_lower.end(), raw_data_lower.begin(), [](unsigned char c)
    {
        return std::tolower(c);
    });

    variable = raw_data_lower.compare("false");
    return 0;
}

template<>
int gSettingRawData::getSetting(std::string& variable) const
{
    if(!setting.has_value()) return 0;

    std::string string_out = "";

    for(std::string setting_string : std::any_cast<gRawData>(setting))
        string_out.append(setting_string);

    variable = string_out;
    return 0;
}

//-----------------------------
// gSettings Function Overloads
//-----------------------------
void gSettings::addSetting(const std::string& new_setting_name, const gSettingRawData& new_setting)
{ raw_data[new_setting_name] = new_setting; }

void gSettings::addSetting(const std::string& new_setting_name, const gSettingCppReference& new_setting)
{ cpp_references[new_setting_name] = new_setting; }

void gSettings::addSetting(const std::string& new_setting_name, const gSettingTheatreReference& new_setting)
{ theatre_references[new_setting_name] = new_setting; }

void gSettings::addSetting(const std::string& new_setting_name, const gSettingExternalReference& new_setting)
{ external_references[new_setting_name] = new_setting; }