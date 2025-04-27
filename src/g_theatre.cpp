#include "g_theatre.hpp"
#include "sanity_printouts.hpp"
#include "g_actor.hpp"
#include "g_device.hpp"
#include <set>
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
    for(auto& pair : wrapped_actors)
        if(pair.second.owned_by_me)
            delete pair.second.pointer;

    for(auto& pair : wrapped_devices)
        if(pair.second.owned_by_me)
            delete pair.second.pointer;

    wrapped_actors.clear();
    wrapped_devices.clear();

    unwrapped_actors.clear();
    unwrapped_devices.clear();
}

int Theatre::getUID() const
{ return UID; }

void Theatre::probeRenderCommands() const
{

}


void Theatre::addActor(Actor* new_actor)
{
    if(wrapped_actors.contains(new_actor->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addActor", "an Actor", std::to_string(new_actor->getUID())))
        return;
    }

    parallelAddActor(new_actor, new_actor->getUID(), false);
}

void Theatre::addDevice(Device* new_device)
{
    if(wrapped_devices.contains(new_device->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addDevice", "a Device", std::to_string(new_device->getUID())))
        return;
    }

    parallelAddDevice(new_device, new_device->getUID(), false);
}

std::vector<Actor*> Theatre::getAllActors() const
{ return unwrapped_actors; }

std::vector<Device*> Theatre::getAllDevices() const
{ return unwrapped_devices; }

Actor* Theatre::getActor(const int UID) const
{
    if(wrapped_actors.contains(UID))
        return wrapped_actors.at(UID).pointer;
    PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getActor", "Actor", std::to_string(UID)))
    return &graphx::safety::actor;
}

Device* Theatre::getDevice(const int UID) const
{
    if(wrapped_devices.contains(UID))
        return wrapped_devices.at(UID).pointer;
    PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getDevice", "Device", std::to_string(UID)))
    return &graphx::safety::device;
}

// Private functions
void Theatre::addInterpretedActor(Actor* new_actor)
{
    if(wrapped_actors.contains(new_actor->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addInterpretedActor", "an Actor", std::to_string(new_actor->getUID())))
        return;
    }

    parallelAddActor(new_actor, new_actor->getUID(), true);
}

void Theatre::addInterpretedDevice(Device* new_device)
{
    if(wrapped_devices.contains(new_device->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addInterpretedDevice", "a Device", std::to_string(new_device->getUID())))
        return;
    }

    parallelAddDevice(new_device, new_device->getUID(), true);
}

// These functions make sure that the maps and vectors are kept parallel; since these are private
// utility functions, it is assumed that all safety checks have already been made. These functions
// are one to two steps away from just being a #define macro for std::vector::insert and std::map[]
// so treat them as such and MAKE YOUR SAFETY CHECKS BEFORE CALLING THEM! To avoid undefined behaviour,
// these functions will ALWAYS put their pointer argument into their respective map & vector.
void Theatre::parallelAddActor(Actor* pointer, const int UID, const bool ownership)
{
    wrapped_actors[UID] = ActorPointerWrapper(pointer, ownership);
    unwrapped_actors.insert(unwrapped_actors.end(), pointer);

    checkAndManageParallelActorDesync();
}

void Theatre::parallelAddDevice(Device* pointer, const int UID, const bool ownership)
{
    wrapped_devices[UID] = DevicePointerWrapper(pointer, ownership);
    unwrapped_devices.insert(unwrapped_devices.end(), pointer);

    checkAndManageParallelActorDesync();
}

void Theatre::parallelRemoveActor(const int UID)
{}

void Theatre::parallelRemoveDevice(const int UID)
{}

void Theatre::checkAndManageParallelActorDesync()
{
    if(wrapped_actors.size() == unwrapped_actors.size())
        return;

    PRINTERR(THEATRE_ERR_PARALLEL_DESYNC("Theatre::parallelAddActor(Actor*, const bool)", "Actor"))

    std::set<int> synced_uids;

    if(wrapped_actors.size() > unwrapped_actors.size())
    {
        for(Actor* pointer : unwrapped_actors)
            synced_uids.insert(pointer->getUID());

        for(auto& pair : wrapped_actors)
        {
            if(!synced_uids.contains(pair.first))
            {
                PRINTERR(THEATRE_ERR_DESYNC_DETECTION("Actor", "std::map<int, ActorPointerWrapper> Theatre::wrapped_actors", pair.first))
                if(pair.second.owned_by_me)
                    delete pair.second.pointer;
                wrapped_actors.erase(pair.first);
                return;
            }
        }
    }

    for(auto& pair : wrapped_actors)
        synced_uids.insert(pair.first);

    for(int i = 0; i < unwrapped_actors.size(); i++)
    {
        if(!synced_uids.contains(unwrapped_actors[i]->getUID()))
        {
            PRINTERR(THEATRE_ERR_DESYNC_DETECTION("Actor", "std::vector<Actor*> Theatre::unwrapped_actors", unwrapped_actors[i]->getUID()))
            delete unwrapped_actors[i];
            unwrapped_actors.erase(unwrapped_actors.begin() + i);
            return;
        }
    }

    PRINTERR("Theatre::checkAndManageParallelActorDesync() - Desync detected, but not rectified!")
}

void Theatre::checkAndManageParallelDeviceDesync()
{
    if(wrapped_devices.size() == unwrapped_devices.size())
        return;

    PRINTERR(THEATRE_ERR_PARALLEL_DESYNC("Theatre::parallelAddDevice(Device*, const bool)", "Device"))

    std::set<int> synced_uids;

    if(wrapped_devices.size() > unwrapped_devices.size())
    {
        for(Device* pointer : unwrapped_devices)
            synced_uids.insert(pointer->getUID());

        for(auto& pair : wrapped_devices)
        {
            if(!synced_uids.contains(pair.first))
            {
                PRINTERR(THEATRE_ERR_DESYNC_DETECTION("Device", "std::map<int, DevicePointerWrapper> Theatre::wrapped_devices", pair.first))
                if(pair.second.owned_by_me)
                    delete pair.second.pointer;
                wrapped_devices.erase(pair.first);
                return;
            }
        }
    }

    for(auto& pair : wrapped_devices)
        synced_uids.insert(pair.first);

    for(int i = 0; i < unwrapped_devices.size(); i++)
    {
        if(!synced_uids.contains(unwrapped_devices[i]->getUID()))
        {
            PRINTERR(THEATRE_ERR_DESYNC_DETECTION("Device", "std::vector<Device*> Theatre::unwrapped_devices", unwrapped_devices[i]->getUID()))
            delete unwrapped_devices[i];
            unwrapped_devices.erase(unwrapped_devices.begin() + i);
            return;
        }
    }

    PRINTERR("Theatre::checkAndManageParallelDeviceDesync() - Desync detected, but not rectified!")
}