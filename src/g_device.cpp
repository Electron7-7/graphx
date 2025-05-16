#include "g_device.hpp"

//
// Device
//
Device::Device(const std::string& my_name)
: name(my_name), parent_theatre(nullptr), settings(empty_settings), device_uid(-1)
{}

Device::Device(Theatre* my_parent_theatre, const int my_uid, const graphx::gSettings& my_settings)
: name("Untitled Device"), parent_theatre(my_parent_theatre), settings(my_settings), device_uid(my_uid)
{}

Device::~Device() = default;

void Device::loadSettings(graphx::gSettings new_settings)
{
    if(settings.contains(empty_settings_identifier))
        settings = new_settings;
    if(new_settings.contains(empty_settings_identifier))
        new_settings = settings;

    getSetting(name, new_settings["Name"]);
}

void Device::initialize()
{}

void Device::prepForDestruction()
{
    if(ready_to_destroy)
        return;
    ready_to_destroy = true;
}

graphx::gSettings Device::getSettings() const
{
    return settings;
}

void Device::setSettings(const graphx::gSettings& new_settings)
{
    settings = new_settings;
}

void Device::setUID(int new_uid)
{
    device_uid = new_uid;
}

int Device::getUID() const
{
    return device_uid;
}