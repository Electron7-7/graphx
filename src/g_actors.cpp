#include "g_actor.hpp"
#include "g_actors.hpp"
#include "graphx_classes_namespace.hpp"
#include <gmath.hpp>
#include <models.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

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

//
// Label
//
Label::Label(const graphx::gUID& my_id, const graphx::gSettings& my_settings)
: Actor(&graphx::classes::LABEL, my_id, my_settings), parent(this), text_render_command(TextRenderCmd("Verdana", my_id.name, 0.0f, 0.0f, 1.0f, glm::vec3(0.15f, 0.6f, 0.9f)))
{}

Label::Label(const int my_id_number, const std::string& my_name, const graphx::gSettings& my_settings)
: Label(graphx::gUID(my_id_number, my_name), my_settings)
{}

RenderCommands Label::getRenderCommands()
{
	RenderCommands render_commands = Actor::getRenderCommands();
	render_commands.render_command.mesh_material->color_alpha = label_alpha;
	text_render_command.render_state = &current_state_buffer[state_index];
	text_render_command.position_x = position_global.x;
	text_render_command.position_y = position_global.y;
	render_commands.text_render_command = text_render_command;
	return render_commands;
}

void Label::tick(int current_tick)
{
	if(parent != nullptr)
	{
		setGlobalPosition(parent->getGlobalPosition());
		setGlobalQuaternion(parent->getGlobalQuaternion());
	}
	else
	{
		position_global.x += 0.0f;
	}
}

void Label::loadSettings(const graphx::gSettings& new_settings)
{
	Actor::loadSettings(new_settings);
	/**
	 * `getSetting` Tip:
	 *   When "overloading" `getSetting` settings, I like to make sure that the most verbose/explicit option always
	 *   comes last, making it override all other options. As an example, "TextColor" will always win over "Color",
	 *   and "Label" will always win over both "Message" and "Text". This basically just ensures that should multiple
	 *   versions of the same setting be used, the one that wins is the one that looks more intentional, hopefully
	 *   avoiding confusion.
	*/
	getSetting(label_alpha, settings["Transparency"]);
	getSetting(label_alpha, settings["Alpha"]);
	getSetting(text_render_command.font_name, settings["Font"]);
	getSetting(text_render_command.font_name, settings["FontName"]);
	getSetting(text_render_command.color, settings["Color"]);
	getSetting(text_render_command.color, settings["TextColor"]);
	getSetting(text_render_command.text, settings["Message"]);
	getSetting(text_render_command.text, settings["Text"]);
	getSetting(text_render_command.text, settings["Label"]);
	getSetting(text_render_command.scale, settings["TextScale"]);

	text_render_command.position_x = 0.0f;
	text_render_command.position_y = 0.0f;
}

//
// PhysicsActor
//
PhysicsActor::PhysicsActor(const graphx::gClass* my_class, const graphx::gUID& my_id, const graphx::gSettings& my_settings)
: Actor(my_class, my_id, my_settings)
{}

PhysicsActor::PhysicsActor(const graphx::gUID& my_id, const graphx::gSettings& my_settings)
: PhysicsActor(&graphx::classes::PHYSICSACTOR, my_id, my_settings)
{}

void PhysicsActor::loadSettings(const graphx::gSettings& new_settings)
{
	Actor::loadSettings(new_settings);

	getSetting(mass, settings["Mass"]);
	getSetting(collider, settings["Collider"]);
	collider->loadSettings();

	reset_position = gmath::convertMath<JPH::Vec3>(getGlobalPosition() + getLocalPosition());
	reset_quaternion = gmath::convertMath<JPH::Quat>(getGlobalQuaternion() * getLocalQuaternion());

	reset_to_initial_orientation_for_testing();
}

void PhysicsActor::reset_to_initial_orientation_for_testing()
{
	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	body_interface.SetPositionAndRotation(collider->getBodyID(), reset_position, reset_quaternion, JPH::EActivation::Activate);
	body_interface.SetLinearAndAngularVelocity(collider->getBodyID(), JPH::Vec3::sZero(), JPH::Vec3::sZero());
}

