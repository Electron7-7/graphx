#include "engine/theatre/theatre.hpp"
#include "engine/things/actors/actor.hpp"
#include "engine/things/actors/actors.hpp"
#include "math/gmath.hpp"
#include "models.hpp"
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>
#include <GLFW/glfw3.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

//
// Label
//
RenderCommands Label::getRenderCommands()
{
	RenderCommands render_commands = Actor::getRenderCommands();

	render_commands.render_command.mesh_data_name = GRAPHX_QUAD;
	render_commands.render_command.mesh_material.color_alpha = label_alpha;
	text_render_command.render_state = &current_state_buffer[state_index];
	text_render_command.position_x = (getGlobalPosition() + getLocalPosition()).x;
	text_render_command.position_y = (getGlobalPosition() + getLocalPosition()).y;
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
		// Todo: find better ways of accessing specific parts of position/rotation/scale
		glm::vec3 global_position = getGlobalPosition();
		global_position.x += 0.01f;
		setGlobalPosition(global_position);
	}
}

void Label::loadSettings()
{
	gSettings::configureBaseVariables(this);
	/**
	 * `getSetting` Tip:
	 *   When "overloading" `getSetting` settings, I like to make sure that the most verbose/explicit option always
	 *   comes last, making it override all other options. As an example, "TextColor" will always win over "Color",
	 *   and "Label" will always win over both "Message" and "Text". This basically just ensures that should multiple
	 *   versions of the same setting be used, the one that wins is the one that looks more intentional, hopefully
	 *   avoiding confusion.
	*/
	settings.getNumber("Transparency", label_alpha);
	settings.getNumber("Alpha", label_alpha);
	settings.getString("Font", text_render_command.font_name);
	settings.getString("FontName", text_render_command.font_name);
	settings.getNumber("Color", text_render_command.color);
	settings.getNumber("TextColor", text_render_command.color);
	settings.getString("Message", text_render_command.text);
	settings.getString("Text", text_render_command.text);
	settings.getString("Label", text_render_command.text);
	settings.getNumber("TextScale", text_render_command.scale);

	text_render_command.position_x = 0.0f;
	text_render_command.position_y = 0.0f;
}

//
// PhysicsActor
//
void PhysicsActor::overrideColliderPosition(glm::vec3 new_value)
{
	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	body_interface.SetPosition(collider->getBodyID(), gmath::convertMath<JPH::Vec3>(new_value), JPH::EActivation::Activate);
}

void PhysicsActor::overrideColliderRotation(glm::vec3 new_value, bool as_degrees)
{
	if(as_degrees)
		new_value = glm::radians(new_value);
	glm::quat new_quaternion(new_value);
	JPH::Quat new_new_quaternion = gmath::convertMath<JPH::Quat>(new_quaternion);
	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	body_interface.SetRotation(collider->getBodyID(), new_new_quaternion, JPH::EActivation::Activate);
}

bool PhysicsActor::isPhysicsActor() const
{
	return true;
}

void PhysicsActor::loadSettings()
{
	gSettings::configureBaseVariables(this);

	settings.getNumber("Mass", mass);
	settings.getDevice("Collider", collider);

	collider->loadSettings();

	reset_to_initial_orientation_for_testing();
}

