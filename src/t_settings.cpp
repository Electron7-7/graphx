#include "t_settings.hpp"
#include "g_actor.hpp"
#include "g_device.hpp"
#include "g_theatre.hpp"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <algorithm>

//---------------
// ActorReference
//---------------
ActorReference::ActorReference(const int my_uid, Theatre* my_parent_theatre)
: uid(my_uid), name(""), parent_theatre(my_parent_theatre)
{}

ActorReference::ActorReference(const std::string& my_name, Theatre* my_parent_theatre)
: uid(-1), name(my_name), parent_theatre(my_parent_theatre)
{}

bool ActorReference::isValid() const
{ return (uid != -1 || !name.empty()); }

Actor* ActorReference::getPointer() const
{
    if(uid != -1)
        return parent_theatre->getActor(uid);

    if(!name.empty())
        return parent_theatre->getActor(name);

    PRINTERR("in ActorReference::getPointer() - both uid and name are invalid!! Returning nullptr!!")
    return nullptr;
}

//----------------
// DeviceReference
//----------------
DeviceReference::DeviceReference(const int my_uid, Theatre* my_parent_theatre)
: uid(my_uid), name(""), parent_theatre(my_parent_theatre)
{}

DeviceReference::DeviceReference(const std::string& my_name, Theatre* my_parent_theatre)
: uid(-1), name(my_name), parent_theatre(my_parent_theatre)
{}

bool DeviceReference::isValid() const
{ return (uid != -1 || !name.empty()); }

Device* DeviceReference::getPointer() const
{
    if(uid != -1)
        return parent_theatre->getDevice(uid);

    if(!name.empty())
        return parent_theatre->getDevice(name);

    PRINTERR("in DeviceReference::getPointer() - both uid and name are invalid!! Returning nullptr!!")
    return nullptr;
}


template<typename V>
void gSettings::getNumber(const std::string& setting, V& variable) const
{
    if(!raw_data.contains(setting) || raw_data.at(setting).size() == 0)
        return;

    // Sorry, John Carmack, but I felt the need to write some overly descriptive comments
    if constexpr(std::is_same_v<V, glm::vec2> || std::is_same_v<V, glm::vec3> || std::is_same_v<V, glm::vec4> || std::is_same_v<V, glm::quat>)
    {
        // We already know the gRawData vector exists and isn't empty,
        // so we can take advantage of glm swizzling if the vector only
        // has one number, and do an early return
        if(raw_data.at(setting).size() == 1)
        {
            // Unfortunately, quaternions can't be swizzled exactly the same, so I assume this is user error and use the default constructor instead
            if constexpr(std::is_same_v<V, glm::quat>)
            {
                variable = V();
                return;
            }
            else
            {
                variable = V(std::stof(raw_data.at(setting).at(0)));
                return;
            }
        }

        // Make the rest of the code easier to read
        const gRawData& raw_data_copy = raw_data.at(setting);
        variable = V();

        // Fill the rest of the glm variable
        for(int i = 0; i < raw_data_copy.size(); i++)
            variable[i] = std::stof(raw_data_copy.at(i));

        return;
    }

    // Get the gRawData vector if it's not empty and create a new one with a single "0" if it is
    const gRawData& raw_data_copy = (raw_data.at(setting).size() > 0) ? raw_data.at(setting) : gRawData{"0"};

    if constexpr(std::is_same_v<V, int>)    { variable = std::stoi(raw_data_copy.at(0)); return; }
    if constexpr(std::is_same_v<V, long>)   { variable = std::stol(raw_data_copy.at(0)); return; }
    if constexpr(std::is_same_v<V, float>)  { variable = std::stof(raw_data_copy.at(0)); return; }
    if constexpr(std::is_same_v<V, double>) { variable = std::stod(raw_data_copy.at(0)); return; }

    // Defaults for anything else:
    if constexpr(std::is_integral_v<V>)       { variable = std::stol(raw_data_copy.at(0)); return; }
    if constexpr(std::is_floating_point_v<V>) { variable = std::stod(raw_data_copy.at(0)); return; }

    PRINTERR("in gSettings::getNumeric - unexpected input type!")
}