/*//
// RigidBodyActor
//
RigidBodyActor::RigidBodyActor(const graphx::gUID& my_id, const graphx::gSettings& my_settings)
: PhysicsActor(&graphx::classes::RIGIDBODYACTOR, my_id, my_settings)
{}

void RigidBodyActor::loadSettings(const graphx::gSettings& new_settings)
{
	PhysicsActor::loadSettings(new_settings);

	collider->prepForDestruction();
	collider->activation = JPH::EActivation::Activate;
	collider->motion_type = JPH::EMotionType::Dynamic;
	collider->object_layer = Layers::MOVING;
	collider->scale = scale;
	collider->position = position_global;
	collider->local_position = position_local;
	collider->euler_angles = glm::degrees(glm::eulerAngles(quaternion));
	collider->local_euler_angles = glm::degrees(glm::eulerAngles(local_quaternion));
	collider->createBody();
}

void RigidBodyActor::tick(int current_tick)
{
	PhysicsActor::tick(current_tick);
}

void RigidBodyActor::reset_to_initial_orientation_for_testing()
{
	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	body_interface.SetPositionAndRotation(collider->getBodyID(), reset_position, reset_quaternion, JPH::EActivation::Activate);
	body_interface.SetLinearAndAngularVelocity(collider->getBodyID(), JPH::Vec3::sZero(), JPH::Vec3::sZero());
}

void RigidBodyActor::takeABow()
{
	if(collider != nullptr)
		collider->prepForDestruction();
	PhysicsActor::takeABow();
}

//
// StaticBodyActor
//
StaticBodyActor::StaticBodyActor()
: PhysicsActor()
{
	my_type = &graphx::classes::STATICBODYACTOR;
}

void StaticBodyActor::youGotACallBack(graphx::gSettings new_settings)
{
	PhysicsActor::youGotACallBack(new_settings);
}

void StaticBodyActor::callToStage(Theatre *parent_theatre)
{
	PhysicsActor::callToStage(parent_theatre);
	my_type = &graphx::classes::STATICBODYACTOR;
	if(collider == nullptr)
		return;
	collider->prepForDestruction();
	collider = new Collider();
	collider->activation = JPH::EActivation::Activate;
	collider->motion_type = JPH::EMotionType::Static;
	collider->object_layer = Layers::NON_MOVING;
	collider->scale = scale;
	collider->position = position_global;
	collider->local_position = position_local;
	collider->euler_angles = glm::degrees(glm::eulerAngles(quaternion));
	collider->local_euler_angles = glm::degrees(glm::eulerAngles(local_quaternion));
	collider->createBody();
}

void StaticBodyActor::takeABow()
{
	if(collider != nullptr)
		collider->prepForDestruction();
	PhysicsActor::takeABow();
}*/

//
// Camera
//
Camera::Camera()
{
	my_type = &graphx::classes::CAMERA;
}

void Camera::tick(int current_tick)
{}

void Camera::doRotation(glm::vec2 mouse_input)
{
	euler_rotation += euler_rotation_local;
	euler_rotation[0] -= glm::radians(mouse_input[1]);
	euler_rotation[1] -= glm::radians(mouse_input[0]);

	if(std::abs(glm::degrees(euler_rotation[0])) > view_pitch_clamp)
		euler_rotation[0] = glm::radians(view_pitch_clamp * ((glm::degrees(euler_rotation[0]) > 0) - (glm::degrees(euler_rotation[0]) < 0)));

	quaternion = glm::quat(euler_rotation);
	updateVectors();
}

void Camera::youGotACallBack(graphx::gSettings new_settings)
{
	Actor::youGotACallBack(new_settings);

	getSetting(position_local, settings["LocalPosition"]);
	getSetting(euler_rotation_local, settings["LocalRotationDegrees"]);
}