void PhysicsActor::callToStage(Theatre* parent_theatre)
{
	Actor::callToStage(parent_theatre);

	collider->createBody();

	reset_position = gmath::convertMath<JPH::Vec3>(getGlobalPosition() + getLocalPosition());
	reset_quaternion = gmath::convertMath<JPH::Quat>(getGlobalQuaternion() * getLocalQuaternion());
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

	setGlobalPosition(gmath::convertMath<glm::vec3>(body_position));
	setGlobalQuaternion(gmath::convertMath<glm::quat>(body_quaternion));
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

void RigidBodyActor::loadSettings()
{
	gSettings::configureBaseVariables(this);
}

void RigidBodyActor::callToStage(Theatre *parent_theatre)
{
	PhysicsActor::callToStage(parent_theatre);

	if(collider == nullptr)
		return;
	collider->prepForDestruction();
	collider->activation = JPH::EActivation::Activate;
	collider->motion_type = JPH::EMotionType::Dynamic;
	collider->object_layer = Layers::MOVING;
	collider->scale = getAbsoluteScale();
	collider->position = getGlobalPosition();
	collider->local_position = getLocalPosition();
	collider->euler_angles = getGlobalEulerAngles(true);
	collider->local_euler_angles = getLocalEulerAngles(true);
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

void StaticBodyActor::loadSettings()
{
	gSettings::configureBaseVariables(this);
}

void StaticBodyActor::callToStage(Theatre* parent_theatre)
{
	PhysicsActor::callToStage(parent_theatre);

	if(collider == nullptr)
		return;
	collider->prepForDestruction();
	collider->activation = JPH::EActivation::Activate;
	collider->motion_type = JPH::EMotionType::Static;
	collider->object_layer = Layers::NON_MOVING;
	collider->scale = getAbsoluteScale();
	collider->position = getGlobalPosition();
	collider->local_position = getLocalPosition();
	collider->euler_angles = getGlobalEulerAngles(true);
	collider->local_euler_angles = getLocalEulerAngles(true);
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

void Camera::tick(int current_tick)
{}

void Camera::doRotation(glm::vec2 mouse_input)
{
	euler_rotation += euler_rotation_local;
	euler_rotation[0] -= glm::radians(mouse_input[1]);
	euler_rotation[1] -= glm::radians(mouse_input[0]);

	if(std::abs(glm::degrees(euler_rotation[0])) > view_pitch_clamp)
		euler_rotation[0] = glm::radians(view_pitch_clamp * ((glm::degrees(euler_rotation[0]) > 0) - (glm::degrees(euler_rotation[0]) < 0)));

	setGlobalQuaternion(glm::quat(euler_rotation));
}

void Camera::loadSettings()
{
	gSettings::configureBaseVariables(this);

	settings.getNumber("LocalPosition", position_local);
	settings.getNumber("LocalRotationDegrees", euler_rotation_local);

	euler_rotation = glm::radians(glm::vec3(0.0f));
	setGlobalQuaternion(glm::quat(euler_rotation));

	setLocalPosition(glm::vec3(0.0f, 3.0f, 1.0f)); // Temporary hardcoded offset
}

//
// GraphXPlayer
//
void GraphXPlayer::loadSettings()
{
	gSettings::configureBaseVariables(this);

	settings.getBoolean("DoGravity", do_gravity);
	settings.getNumber("MouseSensitivity", mouse_sensitivity);
	settings.getNumber("MovementSpeed", movement_speed);
	settings.getNumber("MovementAcceleration", lerp_speed);
	settings.getNumber("Friction", friction);
	settings.getNumber("Mass", mass);
	settings.getNumber("FOV", field_of_view);

	lerp_speed *= (double)1.0 / 120; // Hardcoded until I move TICKLENGTH and TICKRATE out of main.cpp
	visible = false;

	player_camera.loadSettings(); // The camera never gets tick() or loadSettings() called, since it's not a child of the Theatre
}

void GraphXPlayer::callToStage(Theatre *parent_theatre)
{
	Actor::callToStage(parent_theatre);

	player_settings = new JPH::CharacterSettings;
	player_settings->mMaxSlopeAngle = JPH::DegreesToRadians(45.0f);
	player_settings->mLayer = Layers::MOVING;
	player_settings->mShape = JPH::RotatedTranslatedShapeSettings(JPH::Vec3::sZero(), JPH::Quat::sIdentity(), new JPH::CylinderShape(getGlobalScale()[1], getGlobalScale()[0])).Create().Get();
	player_settings->mFriction = friction;
	player_settings->mMass = mass;
	player_settings->mGravityFactor *= do_gravity;
	player_settings->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), getGlobalScale()[0]);
	// Todo: get rid of this if it's a memory leak
	jph_character = new JPH::Character(player_settings, gmath::convertMath<JPH::Vec3>(getGlobalPosition() + getLocalPosition()), JPH::Quat::sIdentity(), 0, &jolt_physics_system);
	jph_character->AddToPhysicsSystem(JPH::EActivation::Activate);
}

void GraphXPlayer::processMouse(GLFWwindow *window, double x_position_in, double y_position_in)
{
	glm::vec2 mouse_position(static_cast<float>(x_position_in), static_cast<float>(y_position_in));
	glm::vec2 mouse_offset = mouse_position - mouse_last;
	mouse_last = mouse_position;

	doMouseMovement(mouse_offset);
}

void GraphXPlayer::checkForInput(GLFWwindow* window)
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
	setGlobalPosition(gmath::convertMath<glm::vec3>(jph_character->GetPosition()));
	player_camera.setGlobalPosition(getGlobalPosition());
}

void GraphXPlayer::processKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if((key == GLFW_KEY_F && action == GLFW_PRESS))
	{
		player_flashlight = getCurrentTheatre()->getFlashlight(); // TEMPORARY
		player_flashlight->toggleLight();
		if(player_flashlight->light_color == glm::vec3(0.0f))
			PRINTNOTE("Flashlight Off")
		else
			PRINTNOTE("Flashlight On")
	}

	if(key == GLFW_KEY_Q && action == GLFW_PRESS)
	{
		player_flashlight = getCurrentTheatre()->getFlashlight(); // TEMPORARY
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
	wish_velocity += gmath::convertMath<JPH::Vec3>(glm::vec3(getOrientationFront()[0], 0.0f, getOrientationFront()[2])) * static_cast<float>(direction[0] * movement_speed);
	wish_velocity += gmath::convertMath<JPH::Vec3>(getOrientationRight()) * static_cast<float>(direction[1] * movement_speed);

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
	setGlobalQuaternion(glm::quat(horizontal_rotation));
}

