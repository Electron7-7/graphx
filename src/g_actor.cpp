#include "g_actor.hpp"
#include "graphx_classes_namespace.hpp"
#include <gmath.hpp>
using namespace graphx;

glm::vec3 vector3_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 vector3_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 vector3_right = glm::vec3(1.0f, 0.0f, 0.0f);

//------
// Actor
//------
Actor::Actor(const graphx::gClass* my_type, const graphx::gUID& my_uid, const graphx::gSettings& my_settings)
: type(my_type), uid(my_uid), settings(my_settings)
{
	RenderState render_state(position_global + position_local, quaternion_global * quaternion_local, scale_global * scale_local);
	current_state_buffer = { render_state, render_state };
	previous_state_buffer = { render_state, render_state };
	updateVectors();
}

Actor::Actor(const graphx::gClass* my_type, const int my_id, const std::string& my_name, const graphx::gSettings& my_settings)
: Actor(my_type, graphx::gUID(my_id, my_name), my_settings)
{}

Actor::Actor(const int my_id, const std::string& my_name, const graphx::gSettings& my_settings)
: Actor(&graphx::classes::ACTOR, my_id, my_name, my_settings)
{}

Actor::Actor(const graphx::gUID& my_id, const graphx::gSettings& my_settings)
: Actor(&graphx::classes::ACTOR, my_id, my_settings)
{}

Actor::~Actor()
{
	mesh->~Mesh();
	delete mesh;
	collider->~Collider();
	delete collider;
}

graphx::gUID Actor::getUID() const
{ return uid; }

void Actor::setName(const std::string& new_name)
{ uid.name = new_name; }

void Actor::debug_highlight(const bool turn_highlight_on)
{ debug_highlight_color = glm::vec4(type->debugging_color, 0.3f * turn_highlight_on); }

// Get/Set Global/Local Position/Rotation/Quaternion
glm::vec3 Actor::getGlobalPosition() const
{ return position_global; }

glm::vec3 Actor::getGlobalRotationAngles(const bool degrees_instead_of_radians) const
{ return (degrees_instead_of_radians) ? (glm::degrees(glm::eulerAngles(quaternion_global))) : glm::eulerAngles(quaternion_global); }

glm::quat Actor::getGlobalQuaternion() const
{ return quaternion_global; }

glm::vec3 Actor::getLocalPosition() const
{ return position_local; }

glm::vec3 Actor::getLocalRotationAngles(const bool degrees_instead_of_radians) const
{ return (degrees_instead_of_radians) ? (glm::degrees(glm::eulerAngles(quaternion_local))) : glm::eulerAngles(quaternion_local); }

glm::quat Actor::getLocalQuaternion() const
{ return quaternion_local; }

void Actor::setGlobalPosition(const glm::vec3& new_position)
{ position_global = new_position; }

void Actor::setGlobalRotationAngles(const glm::vec3& new_rotation, const bool degrees_instead_of_radians)
{ quaternion_global = (degrees_instead_of_radians) ? (glm::quat(glm::radians(new_rotation))) : glm::quat(new_rotation); }

void Actor::setGlobalQuaternion(const glm::quat& new_quaternion)
{ quaternion_global = new_quaternion; }

void Actor::setLocalPosition(const glm::vec3& new_position)
{ position_local = new_position; }

void Actor::setLocalRotationAngles(const glm::vec3& new_rotation, const bool degrees_instead_of_radians)
{ quaternion_local = (degrees_instead_of_radians) ? (glm::quat(glm::radians(new_rotation))) : glm::quat(new_rotation); }

void Actor::setLocalQuaternion(const glm::quat& new_quaternion)
{ quaternion_local = new_quaternion; }

void Actor::selfOverrideColliderTransform(const bool ignore_scale)
{
	if(!givesAFuckAboutPhysics()) return;

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

	JPH::BodyInterface& body_interface = jolt_physics_system.GetBodyInterface();
	setGlobalPosition(gmath::convertMath<glm::vec3>(body_interface.GetPosition(collider->getBodyID())));
	setGlobalQuaternion(gmath::convertMath<glm::quat>(body_interface.GetRotation(collider->getBodyID())));
	if(ignore_scale) return;
	// I don't know a nice way of getting the scale from the collider and I don't wanna find it right now
	// A note about collider scale: it's not a simple scale value, as much as it's a complex shape; a scale value would affect the shape like a cube, which may work sometimes and may be strange other times
}

