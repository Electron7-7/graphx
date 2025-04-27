#include "g_device.hpp"
#include "g_theatre.hpp"
#include "t_settings.hpp"

using namespace graphx;

//---------------------
// DevicePointerWrapper
//---------------------
DevicePointerWrapper::DevicePointerWrapper(Device* new_pointer, const bool ownership)
: pointer(new_pointer), owned_by_me(ownership)
{}

//-------
// Device
//-------
Device::Device(const std::string& my_name)
: name(my_name), settings(gSettings()), UID(-1), parent_theatre(nullptr)
{}

Device::Device(Theatre* my_parent_theatre, const gSettings& my_settings)
: settings(my_settings), parent_theatre(my_parent_theatre)
{}

Device::~Device()
{}

int Device::getUID() const
{ return UID; }

void Device::setUID(const int new_uid)
{ UID = (parent_theatre != nullptr) ? parent_theatre->changeDeviceUID(UID, new_uid) : new_uid; }

gSettings Device::getSettings() const
{ return settings; }

void Device::setSettings(const gSettings& new_settings)
{ settings = new_settings; }

void Device::loadSettings()
{
    getSetting(name, settings["Name"]);
}