//
// GraphXPlayer
//
GraphXPlayer::GraphXPlayer(std::string new_name, glm::vec3 init_position, glm::vec3 init_rotation_euler)
: Actor(new_name, &player_mesh, init_position, init_rotation_euler, glm::vec3(1.0f, 2.0f, 1.0f))
{
	my_type = &graphx::classes::GRAPHXPLAYER;
	player_camera.euler_rotation = glm::radians(init_rotation_euler);
	player_camera.setGlobalRotation(init_position);
	visible = false;
}

void GraphXPlayer::youGotACallBack(graphx::gSettings new_settings)
{
	Actor::youGotACallBack(new_settings);

	getSetting(do_gravity, settings["DoGravity"]);
	getSetting(mouse_sensitivity, settings["MouseSensitivity"]);
	getSetting(movement_speed, settings["MovementSpeed"]);
	getSetting(lerp_speed, settings["MovementAcceleration"]);
	getSetting(friction, settings["Friction"]);
	getSetting(mass, settings["Mass"]);
	getSetting(field_of_view, settings["FOV"]);

	lerp_speed *= (double)1.0 / 120; // Hardcoded until I move TICKLENGTH and TICKRATE out of main.cpp
}

void GraphXPlayer::callToStage(Theatre *parent_theatre)
{
	Actor::callToStage(parent_theatre);

	player_flashlight = static_cast<LightFlashlight *>(getCurrentTheatre()->unsafeGetFirstActorOfType(graphx::classes::LIGHTFLASHLIGHT));

	player_settings = new JPH::CharacterSettings;
	player_settings->mMaxSlopeAngle = JPH::DegreesToRadians(45.0f);
	player_settings->mLayer = Layers::MOVING;
	player_settings->mShape = JPH::RotatedTranslatedShapeSettings(JPH::Vec3::sZero(), JPH::Quat::sIdentity(), new JPH::CylinderShape(scale[1], scale[0])).Create().Get();
	player_settings->mFriction = friction;
	player_settings->mMass = mass;
	player_settings->mGravityFactor *= do_gravity;
	player_settings->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), scale[0]);
	jph_character = new JPH::Character(player_settings, getPosition<JPH::Vec3>(), JPH::Quat::sIdentity(), 0, &jolt_physics_system);
	jph_character->AddToPhysicsSystem(JPH::EActivation::Activate);
}

void GraphXPlayer::processMouse(GLFWwindow *window, double x_position_in, double y_position_in)
{
	glm::vec2 mouse_position(static_cast<float>(x_position_in), static_cast<float>(y_position_in));
	glm::vec2 mouse_offset = mouse_position - mouse_last;
	mouse_last = mouse_position;

	doMouseMovement(mouse_offset);
}

void GraphXPlayer::processInput(GLFWwindow *window)
{
	int input_vector[2] =
	{
		glfwGetKey(window, GLFW_KEY_W) - glfwGetKey(window, GLFW_KEY_S),
		glfwGetKey(window, GLFW_KEY_D) - glfwGetKey(window, GLFW_KEY_A)
	};

	doMovement(input_vector);
}

void GraphXPlayer::tick(int current_tick)
{
	position_global = gmath::convertMath<glm::vec3>(jph_character->GetPosition());
	player_camera.setGlobalPosition(position_global);
}

void GraphXPlayer::processKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_F && action == GLFW_PRESS)
	{
		player_flashlight->toggleLight();
		if(player_flashlight->light_color == glm::vec3(0.0f))
			PRINTNOTE("Flashlight Off")
		else
			PRINTNOTE("Flashlight On")
	}

	if(key == GLFW_KEY_Q && action == GLFW_PRESS && player_flashlight != nullptr)
	{
		player_flashlight->setLightColor(flashlight_debug_toggle_color_god_damn_this_variable_name_is_long);
		if(player_flashlight->light_color == flashlight_debug_toggle_color_god_damn_this_variable_name_is_long)
			PRINTNOTE("Flashlight Red")
		else
			PRINTNOTE("Flashlight Not Red Anymore")
	}
}

