#include "g_theatre.hpp"
#include "g_actors.hpp"
#include "r_rendering.hpp"
#include <set>

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
: name(new_name), theatre_uid(new_uid)
{}

Theatre::Theatre(const std::string& new_name)
: Theatre(-1, new_name)
{}

// Terrible, no good, very bad functions
std::set<std::string> Theatre::getMeshDataNames()
{
    std::set<std::string> mesh_data_names = {ERROR_MODEL};

    for(int i = 0; i < device_vector.size(); i++)
        if(dynamic_pointer_cast<Model>(device_vector[i]))
        {
            std::shared_ptr<Model> test = dynamic_pointer_cast<Model>(device_vector[i]);
            mesh_data_names.insert(dynamic_pointer_cast<Model>(device_vector[i])->mesh_data_name);
        }

    return mesh_data_names;
}

std::set<std::string> Theatre::getTextureNames()
{
    std::set<std::string> texture_names;

    for(int i = 0; i < device_vector.size(); i++)
        if(std::shared_ptr<Material> material = dynamic_pointer_cast<Material>(device_vector[i]))
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
{ return theatre_uid; }

void Theatre::setUID(int new_uid)
{ theatre_uid = new_uid; }

void Theatre::probeRenderCommands()
{
    point_lights_count = 0;
    spot_lights_count = 0;
    directional_lights_count = 0;

    for(int i = 0; i < actor_vector.size(); i++)
    {
        if(graphx::state::loading_new_main_theatre)
            return;

        if(dynamic_pointer_cast<Light>(actor_vector.at(i)))
        {
            if(dynamic_pointer_cast<LightSpot>(actor_vector.at(i)))
                spot_lights_count++;

            else if(dynamic_pointer_cast<LightDirectional>(actor_vector.at(i)))
                directional_lights_count++;

            else
                point_lights_count++;
        }

        R_BufferRenderCommands(actor_vector.at(i)->getRenderCommands());
    }
}

void Theatre::delegateKeyInput(GLFWwindow* window, const int key, const int scancode, const int action, const int mods) const
{
    for(std::shared_ptr<Actor> actor : actor_vector)
        actor->processKey(window, key, scancode, action, mods);
}

void Theatre::delegateMouseInput(GLFWwindow* window, const double x_position_in, const double y_position_in) const
{
    for(std::shared_ptr<Actor> actor : actor_vector)
        actor->processMouse(window, x_position_in, y_position_in);
}

const LightsCount Theatre::getLightsCount() const
{ return LightsCount(point_lights_count, spot_lights_count, directional_lights_count); }

int Theatre::addActor(std::shared_ptr<Actor> new_actor)
{
    int uid = new_actor->getUID();
    if(actor_map.contains(uid) || uid == -1)
    {
        PRINTNOTE("In Theatre::addActor - Actor UID is either -1 or already in the Theatre. Changing this Actor's UID before adding it")
        uid = generateUID(true);
    }

    parallelAddActor(new_actor, uid, false);
    return uid;
}

int Theatre::addDevice(std::shared_ptr<Device> new_device)
{
    int uid = new_device->getUID();
    if(device_map.contains(uid) || uid == -1)
    {
        PRINTNOTE("In Theatre::addDevice - Device UID is either -1 or already in the Theatre. Changing this Device's UID before adding it")
        uid = generateUID(true);
    }

    parallelAddDevice(new_device, uid, false);
    return uid;
}

std::vector<std::shared_ptr<Actor>> Theatre::getAllActors() const
{ return actor_vector; }

std::vector<std::shared_ptr<Device>> Theatre::getAllDevices() const
{ return device_vector; }

int Theatre::getActorUID(const std::string& actor_name) const
{
    for(int i = 0; i < actor_vector.size(); i++)
        if(!actor_vector.at(i)->name.compare(actor_name))
            return actor_vector.at(i)->getUID();
    return -1;
}

int Theatre::getDeviceUID(const std::string& device_name) const
{
    for(int i = 0; i < device_vector.size(); i++)
        if(!device_vector.at(i)->name.compare(device_name))
            return device_vector.at(i)->getUID();
    return -1;
}

// Private functions
int Theatre::generateUID(const bool for_actor)
{
    std::uniform_int_distribution<> uid_distribution(0);
    unsigned int new_uid = 0;

    if(for_actor)
        while(actor_map.contains(new_uid))
            new_uid = uid_distribution(uid_random_generator);
    else
        while(device_map.contains(new_uid))
            new_uid = uid_distribution(uid_random_generator);

    return new_uid;
}

int Theatre::changeActorUID(const int old_uid, const int new_uid)
{
    if(actor_map.contains(new_uid))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::changeActorUID", "Actor", old_uid))
        return old_uid;
    }

    auto actor_node = actor_map.extract(old_uid);
    actor_node.key() = new_uid;
    actor_map.insert(std::move(actor_node));
    return new_uid;
}

int Theatre::changeDeviceUID(const int old_uid, const int new_uid)
{
    if(device_map.contains(new_uid))
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::changeDeviceUID", "Device", old_uid))
        return old_uid;
    }

    auto device_node = device_map.extract(old_uid);
    device_node.key() = new_uid;
    device_map.insert(std::move(device_node));
    return new_uid;
}

void Theatre::addInterpretedActor(std::shared_ptr<Actor> new_actor)
{
    int uid = new_actor->getUID();

    if(actor_map.contains(uid) && uid != -1)
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addInterpretedActor", "an Actor", new_actor->getUID()))
        return;
    }

    else if(uid == -1)
        uid = generateUID(true);

    parallelAddActor(new_actor, new_actor->getUID(), true);
}

