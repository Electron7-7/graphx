#include "g_device.hpp"
#include "g_theatre.hpp"
#include "t_settings.hpp"

using namespace graphx;

//-------
// Device
//-------
Device::Device(const std::string& my_name)
:  name(my_name), parent_theatre(nullptr), settings(gSettings()), UID(-1)
{}

Device::Device(Theatre* my_parent_theatre, const int my_uid, const gSettings& my_settings)
: name("Untitled Device"), parent_theatre(my_parent_theatre), settings(my_settings), UID(my_uid)
{}

Device::~Device() = default;

int Device::getUID() const
{ return UID; }

void Device::setUID(const int new_uid)
{ UID = (parent_theatre != nullptr) ? parent_theatre->changeDeviceUID(UID, new_uid) : new_uid; }

gSettings Device::getSettings() const
{ return settings; }

void Device::setSettings(const gSettings& new_settings)
{ settings = new_settings; }

void Device::loadSettings()
{ configureBaseVariables(this); }