void GraphXPlayer::doMovement(int direction[2])
{
	if(jph_character == nullptr)
		return;
	JPH::Vec3 current_velocity = jph_character->GetLinearVelocity();
	JPH::Vec3 wish_velocity = JPH::Vec3(0.0f, 0.0f, 0.0f);
	wish_velocity += gmath::convertMath<JPH::Vec3>(orientation_grounded_front) * static_cast<float>(direction[0] * movement_speed);
	wish_velocity += gmath::convertMath<JPH::Vec3>(orientation_right) * static_cast<float>(direction[1] * movement_speed);

	if(direction[0] == last_direction[0] && direction[1] == last_direction[1])
	{
		if(movement_lerp < 1.0f)
			movement_lerp += lerp_speed;
		if(movement_lerp > 1.0f)
			movement_lerp = 1.0f;
	}

	else
	{
		movement_lerp = 0.0f;
	}

	last_direction[0] = direction[0];
	last_direction[1] = direction[1];

	JPH::Vec3 new_velocity = gmath::linearInterpolate(current_velocity, wish_velocity, movement_lerp);
	new_velocity.SetY(current_velocity.GetY());
	if(new_velocity == current_velocity)
		return;
	jph_character->SetLinearVelocity(new_velocity);
}

void GraphXPlayer::doMouseMovement(glm::vec2 mouse_offset)
{
	player_camera.doRotation(mouse_offset * mouse_sensitivity);
	glm::vec3 horizontal_rotation = glm::vec3(0.0f, player_camera.euler_rotation[1], 0.0f);
	quaternion = glm::quat(horizontal_rotation);
	updateVectors();
}

glm::mat4 GraphXPlayer::getViewMatrix()
{
	return glm::lookAt(player_camera.getPosition<glm::vec3>(), player_camera.getPosition<glm::vec3>() + player_camera.orientation_front, player_camera.orientation_up);
}

glm::vec3 GraphXPlayer::getViewPosition()
{
	return player_camera.getPosition<glm::vec3>();
}

void GraphXPlayer::takeABow()
{
	Actor::takeABow();
	jolt_physics_system.GetBodyInterface().RemoveBody(jph_character->GetBodyID());
}

//
// Light
//
Light::Light(std::string init_name)
: Actor(init_name)
{
	my_type = &graphx::classes::LIGHT;
	my_light_type = &graphx::classes::LIGHT;
	debug_visible = true;
	scale = glm::vec3(0.25f);
}

const bool Light::isLightType(const graphx::gClass* light_type) const
{
	return light_type == my_light_type;
}

const bool Light::isLightType(const graphx::gClass& light_type) const
{
	return light_type == my_light_type;
}

const graphx::gClass* Light::getLightType() const
{
	return my_light_type;
}

void Light::youGotACallBack(graphx::gSettings new_settings)
{
	Actor::youGotACallBack(new_settings);

	getSetting(light_color, settings["Color"]);
	getSetting(light_energy, settings["Energy"]);
	getSetting(light_ambient_strength, settings["AmbientStrength"]);
	getSetting(light_specular_strength, settings["SpecularStrength"]);
	getSetting(light_attenuation, settings["FadeIntensity"]);
	getSetting(light_attenuation, settings["Attenuation"]);
	getSetting(light_range, settings["Range"]);

	// Just to be safe...
	if(mesh != nullptr)
	{
		mesh->prepForDestruction();
		mesh = nullptr;
		delete mesh;
	}
}

RenderCommands Light::getRenderCommands()
{
	RenderCommands render_commands = Actor::getRenderCommands();

	render_commands.light_render_command.light_type = my_light_type;
	render_commands.light_render_command.light_data.energy = light_energy;
	render_commands.light_render_command.light_data.ambient_strength = light_ambient_strength;
	render_commands.light_render_command.light_data.specular_strength = light_specular_strength;
	render_commands.light_render_command.light_data.color = light_color;
	render_commands.light_render_command.light_data.position = getPosition<glm::vec3>();
	render_commands.light_render_command.light_data.attenuation = light_attenuation;
	render_commands.light_render_command.light_data.range = light_range;

	if(debug_visible)
	{
		render_commands.render_command.is_light_debug_mesh = true;
		render_commands.render_command.mesh_data_name = GRAPHX_CUBE;
		render_commands.render_command.mesh_material = Material(LIGHT_DEBUGGING, NO_TEXTURE, 8, 0.0f, light_color * light_energy);
	}

	return(render_commands);
}

