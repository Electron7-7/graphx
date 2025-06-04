#include "theatre.hpp"
#include "engine/interpreter/graphx_interpreter_lookups.hpp"
#include "engine/things/actors/actors.hpp"
#include "engine/rendering/rendering.hpp"
#include "engine/things/settings.hpp"
#include <set>

// TEMPORARY
LightFlashlight* Theatre::getFlashlight() const
{
    for(Actor* actor : actor_vector)
        if(actor->name.find("flashlight") != std::string::npos || !actor->getTypeName().compare("LightFlashlight") || dynamic_cast<LightFlashlight*>(actor))
            return dynamic_cast<LightFlashlight*>(actor);
    return &graphx::missing::temporary_backup_flashlight;
}
// TEMPORARY

// REDUNDANT!!!! (but helps keep code pretty, I guess)
Theatre* getCurrentTheatre(bool print_note)
{
    if(graphx::TheatreHandler.getCurrentTheatre()->getUID() == -1 && print_note)
        PRINTDEBUG("getCurrentTheatre() called, but graphx::TheatreHandler.getCurrentTheatre()->getUID() returned -1! This may be a problem, but the engine shouldn't crash... theoretically")
    return graphx::TheatreHandler.getCurrentTheatre();
}

//
// Theatre
//
Theatre::Theatre(std::string init_name, long new_uid)
: name(init_name), UID(new_uid)
{
    // stage_material = new Material(false, glm::vec3(0.5, 0.1, 0.4));
    // stage_mesh = new Mesh(stage_material);
    // stage_mesh->name = "Stage Mesh for Theatre (" + name + ")";
}

std::vector<int> Theatre::dumpActorIDs()
{
    std::vector<int> actor_ids;
    for(auto &actor_pair : objects)
    {
        actor_ids.insert(actor_ids.end(), actor_pair.first);
    }

    return actor_ids;
}

std::vector<Actor*> Theatre::getTroupe()
{
    return actor_vector;
}

std::set<std::string> Theatre::getMeshDataNames()
{
    std::set<std::string> mesh_data_names = {ERROR_MODEL};

    for(Device* device : device_vector)
        if(dynamic_cast<Mesh*>(device))
            mesh_data_names.insert(dynamic_cast<Mesh*>(device)->mesh_data_name);

    return mesh_data_names;
}

std::set<std::string> Theatre::getTextureNames()
{
    std::set<std::string> texture_names;

    for(Device* device : device_vector)
    {
        if(dynamic_cast<Material*>(device))
        {
            texture_names.insert(dynamic_cast<Material*>(device)->diffuse_texture_name);
            texture_names.insert(dynamic_cast<Material*>(device)->specular_texture_name);
        }
    }

    texture_names.insert(MISSING_TEXTURE);
// #ifdef GRAPHX_DEBUG // I want light debug meshes to be visible even on the Production builds for now
    texture_names.insert(LIGHT_DEBUGGING);
// #endif
    return texture_names;
}

void Theatre::probeActorsForRenderCommands()
{
    point_lights_count = 0;
    spot_lights_count = 0;
    directional_lights_count = 0;

    for(Actor* actor : actor_vector)
    {
        if(loading_new_main_theatre)
            return;

        if(dynamic_cast<Light*>(actor))
        {
            if(dynamic_cast<LightSpot*>(actor))
                spot_lights_count++;
            else if(dynamic_cast<LightDirectional*>(actor))
                directional_lights_count++;
            else
                point_lights_count++;
        }

        R_BufferRenderCommands(actor->getRenderCommands());
    }
}

