#include "g_actors.hpp"
#include "graphx_classes_namespace.hpp"
#include "sanity.hpp"
#include <gmath.hpp>
#include <models.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

glm::vec3 vector3_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 vector3_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 vector3_right = glm::vec3(1.0f, 0.0f, 0.0f);

//
// Actor
//
Actor::Actor(std::string init_name, Mesh *init_mesh, glm::vec3 init_position, glm::vec3 init_euler_degrees, glm::vec3 init_scale)
: mesh(init_mesh), scale(init_scale), position_global(init_position)
{
	my_type = graphx::classes::ACTOR;
	name = init_name;
	quaternion = glm::quat(glm::radians(init_euler_degrees));
	RenderState render_state = RenderState(init_position, quaternion, init_scale);
	current_state_buffer = { render_state, render_state };
	previous_state_buffer = { render_state, render_state };
	updateVectors();
}

graphx::gClass &Actor::getType()
{
	return my_type;
}

void Actor::setName(std::string new_name)
{
	name = new_name;
}

void Actor::setName(char *new_name)
{
	name = new_name;
}

std::string Actor::getName()
{
	return name;
}

template<> glm::vec3 Actor::getPosition()
{
	return position_global + position_local;
}

template<> JPH::Vec3 Actor::getPosition()
{
	return gmath::convertMath<JPH::Vec3>(position_global) + gmath::convertMath<JPH::Vec3>(position_local);
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

template<> void Actor::setGlobalPosition(glm::vec3 new_value)
{
	position_global = new_value;
}

template<> void Actor::setGlobalPosition(JPH::Vec3 new_value)
{
	position_global = gmath::convertMath<glm::vec3>(new_value);
}

template<> void Actor::setGlobalRotation(glm::quat new_value)
{
	quaternion = new_value;
}

template<> void Actor::setGlobalRotation(JPH::Quat new_value)
{
	quaternion = gmath::convertMath<glm::quat>(new_value);
}

template<> void Actor::setGlobalRotation(glm::vec3 new_value)
{
	quaternion = glm::quat(new_value);
}

template<> void Actor::setGlobalRotation(JPH::Vec3 new_value)
{
	quaternion = glm::quat(gmath::convertMath<glm::vec3>(new_value));
}

template<> void Actor::setLocalPosition(glm::vec3 new_value)
{
	position_local = new_value;
}

template<> void Actor::setLocalPosition(JPH::Vec3 new_value)
{
	position_local = gmath::convertMath<glm::vec3>(new_value);
}

template<> void Actor::setLocalRotation(glm::quat new_value)
{
	local_quaternion = new_value;
}

template<> void Actor::setLocalRotation(JPH::Quat new_value)
{
	local_quaternion = gmath::convertMath<glm::quat>(new_value);
}

template<> void Actor::setLocalRotation(glm::vec3 new_value)
{
	local_quaternion = glm::quat(new_value);
}

template<> void Actor::setLocalRotation(JPH::Vec3 new_value)
{
	local_quaternion = glm::quat(gmath::convertMath<glm::vec3>(new_value));
}

RenderCommands Actor::getRenderCommands()
{
	RenderCommands render_commands;

	render_commands.render_command.current_render_state = &current_state_buffer[state_index];
	render_commands.render_command.previous_render_state = &previous_state_buffer[state_index];
	if(mesh != nullptr && visible && (my_type != graphx::classes::GRAPHXPLAYER))
	{
		render_commands.render_command.mesh_data_name = mesh->mesh_data_name;
		render_commands.render_command.mesh_material = *mesh->material;
	}
	else
	{
		// Todo: change this
		render_commands.render_command.mesh_data_name = ""; // So that RenderCmd::isValid returns false (might wanna make this a bit more sophisticated, later)
	}

	return(render_commands);
}

bool Actor::isPhysicsActor()
{
	return false;
}

void Actor::youGotACallBack(graphx::gSettings new_settings)
{
	if(settings.contains(empty_settings_identifier))
		settings = new_settings;

	if(new_settings.contains(empty_settings_identifier))
		new_settings = settings;

	glm::vec3 local_euler_degrees = glm::vec3(0.0f);
	glm::vec3 global_euler_degrees = glm::degrees(glm::eulerAngles(quaternion));

	getSetting(name, settings["Name"]);
	getSetting(mesh, settings["Mesh"]);
	getSetting(position_global, settings["Position"]);
	getSetting(position_local, settings["LocalPosition"]);
	getSetting(global_euler_degrees, settings["Rotation"]);
	getSetting(local_euler_degrees, settings["LocalRotation"]);
	getSetting(scale, settings["Scale"]);

	local_quaternion = glm::quat(glm::radians(local_euler_degrees));
	quaternion = glm::quat(glm::radians(global_euler_degrees));

	updateVectors();
}

void Actor::setUID(long manual_uid)
{
	if(manual_uid != -1)
		UID = manual_uid;
}

long Actor::getUID()
{
	return UID;
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
	current_state_buffer[state_index].render_position	=	getPosition<glm::vec3>();
	current_state_buffer[state_index].render_quaternion	=	getRotation<glm::quat>();
	current_state_buffer[state_index].render_scale		=	scale;

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

//
// Label
//
Label::Label(std::string init_name, Actor *init_parent)
: Actor(init_name, &label_mesh), parent(init_parent), text_render_command(TextRenderCmd("Verdana", init_name, 0.0f, 0.0f, 1.0f, glm::vec3(0.15f, 0.6f, 0.9f)))
{
	my_type = graphx::classes::LABEL;
}

RenderCommands Label::getRenderCommands()
{
	RenderCommands render_commands = Actor::getRenderCommands();
	text_render_command.position_x = position_global.x;
	text_render_command.position_y = position_global.y;
	render_commands.text_render_command = text_render_command;
	render_commands.text_render_command.current_render_state = &current_state_buffer[state_index];
	render_commands.text_render_command.previous_render_state = &previous_state_buffer[state_index];
	return render_commands;
}

void Label::tick(int current_tick)
{
	if(parent != nullptr)
	{
		setGlobalPosition(parent->getPosition<glm::vec3>());
		setGlobalRotation(parent->getRotation<glm::vec3>());
	}
	else
	{
		position_global.x += 0.0f;
	}
}

void Label::youGotACallBack(graphx::gSettings new_settings)
{
	Actor::youGotACallBack(new_settings);
	/**
	 * `getSetting` Tip:
	 *   When "overloading" `getSetting` settings, I like to make sure that the most verbose/explicit option always
	 *   comes last, making it override all other options. As an example, "TextColor" will always win over "Color",
	 *   and "Label" will always win over both "Message" and "Text". This basically just ensures that should multiple
	 *   versions of the same setting be used, the one that wins is the one that looks more intentional, hopefully
	 *   avoiding confusion.
	*/
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
PhysicsActor::PhysicsActor(std::string init_name, Mesh *init_mesh, glm::vec3 init_position, glm::vec3 init_euler_degrees, glm::vec3 init_scale)
: Actor(init_name, init_mesh, init_position, init_euler_degrees, init_scale)
{
	my_type = graphx::classes::PHYSICSACTOR;
}

bool PhysicsActor::isPhysicsActor()
{
	return true;
}

void PhysicsActor::youGotACallBack(graphx::gSettings new_settings)
{
	Actor::youGotACallBack(new_settings);

	getSetting(mass, settings["Mass"]);
	getSetting(collider, settings["Collider"]);
	collider->loadSettings();
}

void PhysicsActor::callToStage(Theatre *parent_theatre)
{
	Actor::callToStage(parent_theatre);

	collider->createBody();

	reset_position = getPosition<JPH::Vec3>();
	reset_quaternion = getRotation<JPH::Quat>();
}

void PhysicsActor::takeABow()
{
	Actor::takeABow();

	if(collider != nullptr)
		collider->prepForDestruction();
	collider = nullptr;
	delete collider;
}

void PhysicsActor::tick(int current_tick)
{
	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	JPH::Vec3 body_position = body_interface.GetCenterOfMassPosition(collider->getBodyID());
	JPH::Quat body_quaternion = body_interface.GetRotation(collider->getBodyID());

	position_global = gmath::convertMath<glm::vec3>(body_position);
	quaternion = gmath::convertMath<glm::quat>(body_quaternion);
	updateVectors();
}

void PhysicsActor::reset_to_initial_orientation_for_testing()
{
	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	body_interface.SetPositionAndRotation(collider->getBodyID(), reset_position, reset_quaternion, JPH::EActivation::Activate);
	body_interface.SetLinearAndAngularVelocity(collider->getBodyID(), JPH::Vec3::sZero(), JPH::Vec3::sZero());
}

//
// RigidBodyActor
//
RigidBodyActor::RigidBodyActor()
: PhysicsActor()
{
	my_type = graphx::classes::RIGIDBODYACTOR;
}

void RigidBodyActor::youGotACallBack(graphx::gSettings new_settings)
{
	PhysicsActor::youGotACallBack(new_settings);
}

void RigidBodyActor::callToStage(Theatre *parent_theatre)
{
	PhysicsActor::callToStage(parent_theatre);
	my_type = graphx::classes::RIGIDBODYACTOR;

	if(collider == nullptr)
		return;
	collider->prepForDestruction();
	collider = new Collider();
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
	my_type = graphx::classes::STATICBODYACTOR;
}

void StaticBodyActor::youGotACallBack(graphx::gSettings new_settings)
{
	PhysicsActor::youGotACallBack(new_settings);
}

void StaticBodyActor::callToStage(Theatre *parent_theatre)
{
	PhysicsActor::callToStage(parent_theatre);
	my_type = graphx::classes::STATICBODYACTOR;
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
}

//
// Camera
//
Camera::Camera()
{
	my_type = graphx::classes::CAMERA;
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
	my_type = graphx::classes::GRAPHXPLAYER;
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
	my_type = graphx::classes::LIGHT;
	my_light_type = graphx::classes::LIGHT;
	debug_visible = true;
	scale = glm::vec3(0.25f);
}

bool Light::isLightType(graphx::gClass light_type)
{
	return light_type == my_light_type;
}

graphx::gClass const &Light::getLightType()
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
	my_type = graphx::classes::LIGHTDIRECTIONAL;
	my_light_type = graphx::classes::LIGHTDIRECTIONAL;
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
	my_type = graphx::classes::LIGHTSPOT;
	my_light_type = graphx::classes::LIGHTSPOT;
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
	my_type = graphx::classes::LIGHTFLASHLIGHT;
	my_light_type = graphx::classes::LIGHTSPOT;
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
	my_type = graphx::classes::LIGHTTESTERMOVER;
	my_light_type = graphx::classes::LIGHT;
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