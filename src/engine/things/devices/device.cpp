#include "device.hpp"

//
// Device
//
Device::Device(const std::string& my_name)
: name(my_name), parent_theatre(nullptr), settings(gSettings()), device_uid(-1)
{}

Device::Device(Theatre* my_parent_theatre, const int my_uid, const gSettings& my_settings)
: name("Untitled Device"), parent_theatre(my_parent_theatre), settings(my_settings), device_uid(my_uid)
{}

Device::~Device() = default;

void Device::loadSettings()
{
    gSettings::configureBaseVariables(this);
}

void Device::initialize()
{}

void Device::prepForDestruction()
{
    if(ready_to_destroy)
        return;
    ready_to_destroy = true;
}

gSettings Device::getSettings() const
{
    return settings;
}

void Device::setSettings(const gSettings& new_settings)
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