glm::mat4 GraphXPlayer::getViewMatrix()
{ return glm::lookAt(player_camera.getAbsolutePosition(), player_camera.getAbsolutePosition() + player_camera.getOrientationFront(), player_camera.getOrientationUp()); }

glm::vec3 GraphXPlayer::getViewPosition()
{ return player_camera.getAbsolutePosition(); }

void GraphXPlayer::takeABow()
{
	Actor::takeABow();
	jph_character->RemoveFromPhysicsSystem();
}

//
// Light
//
void Light::loadSettings()
{
	gSettings::configureBaseVariables(this);

	settings.getNumber("Color", light_color);
	settings.getNumber("Energy", light_energy);
	settings.getNumber("AmbientStrength", light_ambient_strength);
	settings.getNumber("SpecularStrength", light_specular_strength);
	settings.getNumber("FadeIntensity", light_attenuation);
	settings.getNumber("Attenuation", light_attenuation);
	settings.getNumber("Range", light_range);

	// mesh->prepForDestruction();
	mesh = &graphx::debug::light_debug_mesh;
}

RenderCommands Light::getRenderCommands()
{
	RenderCommands render_commands = Actor::getRenderCommands();

	render_commands.render_command.current_render_state.render_scale = glm::vec3(0.35f);
	render_commands.render_command.previous_render_state.render_scale = glm::vec3(0.35f);

	render_commands.light_render_command.light_type = LightRenderCmd::POINT_LIGHT;
	render_commands.light_render_command.light_data.energy = light_energy;
	render_commands.light_render_command.light_data.ambient_strength = light_ambient_strength;
	render_commands.light_render_command.light_data.specular_strength = light_specular_strength;
	render_commands.light_render_command.light_data.color = light_color;
	render_commands.light_render_command.light_data.position = getAbsolutePosition();
	render_commands.light_render_command.light_data.attenuation = light_attenuation;
	render_commands.light_render_command.light_data.range = light_range;

	if(debug_visible)
	{
		render_commands.render_command.is_light_debug_mesh = true;
		render_commands.render_command.mesh_data_name = GRAPHX_CUBE;
		render_commands.render_command.mesh_material = Material("Light Debug Material", LIGHT_DEBUGGING, true);
	}

	return(render_commands);
}

//
// LightDirectional
//
void LightDirectional::loadSettings()
{
	gSettings::configureBaseVariables(this);

	// Todo: expand configureBaseVariables to include other types (like Light)
	settings.getNumber("Color", light_color);
	settings.getNumber("Energy", light_energy);
	settings.getNumber("AmbientStrength", light_ambient_strength);
	settings.getNumber("SpecularStrength", light_specular_strength);
	settings.getNumber("FadeIntensity", light_attenuation);
	settings.getNumber("Attenuation", light_attenuation);
	settings.getNumber("Range", light_range);
	settings.getNumber("Direction", directional_direction);

	mesh = &graphx::debug::light_debug_mesh;
	debug_visible = false;
}

RenderCommands LightDirectional::getRenderCommands()
{
	RenderCommands render_commands = Light::getRenderCommands();

	render_commands.light_render_command.light_type = LightRenderCmd::DIRECTIONAL_LIGHT;
	render_commands.light_render_command.light_data.direction = directional_direction;

	return(render_commands);
}

//
// LightSpot
//
void LightSpot::loadSettings()
{
	gSettings::configureBaseVariables(this);

	// Todo: expand configureBaseVariables to include other types (like Light)
	settings.getNumber("Color", light_color);
	settings.getNumber("Energy", light_energy);
	settings.getNumber("AmbientStrength", light_ambient_strength);
	settings.getNumber("SpecularStrength", light_specular_strength);
	settings.getNumber("FadeIntensity", light_attenuation);
	settings.getNumber("Attenuation", light_attenuation);
	settings.getNumber("Range", light_range);
	settings.getNumber("Direction", spot_direction);
	settings.getNumber("Angle", spot_angle);
	settings.getNumber("AngleFadeIntensity", spot_angle_fade);

	mesh = &graphx::debug::light_debug_mesh;
}

RenderCommands LightSpot::getRenderCommands()
{
	RenderCommands render_commands = Light::getRenderCommands();

	render_commands.light_render_command.light_type = LightRenderCmd::SPOT_LIGHT;
	render_commands.light_render_command.light_data.direction = spot_direction;
	render_commands.light_render_command.light_data.spot_cutoff = glm::cos(glm::radians(spot_angle));
	render_commands.light_render_command.light_data.spot_cutoff_fade = glm::cos(glm::radians(spot_angle - spot_angle_fade));

	return(render_commands);
}