void Theatre::addInterpretedDevice(std::shared_ptr<Device> new_device)
{
    int uid = new_device->getUID();

    if(device_map.contains(uid) && uid != -1)
    {
        PRINTERR(THEATRE_ERR_DUPLICATE_UID("Theatre::addInterpretedDevice", "a Device", new_device->getUID()))
        return;
    }

    else if(uid == -1)
        uid = generateUID(false);

    parallelAddDevice(new_device, uid, true);
}

// These functions make sure that the maps and vectors are kept parallel; since these are private
// utility functions, it is assumed that all safety checks have already been made. These functions
// are one to two steps away from just being a #define macro for std::vector::insert and std::map[]
// so treat them as such and MAKE YOUR SAFETY CHECKS BEFORE CALLING THEM! To avoid undefined behaviour,
// these functions will ALWAYS put their pointer argument into their respective map & vector.
void Theatre::parallelAddActor(std::shared_ptr<Actor> pointer, const int UID, const bool ownership)
{
    actor_map[UID] = pointer;
    actor_vector.insert(actor_vector.end(), pointer);
    pointer->loadSettings();
    checkAndManageParallelActorDesync();
    checkAndSetCurrentVariables(pointer, nullptr);
}

void Theatre::parallelAddDevice(std::shared_ptr<Device> pointer, const int UID, const bool ownership)
{
    device_map[UID] = pointer;
    device_vector.insert(device_vector.end(), pointer);
    pointer->loadSettings();
    checkAndManageParallelActorDesync();
    checkAndSetCurrentVariables(nullptr, pointer);
}

void Theatre::parallelRemoveActor(const int UID)
{
    for(int i = 0; i < actor_vector.size(); i++)
        if(actor_vector.at(i) == actor_map.at(UID))
            actor_vector.erase(actor_vector.begin() + i);

    actor_map.erase(UID);
}

void Theatre::parallelRemoveDevice(const int UID)
{
    for(int i = 0; i < device_vector.size(); i++)
        if(device_vector.at(i) == device_map.at(UID))
            device_vector.erase(device_vector.begin() + i);

    device_map.erase(UID);
}

void Theatre::checkAndManageParallelActorDesync()
{
    if(actor_map.size() == actor_vector.size())
        return;

    PRINTERR(THEATRE_ERR_PARALLEL_DESYNC("Theatre::parallelAddActor(std::shared_ptr<Actor>, const bool)", "Actor"))

    std::set<int> synced_uids;

    if(actor_map.size() > actor_vector.size())
    {
        for(std::shared_ptr<Actor> pointer : actor_vector)
            synced_uids.insert(pointer->getUID());

        for(auto& pair : actor_map)
        {
            if(!synced_uids.contains(pair.first))
            {
                PRINTERR(THEATRE_ERR_DESYNC_DETECTION("Actor", "std::map<int, ActorPointerWrapper> Theatre::actor_map", pair.first))
                actor_map.erase(pair.first);
                return;
            }
        }
    }

    for(auto& pair : actor_map)
        synced_uids.insert(pair.first);

    for(int i = 0; i < actor_vector.size(); i++)
    {
        if(!synced_uids.contains(actor_vector[i]->getUID()))
        {
            PRINTERR(THEATRE_ERR_DESYNC_DETECTION("Actor", "std::vector<std::shared_ptr<Actor>> Theatre::actor_vector", actor_vector[i]->getUID()))
            actor_vector.erase(actor_vector.begin() + i);
            return;
        }
    }

    PRINTERR("Theatre::checkAndManageParallelActorDesync() - Desync detected, but not rectified!")
}

void Theatre::checkAndManageParallelDeviceDesync()
{
    if(device_map.size() == device_vector.size())
        return;

    PRINTERR(THEATRE_ERR_PARALLEL_DESYNC("Theatre::parallelAddDevice(std::shared_ptr<Device>, const bool)", "Device"))

    std::set<int> synced_uids;

    if(device_map.size() > device_vector.size())
    {
        for(std::shared_ptr<Device> pointer : device_vector)
            synced_uids.insert(pointer->getUID());

        for(auto& pair : device_map)
        {
            if(!synced_uids.contains(pair.first))
            {
                PRINTERR(THEATRE_ERR_DESYNC_DETECTION("Device", "std::map<int, DevicePointerWrapper> Theatre::device_map", pair.first))
                device_map.erase(pair.first);
                return;
            }
        }
    }

    for(auto& pair : device_map)
        synced_uids.insert(pair.first);

    for(int i = 0; i < device_vector.size(); i++)
    {
        if(!synced_uids.contains(device_vector[i]->getUID()))
        {
            PRINTERR(THEATRE_ERR_DESYNC_DETECTION("Device", "std::vector<std::shared_ptr<Device>> Theatre::device_vector", device_vector[i]->getUID()))
            device_vector.erase(device_vector.begin() + i);
            return;
        }
    }

    PRINTERR("Theatre::checkAndManageParallelDeviceDesync() - Desync detected, but not rectified!")
}

void Theatre::checkAndSetCurrentVariables(std::shared_ptr<Actor> new_actor, std::shared_ptr<Device> new_device)
{
    if(new_actor)
    {
        if(auto player = std::dynamic_pointer_cast<GraphXPlayer>(new_actor))
            graphx::current::uids::player = new_actor->getUID();
        // else if...
    }

    if(new_device)
    {
        if(auto environment = dynamic_pointer_cast<Environment>(new_device))
            graphx::current::uids::environment = new_device->getUID();
        // else if...
    }
}