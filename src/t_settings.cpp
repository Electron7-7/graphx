#include "t_settings.hpp"
#include "g_device.hpp"
#include "g_actor.hpp"

//-------------
// Constructors
//-------------
gSetting::gSetting(std::any my_setting)
: setting(my_setting)
{}

gSettingRawData::gSettingRawData(const gRawData& my_setting)
: gSetting(my_setting)
{}

gSettingCppReference::gSettingCppReference(const std::string& my_setting)
: gSetting(my_setting)
{}

gSettingTheatreReference::gSettingTheatreReference(std::shared_ptr<Actor> my_setting, const bool is_sandwich_bun)
: gSetting(my_setting), is_sandwich(is_sandwich_bun)
{}

gSettingTheatreReference::gSettingTheatreReference(std::shared_ptr<Device> my_setting, const bool is_sandwich_bun)
: gSetting(my_setting), is_sandwich(is_sandwich_bun)
{}

gSettingExternalReference::gSettingExternalReference(const std::string& my_setting)
: gSetting(my_setting)
{}

gSettingExternalReference::gSettingExternalReference(const std::filesystem::path& my_setting)
: gSetting(my_setting)
{}

template<typename T> T doStringToNumber(const std::string& str)
{   // There's probably a better way to do this, but fuck it idc
    const T test_if_whole;

    if constexpr(static_cast<int>(test_if_whole) || static_cast<long>(test_if_whole))
    {
        long valid_number = 0;

        try
        {
            valid_number = std::stol(str);
        }

        catch(std::invalid_argument const& exception)
        {
            PRINTERR("in gSetting::doStringToNumber at std::stol: std::invalid_argument " << exception.what())
            valid_number = 0; // Just to be safe...
        }

        return valid_number;
    }

    else
    {
        double valid_number = 0.0f;

        try
        {
            valid_number = std::stod(str);
        }

        catch(std::invalid_argument const& exception)
        {
            PRINTERR("in gSetting::doStringToNumber at std::stod: std::invalid_argument " << exception.what())
            valid_number = 0.0f; // Just to be safe...
        }

        return valid_number;
    }

}

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
            variable[i] = doStringToNumber<float>(raw_data_copy[i]);
    }

    else if constexpr(std::is_arithmetic_v<T>)
    {
        variable = doStringToNumber<T>(std::any_cast<gRawData>(setting)[0]);
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
}

template<>
int gSettingRawData::getSetting(std::string& variable) const
{
    if(!setting.has_value()) return 0;

    std::string string_out = "";

    for(std::string setting_string : std::any_cast<gRawData>(setting))
        string_out.append(setting_string);

    variable = string_out;
}

template<typename T>
int gSettingCppReference::getSetting(T& variable) const
{
    if(!setting.has_value()) return 0;

    if constexpr(typeid(T) == setting.type())
    {
        variable = std::any_cast<T>(setting);
        return 0;
    }

    PRINTERR("in gSettingCppReference::getSetting: type does not match setting type!")
    return GRAB_SETTING_ERR_CPP_REFERENCE;
}

template<typename T> 
int gSettingTheatreReference::getSetting(std::shared_ptr<T>& variable) const
{
    if(!setting.has_value()) return 0;

    static_assert(std::is_base_of_v<Actor, T> || std::is_base_of_v<Device, T>);

    if constexpr(std::is_base_of_v<Actor, T>)
    {
        variable = std::dynamic_pointer_cast<T>(std::any_cast<std::shared_ptr<Actor>>(setting));
        return 0;
    }

    else if constexpr(std::is_base_of_v<Device, T>)
    {
        variable = std::dynamic_pointer_cast<T>(std::any_cast<std::shared_ptr<Device>>(setting));
        return 0;
    }

    PRINTERR("in gSettingTheatreReference::getSetting: type is not derived from Actor or Device!")
    return GRAB_SETTING_ERR_ACTOR_POINTER;
}

template<typename T>
int gSettingExternalReference::getSetting(T& variable) const
{
    if(!setting.has_value()) return 0;

    if(setting.type() == typeid(std::string))
    {
        variable = std::any_cast<std::string>(setting);
        return 0;
    }

    else if(setting.type() == typeid(std::filesystem::path))
    {
        variable = std::any_cast<std::filesystem::path>(setting);
        return 0;
    }

    PRINTERR("in gSettingExternalReference::getSetting: setting was not of type std::string or std::filesystem::path")
    return GRAB_SETTING_ERR_EXTERNAL_REFERENCE;
}