const bool Actor::givesAFuckAboutPhysics() const
{ return !(collider == nullptr || collider->getBodyID().IsInvalid()); }

void Actor::loadSettings(const graphx::gSettings& new_settings)
{
	if(settings == graphx::empty_settings)
		settings = new_settings;

	glm::vec3 local_euler_degrees = getLocalRotationAngles(true);
	glm::vec3 global_euler_degrees = getGlobalRotationAngles(true);

	getSetting(uid.name, settings["Name"]);
	getSetting(mesh, settings["Mesh"]);
	getSetting(position_global, settings["Position"]);
	getSetting(position_local, settings["LocalPosition"]);
	getSetting(global_euler_degrees, settings["Rotation"]);
	getSetting(local_euler_degrees, settings["LocalRotation"]);
	getSetting(scale_global, settings["Scale"]);
	getSetting(scale_local, settings["LocalScale"]);
	getSetting(visible, settings["Visible"]);

	getSetting(collider, settings["Collider"]);

	setLocalQuaternion(glm::quat(glm::radians(local_euler_degrees)));
	setGlobalQuaternion(glm::quat(glm::radians(global_euler_degrees)));

	updateVectors();

	if(collider != nullptr)
		collider->loadSettings();
}

RenderCommands Actor::getRenderCommands()
{
	RenderCommands render_commands;

	render_commands.render_command.current_render_state = &current_state_buffer[state_index];
	render_commands.render_command.previous_render_state = &previous_state_buffer[state_index];
	if(mesh != nullptr && visible && (type != graphx::classes::GRAPHXPLAYER))
	{
		render_commands.render_command.mesh_data_name = mesh->mesh_data_name;
		render_commands.render_command.mesh_material = mesh->material;
	}
	else
	{
		// Todo: change this
		render_commands.render_command.mesh_data_name = ""; // So that RenderCmd::isValid returns false (might wanna make this a bit more sophisticated, later)
	}

	// Debug shit!
	if(graphx::debug::actor_debug_menu_open)
	{
		if(type != graphx::classes::LABEL && visible) // Labels shouldn't have debug labels imho
		{   // Todo: idk I just don't like how Actor interfaces directly with R_BufferRenderCmd, but this *is* a debug function, so... idk
			TextRenderCmd text_command;
			text_command.font_name = "Verdana";
			text_command.text = std::string("Name: " + uid.name + "\nType: " + std::string(type->name) + "\nUID: " + std::to_string(uid.id));
			text_command.color = type->debugging_color;
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

void Actor::processMouse(GLFWwindow *window, double x_position_in, double y_position_in)
{}

void Actor::processInput(GLFWwindow *window)
{}

void Actor::processKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{}

void Actor::updateVectors()
{
	orientation_up = getGlobalQuaternion() * getLocalQuaternion() * vector3_up;
	orientation_front = getGlobalQuaternion() * getLocalQuaternion() * vector3_front;
	orientation_right = getGlobalQuaternion() * getLocalQuaternion() * vector3_right;
	orientation_grounded_front = glm::vec3(orientation_front[0], 0.0f, orientation_front[2]);
}

void Actor::updateStates(std::mutex &state_mutex)
{
	std::lock_guard guard(state_mutex);

	// Copy current state into previous state
	previous_state_buffer[state_index] = current_state_buffer[state_index];

	// Update current state
	current_state_buffer[state_index].render_position	=	getGlobalPosition() + getLocalPosition();
	current_state_buffer[state_index].render_quaternion	=	getGlobalQuaternion() * getLocalQuaternion();
	current_state_buffer[state_index].render_scale		=	getGlobalScale() + getLocalScale();

	// Flip state buffer
	state_index = 1 - state_index;
}

void Actor::tick(int current_tick)
{
	// Opted to not use an early return here, since that could be nasty for any derived Actor that overrides this function but still calls Actor::tick
	if(givesAFuckAboutPhysics() && !collider->overrides_actor_transform)
	{
		JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
		JPH::Vec3 body_position = body_interface.GetCenterOfMassPosition(collider->getBodyID());
		JPH::Quat body_quaternion = body_interface.GetRotation(collider->getBodyID());
		setGlobalPosition(gmath::convertMath<glm::vec3>(body_position));
		setGlobalQuaternion(gmath::convertMath<glm::quat>(body_quaternion));
		updateVectors();
	}
}

