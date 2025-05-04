#include "t_settings.hpp"
#include "g_actor.hpp"
#include "g_device.hpp"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <algorithm>

template<>
void gSettings::getRawData<bool>(const std::string& setting, bool& variable) const
{
    if(!raw_data.contains(setting) || raw_data.at(setting).size() == 0) return;

    std::string raw_data_lower_case = raw_data.at(setting).at(0);
    std::transform(raw_data_lower_case.begin(), raw_data_lower_case.end(), raw_data_lower_case.begin(), [](unsigned char c) { return std::tolower(c); });

    variable = raw_data_lower_case.compare("false");
}

template<>
void gSettings::getRawData<std::string>(const std::string& setting, std::string& variable) const
{
    if(!raw_data.contains(setting) || raw_data.at(setting).size() == 0) return;
    variable = "";
    for(int i = 0; i < raw_data.at(setting).size(); i++)
        variable += raw_data.at(setting).at(i);
}

void gSettings::getActor(const std::string& setting, int& variable) const
{
    if(!actor_reference.contains(setting)) return;
    variable = actor_reference.at(setting);
}

void gSettings::getDevice(const std::string& setting, int& variable) const
{
    if(!device_reference.contains(setting)) return;
    variable = device_reference.at(setting);
}

void gSettings::getExternal(const std::string& setting, std::string& variable) const
{
    if(!external_reference.contains(setting)) return;
    variable = external_reference.at(setting);
}

void gSettings::addRawData(const std::string& setting_name, const gRawData& setting)
{
    if(raw_data.contains(setting_name)) return;
    raw_data[setting_name] = setting;
}

void gSettings::addVariable(const std::string& setting_name, std::any setting)
{
    if(cpp_reference.contains(setting_name)) return;
    cpp_reference[setting_name] = setting;
}

void gSettings::addActor(const std::string& setting_name, int setting)
{
    if(actor_reference.contains(setting_name)) return;
    actor_reference[setting_name] = setting;
}

void gSettings::addDevice(const std::string& setting_name, int setting)
{
    if(device_reference.contains(setting_name)) return;
    device_reference[setting_name] = setting;
}

void gSettings::addExternal(const std::string& setting_name, const std::string& setting)
{
    if(external_reference.contains(setting_name)) return;
    external_reference[setting_name] = setting;
}

void configureBaseVariables(Actor* actor)
{
    const gSettings& settings = actor->getSettings();
    glm::vec3 position_global = actor->getGlobalPosition();
    glm::vec3 position_local = actor->getLocalPosition();
    glm::vec3 global_euler_degrees = actor->getGlobalEulerAngles(true);
    glm::vec3 local_euler_degrees = actor->getLocalEulerAngles(true);
    glm::vec3 scale_global = actor->getGlobalScale();
    glm::vec3 scale_local = actor->getLocalScale();

    settings.getRawData("Name", actor->name);
    settings.getRawData("Visible", actor->visible);
    settings.getDevice("Model", actor->model_uid);
    settings.getDevice("Collider", actor->collider_uid);
    settings.getRawData("Position", position_global);
    settings.getRawData("LocalPosition", position_local);
    settings.getRawData("Rotation", global_euler_degrees);
    settings.getRawData("LocalRotation", local_euler_degrees);
    settings.getRawData("Scale", scale_global);
    settings.getRawData("LocalScale", scale_local);

    actor->setGlobalPosition(position_global);
    actor->setLocalPosition(position_local);
    actor->setGlobalEulerAngles(global_euler_degrees, true);
    actor->setLocalEulerAngles(local_euler_degrees, true);
    actor->setGlobalScale(scale_global);
    actor->setLocalScale(scale_local);
}

void configureBaseVariables(Device* device)
{
    const gSettings& settings = device->getSettings();

    settings.getRawData("Name", device->name);
}