template void gSettings::getNumber(const std::string&, glm::vec2&) const;
template void gSettings::getNumber(const std::string&, glm::vec3&) const;
template void gSettings::getNumber(const std::string&, glm::vec4&) const;
template void gSettings::getNumber(const std::string&, glm::quat&) const;
template void gSettings::getNumber(const std::string&, int&) const;
template void gSettings::getNumber(const std::string&, long&) const;
template void gSettings::getNumber(const std::string&, unsigned int&) const;
template void gSettings::getNumber(const std::string&, unsigned long&) const;
template void gSettings::getNumber(const std::string&, float&) const;
template void gSettings::getNumber(const std::string&, double&) const;

void gSettings::getBoolean(const std::string& setting, bool& variable) const
{
    if(!raw_data.contains(setting) || raw_data.at(setting).size() == 0) return;

    std::string raw_data_lower_case = raw_data.at(setting).at(0);
    std::transform(raw_data_lower_case.begin(), raw_data_lower_case.end(), raw_data_lower_case.begin(), [](unsigned char c) { return std::tolower(c); });

    variable = raw_data_lower_case.compare("false");
}

void gSettings::getString(const std::string& setting, std::string& variable) const
{
    if(!raw_data.contains(setting) || raw_data.at(setting).size() == 0) return;
    variable = "";
    for(int i = 0; i < raw_data.at(setting).size(); i++)
        variable += raw_data.at(setting).at(i);
}

void gSettings::getResource(const std::string& setting, std::string& variable) const
{
    if(external_reference.contains(setting))
    {
        variable = external_reference.at(setting);
        return;
    }

    if(cpp_reference.contains(setting) && cpp_reference.at(setting).type() == typeid(std::string))
    {
        variable = std::any_cast<std::string>(cpp_reference.at(setting));
        return;
    }
}

// void gSettings::getActor(const std::string& setting, Actor*& variable) const
// {
//     if(!actor_reference.contains(setting)) return;
//     variable = actor_reference.at(setting);
// }

// void gSettings::getDevice(const std::string& setting, Device*& variable) const
// {
//     if(!device_reference.contains(setting)) return;
//     variable = device_reference.at(setting);
// }

void gSettings::configureBaseVariables(Actor* actor)
{
    const gSettings& settings = actor->getSettings();
    // glm::vec3 position_global = actor->getGlobalPosition();
    // glm::vec3 position_local = actor->getLocalPosition();
    // glm::vec3 global_euler_degrees = actor->getGlobalEulerAngles(true);
    // glm::vec3 local_euler_degrees = actor->getLocalEulerAngles(true);
    // glm::vec3 scale_global = actor->getGlobalScale();
    // glm::vec3 scale_local = actor->getLocalScale();

    // settings.getString("Name", actor->name);
    // settings.getBoolean("Visible", actor->visible);
    // settings.getDevice("Model", actor->model_uid);
    // settings.getDevice("Collider", actor->collider_uid);
    // settings.getNumber("Position", position_global);
    // settings.getNumber("LocalPosition", position_local);
    // settings.getNumber("Rotation", global_euler_degrees);
    // settings.getNumber("LocalRotation", local_euler_degrees);
    // settings.getNumber("Scale", scale_global);
    // settings.getNumber("LocalScale", scale_local);

    // actor->setGlobalPosition(position_global);
    // actor->setLocalPosition(position_local);
    // actor->setGlobalEulerAngles(global_euler_degrees, true);
    // actor->setLocalEulerAngles(local_euler_degrees, true);
    // actor->setGlobalScale(scale_global);
    // actor->setLocalScale(scale_local);


    // TODO: DELETE ALL OF THIS ONCE I DON'T NEED IT AND CAN USE THE ABOVE STUFF
    glm::vec3 local_euler_degrees = glm::vec3(0.0f);
    glm::vec3 global_euler_degrees = glm::degrees(glm::eulerAngles(actor->quaternion));
    settings.getString("Name", actor->name);
    settings.getDevice("Mesh", actor->mesh);
    settings.getNumber("Position", actor->position_global);
    settings.getNumber("LocalPosition", actor->position_local);
    settings.getNumber("Rotation", global_euler_degrees);
    settings.getNumber("LocalRotation", local_euler_degrees);
    settings.getNumber("Scale", actor->scale);
    settings.getBoolean("Visible", actor->visible);
    actor->local_quaternion = glm::quat(glm::radians(local_euler_degrees));
    actor->quaternion = glm::quat(glm::radians(global_euler_degrees));
}

void gSettings::configureBaseVariables(Device* device)
{
    const gSettings& settings = device->getSettings();

    settings.getString("Name", device->name);
}