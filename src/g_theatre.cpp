#include "g_theatre.hpp"
#include "graphx_classes.hpp"
#include "sanity_printouts.hpp"
#include "g_actor.hpp"
#include "g_device.hpp"
//------------
// LightsCount
//------------
LightsCount::LightsCount(const unsigned int point_lights_count, const unsigned int spot_lights_count, const unsigned int directional_lights_count)
: point_lights(point_lights_count), spot_lights(spot_lights_count), directional_lights(directional_lights_count)
{}

//--------
// Theatre
//--------
Theatre::Theatre(const std::string& new_name)
: name(new_name), UID(-1)
{}

Theatre::Theatre(const int new_uid, const std::string& new_name)
: name(new_name), UID(new_uid)
{}

Theatre::~Theatre()
{
    for(auto& pair : actors)
        delete pair.second;

    for(auto& pair : devices)
        delete pair.second;

    actors.clear();
    devices.clear();
}

int Theatre::getUID() const
{ return UID; }

/*void Theatre::createActorOrDevice(const graphx::gClass& type, graphx::gID id, const graphx::gSettings& settings)
{
    if(graphx::classes::getBaseType(type) == graphx::classes::ACTOR)
    {
        if(actors.contains(&id))
        {
            PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::createActorOrDevice", "an Actor", id.toString()))
            return;
        }

        actors[id] = type.new_actor(id, settings);
        return;
    }

    else if(graphx::classes::getBaseType(type) == graphx::classes::DEVICE)
    {
        if(devices.contains(id))
        {
            PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::createActorOrDevice", "an Actor", id.toString()))
            return;
        }

        devices[id] = type.new_device(id, settings);
        return;
    }

    PRINTERR(THEATRE_ERR_WRONG_BASE_TYPE("Theatre::createActorOrDevice", "ACTOR\" or \"DEVICE", type.name))
}*/

void Theatre::addActor(Actor* new_actor)
{
    if(actors.contains(new_actor->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addActor", "an Actor", std::to_string(new_actor->getUID())))
        return;
    }

    actors[new_actor->getUID()] = new_actor;
}

void Theatre::addDevice(Device* new_device)
{
    if(devices.contains(new_device->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addDevice", "a Device", std::to_string(new_device->getUID())))
        return;
    }

    devices[new_device->getUID()] = new_device;
}

Actor* Theatre::getActor(const int UID) const
{
    if(actors.contains(UID))
        return actors.at(UID);
    PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getActor", "Actor", std::to_string(UID)))
    return &graphx::safety::actor;
}

Device* Theatre::getDevice(const int UID) const
{
    if(devices.contains(UID))
        return devices.at(UID);
    PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getDevice", "Device", std::to_string(UID)))
    return &graphx::safety::device;
}