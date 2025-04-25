#include "g_theatre.hpp"
#include "graphx_classes_namespace.hpp"
#include "g_actor.hpp"
#include "r_common.hpp"
//------------
// LightsCount
//------------
LightsCount::LightsCount(const unsigned int point_lights_count, const unsigned int spot_lights_count, const unsigned int directional_lights_count)
: point_lights(point_lights_count), spot_lights(spot_lights_count), directional_lights(directional_lights_count)
{}

//--------
// Theatre
//--------
Theatre::Theatre(const graphx::gUID init_uid)
: uid(init_uid)
{}

Theatre::Theatre(const int init_id, const std::string init_name)
: uid(init_id, init_name)
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

void Theatre::createActorOrDevice(const graphx::gClass* type, const graphx::gUID& uid, const graphx::gSettings& settings)
{
    if(graphx::classes::getBaseType(*type) == graphx::classes::ACTOR)
    {
        if(actors.contains(uid))
        {
            PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::createActorOrDevice", "an Actor", uid.toString()))
            return;
        }

        actors[uid] = type->new_actor(uid, settings);
        return;
    }

    else if(graphx::classes::getBaseType(*type) == graphx::classes::DEVICE)
    {
        if(devices.contains(uid))
        {
            PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::createActorOrDevice", "an Actor", uid.toString()))
            return;
        }

        devices[uid] = type->new_device(uid, settings);
        return;
    }

    PRINTERR(THEATRE_ERR_WRONG_BASE_TYPE("Theatre::createActorOrDevice", "ACTOR\" or \"DEVICE", type->name))
}

void Theatre::addActor(Actor* new_actor)
{
    if(actors.contains(new_actor->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addActor", "an Actor", new_actor->getUID().toString()))
        return;
    }

    actors[new_actor->getUID()] = new_actor;
}

void Theatre::addDevice(Device* new_device)
{
    if(devices.contains(new_device->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addDevice", "a Device", new_device->getUID().toString()))
        return;
    }

    devices[new_device->getUID()] = new_device;
}

Actor* Theatre::getActor(const graphx::gUID& uid) const
{
    if(actors.contains(uid))
        return actors.at(uid);
    PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getActor", "Actor", uid))
    return new Actor();
}

Device* Theatre::getDevice(const graphx::gUID& uid) const
{
    if(devices.contains(uid))
        return devices.at(uid);
    PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getDevice", "Device", uid))
    return new Device();
}