void Theatre::raiseCurtains()
{
    dropping_curtains = false;
    bool has_directional_light = false;

    for(Device* device : device_vector)
    {
        if(dynamic_cast<Environment*>(device))
            environment_uid = device->getUID();
        device->initialize();
    }

    for(Actor* actor : actor_vector)
    {
        if(dynamic_cast<GraphXPlayer*>(actor))
            player_uid = actor->getUID();
        else if(dynamic_cast<LightDirectional*>(actor))
            has_directional_light = true;
        actor->callToStage(this);
        
    }

    if(!has_directional_light)
    {
        PRINTERR("Theatre \"" << name << "\" doesn't have a directional light, which is pretty much a representation of the sun! I'm gonna assume you did this on purpose, so in order for the lighting to render \"properly\", I'm adding a LightDirectional light to this Theatre but making its light output pitch black.")
        createActor("LightDirectional", 55252525);
        objects.at(55252525)->name = "THE FUCKING SUN HAS GONE OUT!!!!!";
        dynamic_pointer_cast<LightDirectional>(objects.at(55252525))->light_color = glm::vec3(0.0f);
        dynamic_pointer_cast<LightDirectional>(objects.at(55252525))->light_energy = 0.0f;
        dynamic_pointer_cast<LightDirectional>(objects.at(55252525))->light_ambient_strength = 0.0f;
        dynamic_pointer_cast<LightDirectional>(objects.at(55252525))->light_specular_strength = 0.0f;
    }
}

void Theatre::dropCurtains()
{
    dropping_curtains = true;

    for(auto& pair : objects)
        pair.second->takeABow();
    actor_vector.clear();

    for(auto& pair : devices)
        pair.second->prepForDestruction();
    device_vector.clear();
}

// void Theatre::loadStageSettings(gSettings stage_settings)
// {
//     glm::vec3 stage_euler_degrees = glm::vec3(0.0f);
//     stage_settings.getNumber("Position", stage_position);
//     stage_settings.getNumber("Scale", stage_scale);
//     stage_settings.getNumber("Rotation", stage_euler_degrees);
//     stage_quaternion = glm::quat(glm::radians(stage_euler_degrees));
// }

void Theatre::delegateKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    for(auto &pair : objects)
        pair.second->processKey(window, key, scancode, action, mods);
}

void Theatre::delegateMouseInput(GLFWwindow *window, double x_position_in, double y_position_in)
{
    // Todo: don't use `getTroupe()` here, this is stupid
    for(Actor* actor : getTroupe())
        actor->processMouse(window, x_position_in, y_position_in);
}

long Theatre::getUID()
{
    return UID;
}

void Theatre::setUID(long new_uid)
{
    if(new_uid == -1)
        PRINTERR("Attempting to set Theatre \"" << name << "\"'s UID to -1!")
    UID = new_uid;
}

