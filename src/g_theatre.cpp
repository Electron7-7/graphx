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
Theatre::Theatre(const graphx::gID init_id)
: name_and_uid(init_id)
{}

Theatre::Theatre(const int init_id, const std::string init_name)
: name_and_uid(init_id, init_name)
{}

Theatre::~Theatre()
{
    for(auto& pair : actors)
        delete pair.second;
    actors.clear();

    for(auto& pair : devices)
        delete pair.second;
    devices.clear();
}

void Theatre::createActorOrDevice(const graphx::gClass& type, const graphx::gID& id, const graphx::gSettings& settings)
{
    if(graphx::classes::getBaseType(type) == graphx::classes::ACTOR)
    {
        if(actors.contains(id))
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
}

void Theatre::addActor(Actor* new_actor)
{
    if(actors.contains(new_actor->getID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addActor", "an Actor", new_actor->getID().toString()))
        return;
    }

    actors[new_actor->getID()] = new_actor;
}

void Theatre::addDevice(Device* new_device)
{
    if(devices.contains(new_device->getID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addDevice", "a Device", new_device->getID().toString()))
        return;
    }

    devices[new_device->getID()] = new_device;
}

Actor* Theatre::getActor(const graphx::gID& id) const
{
    if(actors.contains(id))
        return actors.at(id);
    PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getActor", "Actor", id.toString()))
    return &graphx::safety::actor;
}

Device* Theatre::getDevice(const graphx::gID& id) const
{
    if(devices.contains(id))
        return devices.at(id);
    PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getDevice", "Device", id.toString()))
    return &graphx::safety::device;
}