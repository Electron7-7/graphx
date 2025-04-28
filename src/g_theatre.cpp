#include "g_theatre.hpp"
#include "sanity_printouts.hpp"
#include "g_actors.hpp"
#include "g_device.hpp"
#include "r_rendering.hpp"
#include <set>

//--------------------
// ActorPointerWrapper
//--------------------
ActorPointerWrapper::ActorPointerWrapper(Actor* new_pointer, const bool ownership)
: pointer(new_pointer), owned_by_me(ownership)
{}

//---------------------
// DevicePointerWrapper
//---------------------
DevicePointerWrapper::DevicePointerWrapper(Device* new_pointer, const bool ownership)
: pointer(new_pointer), owned_by_me(ownership)
{}

//------------
// LightsCount
//------------
LightsCount::LightsCount(const unsigned int point_lights_count, const unsigned int spot_lights_count, const unsigned int directional_lights_count)
: point_lights(point_lights_count), spot_lights(spot_lights_count), directional_lights(directional_lights_count)
{}

std::random_device Theatre::uid_random_device;
std::mt19937 Theatre::uid_random_generator(uid_random_device());

//--------
// Theatre
//--------
Theatre::Theatre(const int new_uid, const std::string& new_name)
: name(new_name), UID(new_uid)
{}

Theatre::Theatre(const std::string& new_name)
: Theatre(-1, new_name)
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

// Terrible, no good, very bad functions
std::set<std::string> Theatre::getMeshDataNames()
{
    std::set<std::string> mesh_data_names = {ERROR_MODEL};

    for(int i = 0; i < unwrapped_devices.size(); i++)
        if(Model* model = dynamic_cast<Model*>(unwrapped_devices[i]))
            mesh_data_names.insert(model->mesh_data_name);

    return mesh_data_names;
}

std::set<std::string> Theatre::getTextureNames()
{
    std::set<std::string> texture_names;

    for(int i = 0; i < unwrapped_devices.size(); i++)
        if(Material* material = dynamic_cast<Material*>(unwrapped_devices[i]))
        {
            texture_names.insert(material->diffuse_texture_name);
            texture_names.insert(material->specular_texture_name);
        }

    texture_names.insert(MISSING_TEXTURE);
// #ifdef GRAPHX_DEBUG // I want light debug meshes to be visible even on the Production builds for now
    texture_names.insert(LIGHT_DEBUGGING);
// #endif
    return texture_names;
}
// Terrible, no good, very bad functions

int Theatre::getUID() const
{ return UID; }

void Theatre::probeRenderCommands()
{
    point_lights_count = 0;
    spot_lights_count = 0;
    directional_lights_count = 0;

    for(Actor* actor : unwrapped_actors)
    {
        if(graphx::state::loading_new_main_theatre)
            return;

        if(dynamic_cast<Light*>(actor))
        {
            if(dynamic_cast<LightSpot*>(actor))
            {
                spot_lights_count++;
                return;
            }

            else if(dynamic_cast<LightDirectional*>(actor))
            {
                directional_lights_count++;
                return;
            }

            point_lights_count++;
        }

        R_BufferRenderCommands(actor->getRenderCommands());
    }
}

void Theatre::addActor(Actor* new_actor)
{
    if(wrapped_actors.contains(new_actor->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addActor", "an Actor", new_actor->getUID()))
        PRINTNOTE("Changing Actor's UID before adding")
        new_actor->setUID(generateUID(true));
    }

    parallelAddActor(new_actor, new_actor->getUID(), false);
}

void Theatre::addDevice(Device* new_device)
{
    if(wrapped_devices.contains(new_device->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addDevice", "a Device", new_device->getUID()))
        PRINTNOTE("Changing Device's UID before adding")
        new_device->setUID(generateUID(true));
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
    PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getActor", "Actor", UID))
    return &graphx::safety::actor;
}

Device* Theatre::getDevice(const int UID) const
{
    if(wrapped_devices.contains(UID))
        return wrapped_devices.at(UID).pointer;
    PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getDevice", "Device", UID))
    return &graphx::safety::device;
}

// Private functions
int Theatre::generateUID(const bool for_actor)
{
    std::uniform_int_distribution<> uid_distribution(0);
    int new_uid = 0;

    if(for_actor)
        while(wrapped_actors.contains(new_uid))
            new_uid = uid_distribution(uid_random_generator);
    else
        while(wrapped_devices.contains(new_uid))
            new_uid = uid_distribution(uid_random_generator);

    return new_uid;
}

int Theatre::changeActorUID(const int old_uid, const int new_uid)
{
    if(wrapped_actors.contains(new_uid))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::changeActorUID", "Actor", old_uid))
        return old_uid;
    }

    auto actor_node = wrapped_actors.extract(old_uid);
    actor_node.key() = new_uid;
    wrapped_actors.insert(std::move(actor_node));
    return new_uid;
}

int Theatre::changeDeviceUID(const int old_uid, const int new_uid)
{
    if(wrapped_devices.contains(new_uid))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::changeDeviceUID", "Device", old_uid))
        return old_uid;
    }

    auto device_node = wrapped_devices.extract(old_uid);
    device_node.key() = new_uid;
    wrapped_devices.insert(std::move(device_node));
    return new_uid;
}

void Theatre::addInterpretedActor(Actor* new_actor)
{
    if(wrapped_actors.contains(new_actor->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addInterpretedActor", "an Actor", new_actor->getUID()))
        return;
    }

    parallelAddActor(new_actor, new_actor->getUID(), true);
}

void Theatre::addInterpretedDevice(Device* new_device)
{
    if(wrapped_devices.contains(new_device->getUID()))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addInterpretedDevice", "a Device", new_device->getUID()))
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
    checkAndSetCurrentVariables(pointer, nullptr);
}

void Theatre::parallelAddDevice(Device* pointer, const int UID, const bool ownership)
{
    wrapped_devices[UID] = DevicePointerWrapper(pointer, ownership);
    unwrapped_devices.insert(unwrapped_devices.end(), pointer);
    checkAndManageParallelActorDesync();
    checkAndSetCurrentVariables(nullptr, pointer);
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

void Theatre::checkAndSetCurrentVariables(Actor* new_actor, Device* new_device)
{
    if(new_actor != nullptr)
    {
        if(GraphXPlayer* new_player = dynamic_cast<GraphXPlayer*>(new_actor))
            graphx::current::player = new_player;
        // else if...
    }

    if(new_device != nullptr)
    {
        if(Environment* new_environment = dynamic_cast<Environment*>(new_device))
            graphx::current::environment = new_environment;
        // else if...
    }
}