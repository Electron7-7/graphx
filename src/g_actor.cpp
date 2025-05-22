#include "g_actor.hpp"
#include "g_device.hpp"
#include "g_theatre.hpp"
#include "r_common.hpp"
#include "g_devices.hpp"
#include <gmath.hpp>

using namespace graphx;

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
    // actor_uid = (parent_theatre != nullptr) ? parent_theatre->changeActorUID(actor_uid, new_uid) : new_uid;
    actor_uid = new_uid;
}

int Actor::getUID() const
{ return actor_uid; }

gSettings Actor::getSettings() const
{ return settings; }

void Actor::setSettings(const gSettings& new_settings)
{
    settings = new_settings;
}

void Actor::updateStates(std::mutex &state_mutex)
{
    std::lock_guard guard(state_mutex);

    // Copy current state into previous state
    previous_state_buffer[state_index] = current_state_buffer[state_index];

    // Update current state
    current_state_buffer[state_index].render_position   =   getGlobalPosition() + getLocalPosition();
    current_state_buffer[state_index].render_quaternion =   getGlobalQuaternion() * getLocalQuaternion();
    current_state_buffer[state_index].render_scale      =   getGlobalScale() + getLocalScale();

    // Flip state buffer
    state_index = 1 - state_index;
}


// Get Absolute Position/Rotation/Quaternion
glm::vec3 Actor::getAbsolutePosition() const
{ return (getGlobalPosition() + getLocalPosition()); }

glm::quat Actor::getAbsoluteQuaternion() const
{ return (getGlobalQuaternion() * getLocalQuaternion()); }

glm::vec3 Actor::getAbsoluteEulerAngles(const bool use_degrees) const
{ return (getGlobalEulerAngles(use_degrees) + getLocalEulerAngles(use_degrees)); }

glm::vec3 Actor::getAbsoluteScale() const
{ return (getGlobalScale() + getLocalScale()); }


// Get Orientation Up/Front/Right
glm::vec3 Actor::getOrientationUp(const bool global) const
{ return (global) ? (quaternion_global * graphx::orientation::up) : (quaternion_global * quaternion_local * graphx::orientation::up); }

glm::vec3 Actor::getOrientationFront(const bool global) const
{ return (global) ? (quaternion_global * graphx::orientation::front) : (quaternion_global * quaternion_local * graphx::orientation::front); }

glm::vec3 Actor::getOrientationRight(const bool global) const
{ return (global) ? (quaternion_global * graphx::orientation::right) : (quaternion_global * quaternion_local * graphx::orientation::right); }


// Get Global/Local Position/Rotation/Quaternion
glm::vec3 Actor::getGlobalPosition() const
{ return position_global; }

glm::quat Actor::getGlobalQuaternion() const
{ return quaternion_global; }

glm::vec3 Actor::getGlobalEulerAngles(const bool use_degrees) const
{ return (use_degrees) ? (glm::degrees(glm::eulerAngles(quaternion_global))) : glm::eulerAngles(quaternion_global); }

glm::vec3 Actor::getLocalPosition() const
{ return position_local; }

glm::quat Actor::getLocalQuaternion() const
{ return quaternion_local; }

glm::vec3 Actor::getLocalEulerAngles(const bool use_degrees) const
{ return (use_degrees) ? (glm::degrees(glm::eulerAngles(quaternion_local))) : glm::eulerAngles(quaternion_local); }

glm::vec3 Actor::getGlobalScale() const
{ return scale_global; }

glm::vec3 Actor::getLocalScale() const
{ return scale_local; }


// Set Global/Local Position/Rotation/Quaternion
void Actor::setGlobalPosition(const glm::vec3& new_position)
{ position_global = new_position; }

void Actor::setGlobalQuaternion(const glm::quat& new_quaternion)
{ quaternion_global = new_quaternion; }

void Actor::setGlobalEulerAngles(const glm::vec3& new_rotation, const bool degrees_instead_of_radians)
{ quaternion_global = (degrees_instead_of_radians) ? (glm::quat(glm::radians(new_rotation))) : glm::quat(new_rotation); }

void Actor::setGlobalPitch(const float new_pitch, const bool use_degrees)
{ setGlobalEulerAngles(glm::vec3(new_pitch, getGlobalEulerAngles(use_degrees)[1], getGlobalEulerAngles(use_degrees)[2]), use_degrees); }

void Actor::setGlobalYaw(const float new_yaw, const bool use_degrees)
{ setGlobalEulerAngles(glm::vec3(getGlobalEulerAngles(use_degrees)[0], new_yaw, getGlobalEulerAngles(use_degrees)[2]), use_degrees); }

void Actor::setGlobalRoll(const float new_roll, const bool use_degrees)
{ setGlobalEulerAngles(glm::vec3(getGlobalEulerAngles(use_degrees)[0], getGlobalEulerAngles(use_degrees)[1], new_roll), use_degrees); }

