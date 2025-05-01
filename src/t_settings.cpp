#include "t_settings.hpp"
#include "g_device.hpp"
#include "g_actor.hpp"

gSetting::gSetting()
: category(Category::UNDEFINED)
{}

gSetting::gSetting(const gRawData& raw_data_setting, const Category new_category)
: category(new_category), setting(raw_data_setting)
{}

gSetting::gSetting(std::any cpp_reference_setting, const bool setting_is_cpp_reference, const Category new_category)
: category(new_category), setting(cpp_reference_setting)
{}

gSetting::gSetting(std::shared_ptr<Actor> actor_reference_setting, const Category new_category)
: category(new_category), setting(actor_reference_setting)
{}

gSetting::gSetting(std::shared_ptr<Device> device_reference_setting, const Category new_category)
: category(new_category), setting(device_reference_setting)
{}

gSetting::gSetting(const std::string& external_reference_setting, const Category new_category)
: category(new_category), setting(external_reference_setting)
{}

double tryConvertStringToNumber(const std::string& str, const bool use_double_instead)
{
    if(use_double_instead)
    {
        double valid_double = 0.0;

        try
        {
            valid_double = std::stod(str);
        }

        catch(std::invalid_argument const& exception)
        {
            PRINTERR("in gSetting::getRawDataGLM at std::stod: std::invalid_argument " << exception.what())
            valid_double = 0.0; // Just to be safe...
        }

        return valid_double;
    }

    float valid_float = 0.0f;

    try
    {
        valid_float = std::stof(str);
    }

    catch(std::invalid_argument const& exception)
    {
        PRINTERR("in gSetting::getRawDataGLM at std::stof: std::invalid_argument: " << exception.what())
        valid_float = 0.0f; // Just to be safe...
    }

    return valid_float;
}


// gSetting::getRawData - Specializations

template<> int getRawData(bool& variable, const gSetting& setting)
{
    gRawData raw_data = std::any_cast<gRawData>(setting.setting);
    // Converting raw_data to all lowercase
    std::string raw_data_lower = raw_data[0];
    std::transform(raw_data_lower.begin(), raw_data_lower.end(), raw_data_lower.begin(), [](unsigned char c)
    { return std::tolower(c); });
    // Potential shorter way of doing this (only try after everything's working):
    // for(char& character : raw_data_lower)
        // character = std::tolower(character);

    variable = raw_data_lower.compare("false");
    return 0;
}

template<> int getRawData(std::string& variable, const gSetting& setting)
{
    gRawData raw_data = std::any_cast<gRawData>(setting.setting);
    std::string raw_data_out = "";
    for(int i = 0; i < raw_data.size(); i++)
        raw_data_out.append(raw_data[i].c_str());
    variable = raw_data_out;
    return 0;
}

// This one's a bit iffy...
template<> int getRawData(const char*& variable, const gSetting& setting)
{
    gRawData raw_data = std::any_cast<gRawData>(setting.setting);
    std::string raw_data_out = "";
    for(int i = 0; i < raw_data.size(); i++)
        raw_data_out.append(raw_data[i].c_str());
    variable = raw_data_out.c_str();
    return 0;
}

// idk when, where, or why you'd need this but here you go i guess...
template<> int getRawData(char& variable, const gSetting& setting)
{
    gRawData raw_data = std::any_cast<gRawData>(setting.setting);
    variable = (!raw_data.empty()) ? raw_data[0][0] : '0'; // just being cautious...
    return 0;
}