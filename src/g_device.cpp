#include "g_device.hpp"
#include "t_settings.hpp"
//-------
// Device
//-------
Device::Device(const graphx::gClass& my_type, const graphx::gID& my_id, const graphx::gSettings& my_settings)
: type(my_type), name_and_uid(my_id), settings(my_settings)
{}

Device::~Device()
{}

void Device::setUID(const int new_uid)
{ name_and_uid.uid = new_uid; }

void Device::setName(const std::string& new_name)
{ name_and_uid.name = new_name; }

graphx::gID Device::getID() const
{ return name_and_uid; }

graphx::gSettings Device::getSettings() const
{ return settings; }

void Device::setSettings(const graphx::gSettings& new_settings)
{ settings = new_settings; }

void Device::loadSettings()
{
    getSetting(name_and_uid.name, settings["Name"]);
}