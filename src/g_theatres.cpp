#include "graphx_classes_namespace.hpp"
#include "g_actors.hpp"
#include "r_common.hpp"
#include "t_settings.hpp"
#include "g_imgui.hpp"
#include <algorithm>
#include <set>

Theatre current_theatre;

Theatre *getCurrentTheatre(bool print_note)
{
    if(current_theatre.getUID() == -1 && print_note)
        PRINTDEBUG("getCurrentTheatre() called, but current_theatre.getUID() returned -1! This may be a problem, but the engine shouldn't crash... theoretically")
    return &current_theatre;
}

Environment *getCurrentEnvironment()
{
    if(loading_new_main_theatre)
        return new Environment();

    if(current_theatre.unsafeGetFirstDeviceOfType(graphx::classes::ENVIRONMENT) == nullptr)
    {
        PRINTERR("getCurrentEnvironment called, but no Environment Device found in current_theatre! Every Theatre needs an Environment! A new Environment will be created and given a UID of 177013 (in Theatre \"" << current_theatre.name << "\")")
        current_theatre.createDevice(graphx::classes::ENVIRONMENT, 177013);
    }

    return static_cast<Environment *>(current_theatre.getFirstDeviceOfType(graphx::classes::ENVIRONMENT));
}

GraphXPlayer *getCurrentPlayer()
{
    if(loading_new_main_theatre)
        return new GraphXPlayer();

    if(current_theatre.unsafeGetFirstActorOfType(graphx::classes::GRAPHXPLAYER) == nullptr)
    {
        PRINTERR("getCurrentPlayer called, but no GraphXPlayer Actor found in current_theatre! Every Theatre needs a GraphXPlayer! A new GraphXPlayer will be created and given a UID of 42069 (in Theatre \"" << current_theatre.name << "\")")
        current_theatre.createActor(graphx::classes::GRAPHXPLAYER, 42069);
    }

    return static_cast<GraphXPlayer *>(current_theatre.getFirstActorOfType(graphx::classes::GRAPHXPLAYER));
}

//
// Theatre
//
Theatre::Theatre(std::string init_name, long new_uid)
: name(init_name), UID(new_uid)
{
    stage_material = new Material(false, glm::vec3(0.5, 0.1, 0.4));
    stage_mesh = new Mesh(stage_material);
    stage_mesh->setName("Stage Mesh for Theatre (" + name + ")");
}

std::vector<long> Theatre::dumpActorIDs()
{
    std::vector<long> actor_ids;
    for(auto &actor_pair : objects)
    {
        actor_ids.insert(actor_ids.end(), actor_pair.first);
    }

    return actor_ids;
}

std::set<std::string> Theatre::getMeshDataNames()
{
    std::set<std::string> mesh_data_names;

    for(auto &device_pair : devices)
        if(device_pair.second->getType() == graphx::classes::MESH)
            mesh_data_names.insert(static_cast<Mesh *>(device_pair.second)->mesh_data_name);

    mesh_data_names.insert(ERROR_MODEL);
    return mesh_data_names;
}

