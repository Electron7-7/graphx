#include "g_actor.hpp"
#include "g_device.hpp"
#include "g_theatre.hpp"
#include "r_common.hpp"
#include "g_devices.hpp"
#include <gmath.hpp>

using namespace graphx;

glm::vec3 vector3_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 vector3_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 vector3_right = glm::vec3(1.0f, 0.0f, 0.0f);

//
// Actor
//
Actor::Actor(const std::string& my_name)
: name(my_name), parent_theatre(nullptr), settings(gSettings()), actor_uid(-1)
{}

Actor::Actor(Theatre* my_parent_theatre, const int my_uid, const gSettings& my_settings)
: name("Untitled Actor"), parent_theatre(my_parent_theatre), settings(my_settings), actor_uid(my_uid)
{}

Actor::~Actor() = default;

void Actor::setUID(const int new_uid)
{
    actor_uid = new_uid; // Note: this is dead simple right now, but more changes in the future will bring this back to not being redundant
}

long Actor::getUID() const
{
    return actor_uid;
}

gSettings Actor::getSettings() const
{
    return settings;
}

void Actor::setSettings(const gSettings& new_settings)
{
    settings = new_settings;
}

void Actor::highlightMe()
{
    debug_highlight_color = glm::vec4(1.0f, 1.0f, 1.0f, 0.25f);
}

void Actor::unHighlightMe()
{
    debug_highlight_color = glm::vec4(0.0f);
}

template<> std::vector<float> Actor::getPosition()
{
    return {position_global.x + position_local.x, position_global.y + position_local.y, position_global.z + position_local.z};
}

template<> glm::vec3 Actor::getPosition()
{
    return position_global + position_local;
}

template<> JPH::Vec3 Actor::getPosition()
{
    return gmath::convertMath<JPH::Vec3>(position_global) + gmath::convertMath<JPH::Vec3>(position_local);
}

template<> std::vector<float> Actor::getRotation()
{
    glm::vec3 the_euler_angles = glm::eulerAngles(quaternion * local_quaternion);
    return {the_euler_angles.x, the_euler_angles.y, the_euler_angles.z};
}

template<> glm::vec3 Actor::getRotation()
{
    return glm::eulerAngles(quaternion * local_quaternion);
}

template<> JPH::Vec3 Actor::getRotation()
{
    return gmath::convertMath<JPH::Vec3>(glm::eulerAngles(quaternion * local_quaternion));
}

template<> glm::quat Actor::getRotation()
{
    return quaternion * local_quaternion;
}

template<> JPH::Quat Actor::getRotation()
{
    return gmath::convertMath<JPH::Quat>(quaternion) * gmath::convertMath<JPH::Quat>(local_quaternion);
}

template<> std::vector<float> Actor::getRotationDegrees()
{
    glm::vec3 the_euler_angles = glm::degrees(glm::eulerAngles(quaternion * local_quaternion));
    return {the_euler_angles.x, the_euler_angles.y, the_euler_angles.z};
}

void Actor::setGlobalPosition(glm::vec3 new_value)
{
    position_global = new_value;
}

void Actor::setGlobalPosition(JPH::Vec3 new_value)
{
    position_global = gmath::convertMath<glm::vec3>(new_value);
}

void Actor::setGlobalRotation(glm::quat new_value)
{
    quaternion = new_value;
}

void Actor::setGlobalRotation(JPH::Quat new_value)
{
    quaternion = gmath::convertMath<glm::quat>(new_value);
}

void Actor::setGlobalRotation(glm::vec3 new_value)
{
    quaternion = glm::quat(new_value);
}

void Actor::setGlobalRotation(JPH::Vec3 new_value)
{
    quaternion = glm::quat(gmath::convertMath<glm::vec3>(new_value));
}

void Actor::setLocalPosition(glm::vec3 new_value)
{
    position_local = new_value;
}

void Actor::setLocalPosition(JPH::Vec3 new_value)
{
    position_local = gmath::convertMath<glm::vec3>(new_value);
}

void Actor::setLocalRotation(glm::quat new_value)
{
    local_quaternion = new_value;
}

void Actor::setLocalRotation(JPH::Quat new_value)
{
    local_quaternion = gmath::convertMath<glm::quat>(new_value);
}

void Actor::setLocalRotation(glm::vec3 new_value)
{
    local_quaternion = glm::quat(new_value);
}

void Actor::setLocalRotation(JPH::Vec3 new_value)
{
    local_quaternion = glm::quat(gmath::convertMath<glm::vec3>(new_value));
}

RenderCommands Actor::getRenderCommands()
{
    RenderCommands render_commands;

    render_commands.render_command.current_render_state = &current_state_buffer[state_index];
    render_commands.render_command.previous_render_state = &previous_state_buffer[state_index];
    if(mesh != nullptr && visible)
    {
        render_commands.render_command.mesh_data_name = mesh->mesh_data_name;
        render_commands.render_command.mesh_material = *mesh->material;
    }
    else
    {
        // Todo: change this
        render_commands.render_command.mesh_data_name = ""; // So that RenderCmd::isValid returns false (might wanna make this a bit more sophisticated, later)
    }

    // Debug shit!
    if(graphx::debug::actor_debug_menu_open)
    {
        if(visible)
        {   // Todo: idk I just don't like how Actor interfaces directly with R_BufferRenderCmd, but this *is* a debug function, so... idk
            TextRenderCmd text_command;
            text_command.font_name = "Verdana";
            text_command.text = std::string("Name: " + name + "\nType: " + std::string(name) + "\nUID: " + std::to_string(actor_uid));
            text_command.color = glm::vec3(1.0f);
            text_command.scale = graphx::debug::actor_debug_menu_text_scale;
            text_command.render_state = &current_state_buffer[state_index];
            text_command.position_y = -25.0f;
            text_command.position_x = 50.0f;
            text_command.is_debug_label = true;

            R_BufferRenderCmd(text_command);
        }

        render_commands.render_command.debug_highlight_color = debug_highlight_color;
    }

    return(render_commands);
}

bool Actor::isPhysicsActor()
{
    return false;
}

void Actor::youGotACallBack()
{
    gSettings::configureBaseVariables(this);

    updateVectors();
}

void Actor::processMouse(GLFWwindow *window, double x_position_in, double y_position_in)
{}

void Actor::processInput(GLFWwindow *window)
{}

void Actor::processKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{}

void Actor::updateVectors()
{
    orientation_up = quaternion * vector3_up;
    orientation_front = quaternion * vector3_front;
    orientation_right = quaternion * vector3_right;
    orientation_grounded_front = glm::vec3(orientation_front[0], 0.0f, orientation_front[2]);
}

void Actor::updateStates(std::mutex &state_mutex)
{
    std::lock_guard guard(state_mutex);

    // Copy current state into previous state
    previous_state_buffer[state_index] = current_state_buffer[state_index];

    // Update current state
    current_state_buffer[state_index].render_position   =   getPosition<glm::vec3>();
    current_state_buffer[state_index].render_quaternion =   getRotation<glm::quat>();
    current_state_buffer[state_index].render_scale      =   scale;

    // Flip state buffer
    state_index = 1 - state_index;
}

void Actor::tick(int current_tick)
{}

void Actor::callToStage(Theatre *parent_theatre)
{}

void Actor::takeABow()
{
    if(mesh != nullptr)
        mesh->prepForDestruction();
}