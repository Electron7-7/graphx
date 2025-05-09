#include "g_device.hpp"
#include "g_theatre.hpp"
#include "t_settings.hpp"

using namespace graphx;

// DEBUG
void Device::debug_GetSettingsPrintout() const
{
    PRINT("Device \"" << name << "\" Settings:")
    for(auto pair : settings.debug_all_settings_printout)
        PRINT("Setting Name: " << pair.first << " | Setting Value Representation: " << pair.second)
}

//-------
// Device
//-------
Device::Device(const std::string& my_name)
:  name(my_name), parent_theatre(nullptr), settings(gSettings()), device_uid(-1)
{}

Device::Device(Theatre* my_parent_theatre, const int my_uid, const gSettings& my_settings)
: name("Untitled Device"), parent_theatre(my_parent_theatre), settings(my_settings), device_uid(my_uid)
{}

Device::~Device() = default;

int Device::getUID() const
{ return device_uid; }

void Device::setUID(const int new_uid)
{ device_uid = (parent_theatre != nullptr) ? parent_theatre->changeDeviceUID(device_uid, new_uid) : new_uid; }

gSettings Device::getSettings() const
{ return settings; }

void Device::setSettings(const gSettings& new_settings)
{ settings = new_settings; }

void Device::loadSettings()
{ configureBaseVariables(this); }