std::set<std::string> Theatre::getTextureNames()
{
    std::set<std::string> texture_names;

    for(auto &device_pair : devices)
    {
        if(device_pair.second->getType() == graphx::classes::MATERIAL)
        {
            texture_names.insert(static_cast<Material *>(device_pair.second)->diffuse_texture_name);
            texture_names.insert(static_cast<Material *>(device_pair.second)->specular_texture_name);
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
    for(auto &pair : objects)
    {
        if(loading_new_main_theatre)
            return;

        if(graphx::classes::isLight(pair.second->getType()))
        {
            LightRenderCmd light_render_command;
            if(static_cast<Light *>(pair.second)->debug_visible)
            {
                light_render_command.current_render_state = &pair.second->current_state_buffer[pair.second->state_index];
                light_render_command.previous_render_state = &pair.second->previous_state_buffer[pair.second->state_index];
            }

            light_render_command.light_type = static_cast<Light *>(pair.second)->getLightType();

            if(light_render_command.light_type == graphx::classes::LIGHT)
                light_render_command.light_data = static_cast<Light *>(pair.second)->getLightData();

            else if(light_render_command.light_type == graphx::classes::LIGHTSPOT)
                light_render_command.light_data = static_cast<LightSpot *>(pair.second)->getLightData();

            else if(light_render_command.light_type == graphx::classes::LIGHTDIRECTIONAL)
                light_render_command.light_data = static_cast<LightDirectional *>(pair.second)->getLightData();

            R_BufferRenderCmd(light_render_command);
            continue;
        }

        if(!pair.second->wantsToBeRendered())
            continue;

        RenderCmd render_command;
        render_command.current_render_state = &pair.second->current_state_buffer[pair.second->state_index];
        render_command.previous_render_state = &pair.second->previous_state_buffer[pair.second->state_index];
        render_command.mesh_data_name = pair.second->mesh->mesh_data_name;
        render_command.mesh_material = *pair.second->mesh->material;

        R_BufferRenderCmd(render_command);
    }
}

void Theatre::raiseCurtains()
{
    dropping_curtains = false;
    bool has_directional_light = false;

    for(auto &pair : devices)
    {
        if(pair.second->getType() == graphx::classes::ENVIRONMENT)
            environment_uid = pair.first;
        pair.second->initialize();
    }

    for(auto &pair : objects)
    {
        if(pair.second->getType() == graphx::classes::GRAPHXPLAYER)
            player_uid = pair.first;
        else if(pair.second->getType() == graphx::classes::LIGHTDIRECTIONAL)
            has_directional_light = true;
        pair.second->callToStage(this);
        troupe.insert(troupe.end(), pair.second);
    }

    if(!has_directional_light)
    {
        PRINTERR("Theatre \"" << name << "\" doesn't have a directional light, which is pretty much a representation of the sun! I'm gonna assume you did this on purpose, so in order for the lighting to render \"properly\", I'm adding a LightDirectional light to this Theatre but making its light output pitch black.")
        createActor(graphx::classes::LIGHTDIRECTIONAL, 55252525);
        objects.at(55252525)->setName("THE FUCKING SUN HAS GONE OUT!!!!!");
        static_cast<LightDirectional *>(objects.at(55252525))->light_color = glm::vec3(0.0f);
        static_cast<LightDirectional *>(objects.at(55252525))->light_energy = 0.0f;
        static_cast<LightDirectional *>(objects.at(55252525))->light_ambient_strength = 0.0f;
        static_cast<LightDirectional *>(objects.at(55252525))->light_specular_strength = 0.0f;
    }

    sortTroupe();
    countLights();
}

void Theatre::dropCurtains()
{
    dropping_curtains = true;

    for(auto &pair : devices)
        pair.second->prepForDestruction();
    devices.clear();

    for(auto &pair : objects)
        pair.second->takeABow();
    objects.clear();
    troupe.clear();
}

std::vector<Actor *> Theatre::getAllActorsOfType(graphx::gClass type_name)
{
    std::vector<Actor *> found_actors;

    for(auto &pair : objects)
        if(pair.second->getType() == type_name)
            found_actors.insert(found_actors.end(), pair.second);

    return found_actors;
}

std::vector<Device *> Theatre::getAllDevicesOfType(graphx::gClass type_name)
{
    std::vector<Device *> found_devices;

    for(auto &pair : devices)
        if(pair.second->getType() == type_name)
            found_devices.insert(found_devices.end(), pair.second);

    return found_devices;
}

void Theatre::loadStageSettings(graphx::gSettings stage_settings)
{
    glm::vec3 stage_euler_degrees = glm::vec3(0.0f);
    getSetting(stage_position, stage_settings["Position"]);
    getSetting(stage_scale, stage_settings["Scale"]);
    getSetting(stage_euler_degrees, stage_settings["Rotation"]);
    stage_quaternion = glm::quat(glm::radians(stage_euler_degrees));
}

void Theatre::delegateKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    for(auto &pair : objects)
        pair.second->processKey(window, key, scancode, action, mods);
}

void Theatre::delegateMouseInput(GLFWwindow *window, double x_position_in, double y_position_in)
{
    for(Actor *actor : troupe)
        actor->processMouse(window, x_position_in, y_position_in);
}

std::string getSettingName(graphx::gSetting setting)
{
    switch(setting.first)
    {
    case THEATRE_REFERENCE:
        if(setting.second.type() == typeid(Actor*))
            return(std::any_cast<Actor *>(setting.second)->getName());

        if(setting.second.type() == typeid(Device*))
            return(std::any_cast<Device *>(setting.second)->getName());

        return "Unknown Theatre Reference setting";
    case RAW_DATA:
        if(setting.second.type() == typeid(graphx::interpreter::gRawData))
        {
            std::string buffer = "";
            graphx::interpreter::gRawData raw_data = std::any_cast<graphx::interpreter::gRawData>(setting.second);
            for(int i = 0 ; i < raw_data.size() ; i++)
            {
                buffer += raw_data[i];
                if(i != raw_data.size() - 1)
                    buffer += ", ";
            }

            return buffer;
        }

        return "Unknown Raw Data setting";
    case CPP_REFERENCE:
        return "C++ Reference setting";
    case SANDWICH:
        if(setting.second.type() == typeid(Actor*))
            return(std::any_cast<Actor *>(setting.second)->getName());

        if(setting.second.type() == typeid(Device*))
            return(std::any_cast<Device *>(setting.second)->getName());

        return "Unknown Theatre Reference setting (Sandwich Bun)";
    case EXTERNAL_REFERENCE:
        return "External Reference setting";
    default:
        return "Setting type unknown/invalid!";
    }
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

Actor *Theatre::getFirstActorOfType(graphx::gClass type_name)
{
    if(graphx::classes::getBaseType(type_name) != graphx::classes::ACTOR)
    {
        PRINTERR("Theatre::getFirstActorOfType - Supplied type is not derived from Actor! Returning nullptr!")
        return nullptr;
    }

    for(auto &pair : objects)
        if(pair.second->getType() == type_name)
            return pair.second;

    PRINTERR("Theatre::getFirstActorOfType could not find an Actor of type: " << type_name.name << "! This function will return a nullptr!")
    return nullptr;
}

Device *Theatre::getFirstDeviceOfType(graphx::gClass type_name)
{
    if(graphx::classes::getBaseType(type_name) != graphx::classes::DEVICE)
    {
        PRINTERR("Theatre::getFirstDeviceOfType - Supplied type is not derived from Device! Returning nullptr!")
        return nullptr;
    }

    for(auto &pair : devices)
        if(pair.second->getType() == type_name)
            return pair.second;

    PRINTERR("Theatre::getFirstDeviceOfType could not find a Device of type: " << type_name.name << "! This function will return a nullptr!")
    return nullptr;
}

Actor *Theatre::unsafeGetFirstActorOfType(graphx::gClass type_name)
{
    if(graphx::classes::getBaseType(type_name) != graphx::classes::ACTOR)
    {
        PRINTDEBUG("Theatre::getFirstActorOfType - Supplied type is not derived from Actor! Returning nullptr!")
        return nullptr;
    }

    for(auto &pair : objects)
        if(pair.second->getType() == type_name)
            return pair.second;
    return nullptr;
}

Device *Theatre::unsafeGetFirstDeviceOfType(graphx::gClass type_name)
{
    if(graphx::classes::getBaseType(type_name) != graphx::classes::DEVICE)
    {
        PRINTDEBUG("Theatre::getFirstDeviceOfType - Supplied type is not derived from Device! Returning nullptr!")
        return nullptr;
    }

    for(auto &pair : devices)
        if(pair.second->getType() == type_name)
            return pair.second;
    return nullptr;
}

void Theatre::createActor(graphx::gClass actor_type, long uid, graphx::gSettings new_settings)
{
    if(objects.contains(uid))
    {
        PRINTERR("Tried adding a new Actor with UID " << std::to_string(uid) << " to Theatre " << name << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
        return;
    }

    if(actor_type == graphx::classes::GRAPHXPLAYER)
        player_uid = uid;

    objects[uid] = graphx::gClass::getClassType(actor_type).create_new_actor();
    objects.at(uid)->setUID(uid);
    objects.at(uid)->youGotACallBack(new_settings);

    troupe.insert(troupe.end(), objects.at(uid));

    sortTroupe();
    countLights();

    if(time_to_render)
    {
        objects.at(uid)->callToStage(this);
    }

    time_to_store_buffers = time_to_render;
}

void Theatre::createDevice(graphx::gClass device_type, long uid, graphx::gSettings new_settings)
{
    if(devices.contains(uid))
    {
        PRINTERR("Tried adding a new Device with UID " << std::to_string(uid) << " to Theatre " << name << " but a Device with that UID already exists! Aborting addition of this Device! If there are problems or crashes, this may be the cause!")
        return;
    }

    if(device_type == graphx::classes::ENVIRONMENT)
        environment_uid = uid;

    devices[uid] = graphx::gClass::getClassType(device_type).create_new_device();
    devices.at(uid)->setUID(uid);
    devices.at(uid)->loadSettings(new_settings);
}

void Theatre::troupeEnter(std::vector<std::pair<Actor *, long>> new_troupe)
{
    for(auto &pair : new_troupe)
    {
        if(objects.contains(pair.second))
        {
            PRINTERR("Tried adding a new Actor with UID " << std::to_string(pair.second) << " to Theatre " << name << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
            return;
        }

        objects[pair.second] = pair.first;
        pair.first->setUID(pair.second);
        troupe.insert(troupe.end(), objects.at(pair.second));

        if(pair.first->getType() == graphx::classes::GRAPHXPLAYER)
            player_uid = pair.second;

        if(time_to_render)
        {
            pair.first->youGotACallBack();
            pair.first->callToStage(this);
        }
    }

    sortTroupe();
    countLights();
    time_to_store_buffers = time_to_render;
}

void Theatre::actorEnter(Actor *new_actor, long uid, graphx::gSettings new_settings)
{
    if(objects.contains(uid))
    {
        PRINTERR("Tried adding a new Actor with UID " << std::to_string(uid) << " to Theatre " << name << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
        return;
    }

    objects[uid] = new_actor;
    troupe.insert(troupe.end(), objects.at(uid));
    new_actor->setUID(uid);

    if(new_actor->getType() == graphx::classes::GRAPHXPLAYER)
        player_uid = uid;

    new_actor->youGotACallBack(new_settings);

    // Writing this has made me realize just how nasty my usage of pointers is.
    // I want to rectify this by using UIDs instead; basically, instead of
    // Actor::mesh being a Mesh pointer, it'd be the UID of a Mesh.
    // Same goes for Mesh::material.
    // Basically, anything that's stored somewhere else shouldn't be a pointer (Materials are already stored in devices, but I'm going to make them more closely resemble MeshData)
    if(!devices.contains(new_actor->mesh->getUID()))
    {
        devices[new_actor->mesh->getUID()] = new Mesh(*new_actor->mesh);
        new_actor->mesh = static_cast<Mesh *>(devices.at(new_actor->mesh->getUID()));
    }

    sortTroupe();
    countLights();

    if(time_to_render)
        new_actor->callToStage(this);

    time_to_store_buffers = time_to_render;
}

void Theatre::actorLeave(Actor *old_actor)
{
    if(old_actor->getType() == graphx::classes::GRAPHXPLAYER)
        player_uid = -1;

    if(auto it = objects.find(old_actor->getUID()) ; it != objects.end())
    {
        it->second = NULL;
        objects.erase(it);
        int i = 0;
        for(auto it = troupe.begin() ; it != troupe.end() ; it++,i++)
        {
            if (troupe[i] == old_actor)
            {
                troupe[i] = NULL;
                troupe.erase(it);
            }
        }

        countLights();
        time_to_store_buffers = time_to_render;
        return;
    }

    PRINTERR("Request to remove an Actor by pointer failed!\n\tUID of Actor given to function: " << std::to_string(old_actor->getUID()))
}

void Theatre::actorLeave(long uid)
{
    if(!objects.contains(uid))
    {
        PRINTERR("Theatre was requested to delete the Actor with UID " << std::to_string(uid) << " but that UID does not exist!")
        return;
    }

    if(objects.at(uid)->getType() == graphx::classes::GRAPHXPLAYER)
        player_uid = -1;

    if(auto it = objects.find(uid) ; it != objects.end())
    {
        it->second = NULL;
        objects.erase(it);
        int i = 0;
        for(auto iter = troupe.begin() ; iter != troupe.end() ; iter++,i++)
        {
            if(troupe[i]->getUID() == uid)
            {
                troupe[i] = NULL;
                troupe.erase(iter);
            }
        }

        countLights();
        time_to_store_buffers = time_to_render;
        return;
    }

    PRINTERR("Request to remove an Actor with UID " << std::to_string(uid) << " failed!")
}

void Theatre::placeDevice(Device *new_device, long uid, graphx::gSettings new_settings)
{
    if(devices.contains(uid))
    {
        PRINTERR("Tried adding a new Device with UID " << std::to_string(uid) << " to Theatre " << name << " but a Device with that UID already exists! Aborting addition of this Device! If there are problems or crashes, this may be the cause!")
        return;
    }

    devices[uid] = new_device;
    devices.at(uid)->setUID(uid);

    if(devices.at(uid)->getType() == graphx::classes::ENVIRONMENT)
        environment_uid = uid;

    new_device->loadSettings(new_settings);

    if(time_to_render)
        new_device->initialize();
}

void Theatre::removeDevice(Device *old_device)
{
    if(auto it = devices.find(old_device->getUID()) ; it != devices.end())
    {
        if(it->second->getType() == graphx::classes::ENVIRONMENT)
            environment_uid = -1;

        it->second = NULL;
        devices.erase(it);
        return;
    }

    PRINTERR("Request to remove a Device by pointer failed!\n\tUID of Device given to function: " << std::to_string(old_device->getUID()))
}

void Theatre::removeDevice(long uid)
{
    if(!devices.contains(uid))
    {
        PRINTERR("Theatre was requested to delete the Device with UID " << std::to_string(uid) << " but that UID does not exist!")
        return;
    }

    if(auto it = devices.find(uid) ; it != devices.end())
    {
        if(it->second->getType() == graphx::classes::ENVIRONMENT)
            environment_uid = -1;

        it->second = NULL;
        devices.erase(it);
        return;
    }

    PRINTERR("Request to remove a Device with UID " << std::to_string(uid) << " failed!")
}

Actor *Theatre::getActor(long actor_uid)
{
    if(objects.contains(actor_uid))
        return objects.at(actor_uid);

    PRINTERR("Hey! Someone asked for an Actor with the UID " << std::to_string(actor_uid) << ", but none were found! The \"getActor\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
    return nullptr;
}

Actor *Theatre::getActor(std::string actor_name)
{
    for(auto &pair : objects)
    {
        if(!pair.second->getName().compare(actor_name))
            return pair.second;
    }

    PRINTERR("Hey! Someone asked for an Actor named " << actor_name << ", but none were found! The \"getActor\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
    return nullptr;
}

Device *Theatre::getDevice(long device_uid)
{
    if(devices.contains(device_uid))
        return devices.at(device_uid);

    PRINTERR("Hey! Someone asked for a Device with the UID " << std::to_string(device_uid) << ", but none were found! The \"getDevice\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
    return nullptr;
}

Device *Theatre::getDevice(std::string device_name)
{
    for(auto &pair : devices)
        if(!pair.second->getName().compare(device_name))
            return pair.second;

    PRINTERR("Hey! Someone asked for a Device named " << device_name << ", but none were found! The \"getDevice\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
    return nullptr;
}

GraphXPlayer *Theatre::getPlayer()
{
    if(player_uid == -1)
    {
        for(auto &pair : objects)
        {
            if(pair.second->getType() == graphx::classes::GRAPHXPLAYER)
            {
                player_uid = pair.first;
                return static_cast<GraphXPlayer *>(pair.second);
            }
        }
    }

    if(objects.contains(player_uid))
        return static_cast<GraphXPlayer *>(objects.at(player_uid));

    return nullptr;
}

Environment *Theatre::getEnvironment()
{
    if(environment_uid == -1)
    {
        for(auto &pair : devices)
        {
            if(pair.second->getType() == graphx::classes::ENVIRONMENT)
            {
                environment_uid = pair.first;
                return static_cast<Environment *>(pair.second);
            }
        }

        return new Environment();
    }

    if(devices.contains(environment_uid))
        return static_cast<Environment *>(devices.at(environment_uid));

    return nullptr;
}

void Theatre::sortTroupe()
{
    std::sort(troupe.begin(), troupe.end(), [](Actor *left, Actor *right)
    {
        return (graphx::classes::isLight(left->getType()) > graphx::classes::isLight(right->getType()));
    });
}

void Theatre::countLights()
{
    for(Actor *actor : troupe)
    {
        if(!graphx::classes::isLight(actor->getType())) // Keep an eye on this... (see notes @ [03/28/25])
            continue;

        if(static_cast<Light *>(actor)->isLightType(graphx::classes::LIGHTSPOT))
        {
            spot_lights_count++;
            continue;
        }

        else if(static_cast<Light *>(actor)->isLightType(graphx::classes::LIGHTDIRECTIONAL))
        {
            directional_lights_count++;
            continue;
        }

        point_lights_count++;
    }
}