//
// LightFlashlight
//
void LightFlashlight::loadSettings()
{
	gSettings::configureBaseVariables(this);

	// Todo: expand configureBaseVariables to include other types (like Light)
	settings.getNumber("Color", light_color);
	settings.getNumber("Energy", light_energy);
	settings.getNumber("AmbientStrength", light_ambient_strength);
	settings.getNumber("SpecularStrength", light_specular_strength);
	settings.getNumber("FadeIntensity", light_attenuation);
	settings.getNumber("Attenuation", light_attenuation);
	settings.getNumber("Range", light_range);
	settings.getBoolean("StartOn", start_enabled);
	settings.getBoolean("StartEnabled", start_enabled);
	settings.getNumber("PositionOffset", position_offset);
	settings.getNumber("RotationOffset", rotation_offset);

	_color = light_color;
	setLight(start_enabled);

	visible = false;
	debug_visible = false;
}

void LightFlashlight::tick(int current_tick)
{
	// Hardcoding LightFlashlight to only be applicable to the player for now
	if(getCurrentTheatre()->getPlayer() == nullptr)
		return;

	setGlobalPosition(getCurrentTheatre()->getPlayer()->player_camera.getAbsolutePosition() + position_offset);
	setGlobalQuaternion(getCurrentTheatre()->getPlayer()->player_camera.getAbsoluteQuaternion() * glm::quat(glm::radians(rotation_offset)));
	spot_direction = getAbsoluteQuaternion() * graphx::orientation::front;
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
void LightTesterMover::loadSettings()
{
	gSettings::configureBaseVariables(this);

	// Todo: expand configureBaseVariables to include other types (like Light)
	settings.getNumber("Color", light_color);
	settings.getNumber("Energy", light_energy);
	settings.getNumber("AmbientStrength", light_ambient_strength);
	settings.getNumber("SpecularStrength", light_specular_strength);
	settings.getNumber("FadeIntensity", light_attenuation);
	settings.getNumber("Attenuation", light_attenuation);
	settings.getNumber("Range", light_range);
	settings.getNumber("PivotPosition", pivot_position);
	settings.getNumber("PivotRadius", pivot_radius);
	settings.getNumber("PivotSpeed", pivot_speed);

	pivot_point.setGlobalPosition(pivot_position);
	pivot_point.mesh = &temporary_pivot_mesh;
	pivot_point.mesh->name = "Pivot Mesh for " + name + " LightTesterMover (UID: " + std::to_string(getUID()) + ")";
	pivot_point.mesh->mesh_data_name = GRAPHX_CUBE;
	pivot_point.mesh->setUID(4815 + getUID());
	gSettings pivosettings;
	pivosettings.raw_data["Name"] = gRawData{std::string("Pivot point Actor for " + name + " LightTesterMover (UID: " + std::to_string(getUID()) + ")")};
	pivot_point.loadSettings();
}

RenderCommands LightTesterMover::getRenderCommands()
{
	// Todo: also add the pivot point to the render commands!
	return Light::getRenderCommands();
}

void LightTesterMover::tick(int current_tick)
{
	pivot_point.setGlobalPosition(pivot_position);

	glm::vec3 new_global_position = getGlobalPosition();

	new_global_position[0] = pivot_position[0] + pivot_radius * glm::cos(glm::radians(pivot_theta));
	new_global_position[1] = pivot_position[1];
	new_global_position[2] = pivot_position[2] + pivot_radius * glm::sin(glm::radians(pivot_theta));

	setGlobalPosition(new_global_position);

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

void Ramiel::loadSettings()
{
	gSettings::configureBaseVariables(this);

	settings.getNumber("MovementType", movement_type);
	settings.getNumber("PivotPosition", pivot_position);
	settings.getNumber("PivotRadius", pivot_radius);
	settings.getNumber("PivotSpeed", pivot_speed);
	settings.getNumber("MovementSpeed", movement_speed);
}

void Ramiel::tick(int current_tick)
{
	glm::vec3 new_global_position = getGlobalPosition();

	if(movement_type == RAMIEL_APPROACH)
	{
		setGlobalPosition(new_global_position += movement_speed * getOrientationFront());
		return;
	}

	new_global_position[0] = pivot_position[0] + pivot_radius * glm::cos(glm::radians(pivot_theta));
	new_global_position[1] = pivot_position[1];
	new_global_position[2] = pivot_position[2] + pivot_radius * glm::sin(glm::radians(pivot_theta));

	setGlobalPosition(new_global_position);

	pivot_theta += pivot_speed;
	if(pivot_theta >= 360.0f)
		pivot_theta = 0.0f;
}