void Actor::setGlobalScale(const glm::vec3& new_scale)
{ scale_global = new_scale; }

void Actor::setLocalPosition(const glm::vec3& new_position)
{ position_local = new_position; }

void Actor::setLocalQuaternion(const glm::quat& new_quaternion)
{ quaternion_local = new_quaternion; }

void Actor::setLocalEulerAngles(const glm::vec3& new_rotation, const bool degrees_instead_of_radians)
{ quaternion_local = (degrees_instead_of_radians) ? (glm::quat(glm::radians(new_rotation))) : glm::quat(new_rotation); }

void Actor::setLocalPitch(const float new_pitch, const bool use_degrees)
{ setLocalEulerAngles(glm::vec3(new_pitch, getLocalEulerAngles(use_degrees)[1], getLocalEulerAngles(use_degrees)[2]), use_degrees); }

void Actor::setLocalYaw(const float new_yaw, const bool use_degrees)
{ setLocalEulerAngles(glm::vec3(getLocalEulerAngles(use_degrees)[0], new_yaw, getLocalEulerAngles(use_degrees)[2]), use_degrees); }

void Actor::setLocalRoll(const float new_roll, const bool use_degrees)
{ setLocalEulerAngles(glm::vec3(getLocalEulerAngles(use_degrees)[0], getLocalEulerAngles(use_degrees)[1], new_roll), use_degrees); }

void Actor::setLocalScale(const glm::vec3& new_scale)
{ scale_local = new_scale; }


void Actor::tick(int current_tick)
{}

void Actor::loadSettings()
{ gSettings::configureBaseVariables(this); }

RenderCommands Actor::getRenderCommands()
{

    RenderCommands render_commands;

    if(!visible)
    {
        // Todo: change this
        render_commands.render_command.mesh_data_name = ""; // So that RenderCmd::isValid returns false (might wanna make this a bit more sophisticated, later)
        return render_commands;
    }

    render_commands.render_command.mesh_data_name = mesh->mesh_data_name;
    render_commands.render_command.mesh_material = *mesh->material;

    render_commands.render_command.current_render_state = &current_state_buffer.at(state_index);
    render_commands.render_command.previous_render_state = &previous_state_buffer.at(state_index);

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
            text_command.render_state = &current_state_buffer.at(state_index);
            text_command.position_y = -25.0f;
            text_command.position_x = 50.0f;
            text_command.is_debug_label = true;

            R_BufferRenderCmd(text_command);
        }

        render_commands.render_command.debug_highlight_color = debug_highlight_color;
    }

    return(render_commands);
}

void Actor::processMouse(GLFWwindow *window, double x_position_in, double y_position_in)
{}

void Actor::processKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{}

void Actor::checkForInput(GLFWwindow* window)
{}

bool Actor::isPhysicsActor() const
{ return false; }

void Actor::callToStage(Theatre *parent_theatre)
{}

void Actor::takeABow()
{}

std::string Actor::getTypeName() const
{ return "Actor"; }

/*void Actor::selfOverrideColliderTransform(const bool ignore_scale)
{
    if(!givesAFuckAboutPhysics()) return;

    std::shared_ptr<Collider> collider = parent_theatre->getDevice<Collider>(collider_uid);

    JPH::BodyInterface& body_interface = jolt_physics_system.GetBodyInterface();
    JPH::Vec3 position = gmath::convertMath<JPH::Vec3>(getGlobalPosition() * getLocalPosition());
    JPH::Quat quaternion = gmath::convertMath<JPH::Quat>(getGlobalQuaternion() * getLocalQuaternion());
    body_interface.SetPositionAndRotation(collider->getBodyID(), position, quaternion, JPH::EActivation::Activate);

    if(ignore_scale) return; // The default, because changing a collider's scale is costly-ish
    // A note about collider scale: it's not a simple scale value, as much as it's a complex shape; a scale value would affect the shape like a cube, which may work sometimes and may be strange other times
    JPH::Vec3 scale = gmath::convertMath<JPH::Vec3>(getGlobalScale() * getLocalScale());
    body_interface.GetShape(collider->getBodyID())->ScaleShape(scale); // I think this is correct...
}

void Actor::colliderOverrideSelfTransform(const bool ignore_scale)
{
    if(!givesAFuckAboutPhysics()) return;

    std::shared_ptr<Collider> collider = parent_theatre->getDevice<Collider>(collider_uid);

    JPH::BodyInterface& body_interface = jolt_physics_system.GetBodyInterface();
    setGlobalPosition(gmath::convertMath<glm::vec3>(body_interface.GetPosition(collider->getBodyID())));
    setGlobalQuaternion(gmath::convertMath<glm::quat>(body_interface.GetRotation(collider->getBodyID())));
    if(ignore_scale) return;
    // I don't know a nice way of getting the scale from the collider and I don't wanna find it right now
}*/

bool Actor::canBeRendered() const
{ return (visible && mesh != nullptr); }