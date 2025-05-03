#include "t_settings.hpp"
#include "graphx_interpreter_lookups.hpp"

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