//
// LightDirectional
//
LightDirectional::LightDirectional(std::string init_name)
: Light(init_name)
{
	my_type = &graphx::classes::LIGHTDIRECTIONAL;
	my_light_type = &graphx::classes::LIGHTDIRECTIONAL;
	debug_visible = false;
}

void LightDirectional::youGotACallBack(graphx::gSettings new_settings)
{
	Light::youGotACallBack(new_settings);

	getSetting(directional_direction, settings["Direction"]);

	// LightDirectional doesn't really need a debug mesh, since it's physical orientation doesn't matter
	if(mesh != nullptr)
		mesh->prepForDestruction();
	mesh = nullptr;
	delete mesh;
}

RenderCommands LightDirectional::getRenderCommands()
{
	RenderCommands render_commands = Light::getRenderCommands();

	render_commands.light_render_command.light_data.direction = directional_direction;

	return(render_commands);
}

//
// LightSpot
//
LightSpot::LightSpot(std::string init_name)
: Light(init_name)
{
	my_type = &graphx::classes::LIGHTSPOT;
	my_light_type = &graphx::classes::LIGHTSPOT;
	debug_visible = true;
	scale = glm::vec3(0.25f);
}

void LightSpot::youGotACallBack(graphx::gSettings new_settings)
{
	Light::youGotACallBack(new_settings);

	getSetting(spot_direction, settings["Direction"]);
	getSetting(spot_angle, settings["Angle"]);
	getSetting(spot_angle_fade, settings["AngleFadeIntensity"]);
}

RenderCommands LightSpot::getRenderCommands()
{
	RenderCommands render_commands = Light::getRenderCommands();

	render_commands.light_render_command.light_data.direction = spot_direction;
	render_commands.light_render_command.light_data.spot_cutoff = glm::cos(glm::radians(spot_angle));
	render_commands.light_render_command.light_data.spot_cutoff_fade = glm::cos(glm::radians(spot_angle - spot_angle_fade));

	return(render_commands);
}

//
// LightFlashlight
//
LightFlashlight::LightFlashlight(std::string init_name)
: LightSpot(init_name)
{
	my_type = &graphx::classes::LIGHTFLASHLIGHT;
	my_light_type = &graphx::classes::LIGHTSPOT;
	debug_visible = false;
	light_ambient_strength = 0.0f;
	light_range = 120.f;
	light_attenuation = 0.5f;
	light_energy = 2.0f;
}

void LightFlashlight::youGotACallBack(graphx::gSettings new_settings)
{
	Light::youGotACallBack(new_settings);

	getSetting(start_enabled, settings["StartOn"]);
	getSetting(start_enabled, settings["StartEnabled"]);
	getSetting(position_offset, settings["PositionOffset"]);
	getSetting(rotation_offset, settings["RotationOffset"]);

	_color = light_color;
	setLight(start_enabled);

	if(mesh != nullptr)
		mesh->prepForDestruction();
	mesh = nullptr;
	delete mesh;
}

void LightFlashlight::tick(int current_tick)
{
	// Hardcoding LightFlashlight to only be applicable to the player for now
	if(getCurrentTheatre()->getPlayer() == nullptr)
		return;

	setGlobalPosition(getCurrentTheatre()->getPlayer()->player_camera.getPosition<glm::vec3>() + position_offset);
	setGlobalRotation(getCurrentTheatre()->getPlayer()->player_camera.getRotation<glm::quat>() * glm::quat(glm::radians(rotation_offset)));
	spot_direction = quaternion * vector3_front;
}

