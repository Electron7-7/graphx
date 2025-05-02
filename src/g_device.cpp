#include "g_device.hpp"
#include "g_theatre.hpp"
#include "t_settings.hpp"

using namespace graphx;

//-------
// Device
//-------
Device::Device(const std::string& my_name)
: parent_theatre(nullptr), settings(gSettings()), UID(-1), name(my_name)
{}

Device::Device(Theatre* my_parent_theatre, const int my_uid, const gSettings& my_settings)
: parent_theatre(my_parent_theatre), settings(my_settings), UID(my_uid), name("Untitled Device")
{ loadSettings(); }

Device::~Device() = default;

int Device::getUID() const
{ return UID; }

void Device::setUID(const int new_uid)
{ UID = (parent_theatre != nullptr) ? parent_theatre->changeDeviceUID(UID, new_uid) : new_uid; }

std::string Device::getName() const
{ return name; }

void Device::setName(const std::string& new_name)
{ name = new_name; }

gSettings Device::getSettings() const
{ return settings; }

void Device::setSettings(const gSettings& new_settings)
{ settings = new_settings; }

void Device::loadSettings()
{ settings.getSetting("Name", name); }