void Theatre::createActor(const std::string& actor_type, long uid, gSettings new_settings)
{
    if(objects.contains(uid))
    {
        PRINTERR("Tried adding a new Actor with UID " << std::to_string(uid) << " to Theatre " << name << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
        return;
    }

    if(!actor_type.compare("GraphXPlayer")) // TEMPORARY
        player_uid = uid;

    objects[uid] = valid_actors.at(actor_type)(this, uid, new_settings); // VERY BAD, WILL BE REPLACED WITH SMART POINTERS WHEN FULLY IMPLEMENTED
    objects.at(uid)->setUID(uid);
    objects.at(uid)->loadSettings();
    actor_vector.insert(actor_vector.end(), objects.at(uid).get());

    if(time_to_render)
        objects.at(uid)->callToStage(this);

    time_to_store_buffers = time_to_render;
}

void Theatre::createDevice(const std::string& device_type, long uid, gSettings new_settings)
{
    if(devices.contains(uid))
    {
        PRINTERR("Tried adding a new Device with UID " << std::to_string(uid) << " to Theatre " << name << " but a Device with that UID already exists! Aborting addition of this Device! If there are problems or crashes, this may be the cause!")
        return;
    }

    if(!device_type.compare("Environment")) // TEMPORARY
        environment_uid = uid;

    devices[uid] = valid_devices.at(device_type)(this, uid, new_settings); // VERY BAD, WILL BE REPLACED WITH SMART POINTERS WHEN FULLY IMPLEMENTED
    devices.at(uid)->setUID(uid);
    devices.at(uid)->loadSettings();

    device_vector.insert(device_vector.end(), devices.at(uid).get());
}

// TEMPORARY: USED BY THE ROTATING LIGHT ACTOR FOR ITS PIVOT POINT VISUALIZER ACTOR
void Theatre::actorEnter(Actor* new_actor, long uid, gSettings new_settings)
{
    if(objects.contains(uid))
    {
        PRINTERR("Tried adding a new Actor with UID " << std::to_string(uid) << " to Theatre " << name << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
        return;
    }

    objects[uid] = std::shared_ptr<Actor>(new_actor);
    actor_vector.insert(actor_vector.end(), objects.at(uid).get());

    new_actor->setUID(uid);

    if(dynamic_cast<GraphXPlayer*>(new_actor))
        player_uid = uid;

    new_actor->loadSettings();

    // Writing this has made me realize just how nasty my usage of pointers is.
    // I want to rectify this by using UIDs instead; basically, instead of
    // Actor::mesh being a Mesh pointer, it'd be the UID of a Mesh.
    // Same goes for Mesh::material.
    // Basically, anything that's stored somewhere else shouldn't be a pointer (Materials are already stored in devices, but I'm going to make them more closely resemble MeshData)
    if(!devices.contains(new_actor->mesh->getUID()))
    {
        int mesh_id = new_actor->mesh->getUID();
        // devices[new_actor->mesh->getUID()] = new Mesh(*new_actor->mesh);
        devices[mesh_id] = std::make_shared<Mesh>(*new_actor->mesh);
        // new_actor->mesh = dynamic_cast<Mesh *>(devices.at(mesh_id).get()); // Unnecessary
    }

    if(time_to_render)
        new_actor->callToStage(this);

    time_to_store_buffers = time_to_render;
}

Actor* Theatre::getActor(long actor_uid)
{
    if(objects.contains(actor_uid))
        return objects.at(actor_uid).get();

    PRINTERR("Hey! Someone asked for an Actor with the UID " << std::to_string(actor_uid) << ", but none were found! The \"getActor\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
    return nullptr;
}

Actor* Theatre::getActor(std::string actor_name)
{
    for(Actor* actor : actor_vector)
    {
        if(!actor->name.compare(actor_name))
            return actor;
    }

    PRINTERR("Hey! Someone asked for an Actor named " << actor_name << ", but none were found! The \"getActor\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
    return nullptr;
}

Device* Theatre::getDevice(long device_uid)
{
    if(devices.contains(device_uid))
        return devices.at(device_uid).get();

    PRINTERR("Hey! Someone asked for a Device with the UID " << std::to_string(device_uid) << ", but none were found! The \"getDevice\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
    return nullptr;
}

Device* Theatre::getDevice(std::string device_name)
{
    for(Device* device : device_vector)
        if(!device->name.compare(device_name))
            return device;

    PRINTERR("Hey! Someone asked for a Device named " << device_name << ", but none were found! The \"getDevice\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
    return nullptr;
}

GraphXPlayer* Theatre::getPlayer()
{
    if(player_uid == -1)
    {
        for(Actor* actor : actor_vector)
        {
            if(dynamic_cast<GraphXPlayer*>(actor))
            {
                player_uid = actor->getUID();
                return dynamic_cast<GraphXPlayer*>(actor);
            }
        }
    }

    if(objects.contains(player_uid))
        return dynamic_cast<GraphXPlayer*>(objects.at(player_uid).get());

    PRINTERR("SAFETY PLAYER RETURNING!!!")
    static GraphXPlayer safety_player = GraphXPlayer("temporary player");
    return &safety_player; // TEMPORARY: AVOIDS CRASHING
}

Environment* Theatre::getEnvironment()
{
    if(environment_uid == -1)
    {
        for(Device* device : device_vector)
        {
            if(dynamic_cast<Environment*>(device))
            {
                environment_uid = device->getUID();
                return dynamic_cast<Environment *>(device);
            }
        }
    }

    if(devices.contains(environment_uid))
        return dynamic_cast<Environment *>(devices.at(environment_uid).get());

    PRINTERR("SAFETY ENVIRONMENT RETURNING!!!")
    static Environment safety_environment = Environment("Safety Environment");
    return &safety_environment; // TEMPORARY: AVOIDS CRASHING
}