void LightFlashlight::toggleLight(glm::vec3 toggle_color)
{
	// https://stackoverflow.com/questions/392932/how-do-i-use-the-conditional-ternary-operator
	// That's just for reference... until I stop forgetting how to use the ternary operator LMFAOOOOOOOOOO
	// (and I wanted to link to the stackoverflow answer bc I don't plagerise, HBomberGuy...)
	light_color = (light_color == toggle_color) ? _color : toggle_color;
}

// Todo: decide whether or not to keep these `setLight` & `setLightColor` functions, since the toggle versions seem to be better
void LightFlashlight::setLight(bool is_on)
{
	light_color = _color * (float)is_on;
}

// Todo: decide whether or not to keep these `setLight` & `setLightColor` functions, since the toggle versions seem to be better
void LightFlashlight::setLightColor(glm::vec3 color)
{
	light_color = color;
	_color = color;
}

void LightFlashlight::setLightColor(bool color_toggle)
{
	if(color_toggle)
	{
		light_color = glm::vec3(1.0f, 0.0f, 0.0f);
		return;
	}

	light_color = _color;
}

//
// LightTesterMover
//
LightTesterMover::LightTesterMover(std::string init_name)
: Light(init_name)
{
	my_type = &graphx::classes::LIGHTTESTERMOVER;
	my_light_type = &graphx::classes::LIGHT;
	debug_visible = true;
}

void LightTesterMover::youGotACallBack(graphx::gSettings new_settings)
{
	Light::youGotACallBack(new_settings);

	getSetting(pivot_position, settings["PivotPosition"]);
	getSetting(pivot_radius, settings["PivotRadius"]);
	getSetting(pivot_speed, settings["PivotSpeed"]);

	pivot_point.setGlobalPosition(pivot_position);
	pivot_point.mesh->setName("Pivot Mesh for " + name + " LightTesterMover (UID: " + std::to_string(UID) + ")");
	pivot_point.mesh->mesh_data_name = GRAPHX_CUBE;
	pivot_point.mesh->setUID(4815 + UID);
	graphx::gSettings pivot_settings
	{
		{"Name", graphx::gSetting(RAW_DATA, graphx::interpreter::gRawData{std::string("Pivot point Actor for " + name + " LightTesterMover (UID: " + std::to_string(UID) + ")")})},
		{"MeshData", settings["MeshData"]},
	};
	getCurrentTheatre()->actorEnter(&pivot_point, 1623 + UID, pivot_settings);
}

void LightTesterMover::tick(int current_tick)
{
	pivot_point.setGlobalPosition(pivot_position);

	position_global[0] = pivot_position[0] + pivot_radius * glm::cos(glm::radians(pivot_theta));
	position_global[1] = pivot_position[1];
	position_global[2] = pivot_position[2] + pivot_radius * glm::sin(glm::radians(pivot_theta));

	pivot_theta += pivot_speed;
	if(pivot_theta >= 360.0f)
		pivot_theta = 0.0f;
}

void LightTesterMover::callToStage(Theatre *parent_theatre)
{}

void LightTesterMover::takeABow()
{}

//
// Ramiel
//
Ramiel::Ramiel()
: Actor("Ramiel")
{}

void Ramiel::youGotACallBack(graphx::gSettings new_settings)
{
	Actor::youGotACallBack(new_settings);

	getSetting(movement_type, settings["MovementType"]);
	getSetting(pivot_position, settings["PivotPosition"]);
	getSetting(pivot_radius, settings["PivotRadius"]);
	getSetting(pivot_speed, settings["PivotSpeed"]);
	getSetting(movement_speed, settings["MovementSpeed"]);
}

void Ramiel::tick(int current_tick)
{
	if(movement_type == RAMIEL_APPROACH)
	{
		position_global += movement_speed * orientation_front;
		return;
	}

	position_global[0] = pivot_position[0] + pivot_radius * glm::cos(glm::radians(pivot_theta));
	position_global[1] = pivot_position[1];
	position_global[2] = pivot_position[2] + pivot_radius * glm::sin(glm::radians(pivot_theta));

	pivot_theta += pivot_speed;
	if(pivot_theta >= 360.0f)
		pivot_theta = 0.0f;
}