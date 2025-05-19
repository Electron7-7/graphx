#include "g_theatre.hpp"
#include "g_actor.hpp"
#include "g_actors.hpp"
#include <gmath.hpp>
#include <models.hpp>
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
	render_commands.render_command.mesh_material.color_alpha = label_alpha;
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

void PhysicsActor::setGlobalPosition(glm::vec3 new_value)
{
	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	body_interface.SetPosition(collider->getBodyID(), gmath::convertMath<JPH::Vec3>(new_value), JPH::EActivation::Activate);
}

void PhysicsActor::setGlobalRotation(glm::vec3 new_value)
{
	glm::quat new_quaternion(new_value);
	JPH::Quat new_new_quaternion = gmath::convertMath<JPH::Quat>(new_quaternion);
	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	body_interface.SetRotation(collider->getBodyID(), new_new_quaternion, JPH::EActivation::Activate);
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

	reset_to_initial_orientation_for_testing();
}

void PhysicsActor::callToStage(Theatre* parent_theatre)
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

void RigidBodyActor::youGotACallBack(graphx::gSettings new_settings)
{
	PhysicsActor::youGotACallBack(new_settings);
}

void RigidBodyActor::callToStage(Theatre *parent_theatre)
{
	PhysicsActor::callToStage(parent_theatre);

	if(collider == nullptr)
		return;
	collider->prepForDestruction();
	collider = new Collider("RigidBodyActor Collider");
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

void StaticBodyActor::youGotACallBack(graphx::gSettings new_settings)
{
	PhysicsActor::youGotACallBack(new_settings);
}

void StaticBodyActor::callToStage(Theatre *parent_theatre)
{
	PhysicsActor::callToStage(parent_theatre);
	if(collider == nullptr)
		return;
	collider->prepForDestruction();
	collider = new Collider("StaticBodyActor Collider");
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

	euler_rotation = glm::radians(glm::vec3(0.0f));
	quaternion = glm::quat(euler_rotation);
	updateVectors();
	PRINTDEBUG("Camera Rotation: " << glm::to_string(getRotation<glm::vec3>()));
}

//
// GraphXPlayer
//

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

	player_camera.youGotACallBack(); // The camera never gets tick() or youGotACallback() called, since it's not a child of the Theatre
}

void GraphXPlayer::callToStage(Theatre *parent_theatre)
{
	Actor::callToStage(parent_theatre);

	player_flashlight = static_cast<LightFlashlight *>(getCurrentTheatre()->getFlashlight()); // TEMPORARY

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
	glm::vec3 camera_pos = player_camera.getPosition<glm::vec3>();
	glm::vec3 camera_orientation_front = player_camera.orientation_front;
	glm::vec3 camera_orientation_up = player_camera.orientation_up;

	return glm::lookAt(camera_pos, camera_pos + camera_orientation_front, camera_orientation_up);
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

	render_commands.light_render_command.light_type = LightRenderCmd::POINT_LIGHT;
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

	render_commands.light_render_command.light_type = LightRenderCmd::DIRECTIONAL_LIGHT;
	render_commands.light_render_command.light_data.direction = directional_direction;

	return(render_commands);
}

//
// LightSpot
//

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

	render_commands.light_render_command.light_type = LightRenderCmd::SPOT_LIGHT;
	render_commands.light_render_command.light_data.direction = spot_direction;
	render_commands.light_render_command.light_data.spot_cutoff = glm::cos(glm::radians(spot_angle));
	render_commands.light_render_command.light_data.spot_cutoff_fade = glm::cos(glm::radians(spot_angle - spot_angle_fade));

	return(render_commands);
}

//
// LightFlashlight
//

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

void LightTesterMover::youGotACallBack(graphx::gSettings new_settings)
{
	Light::youGotACallBack(new_settings);

	getSetting(pivot_position, settings["PivotPosition"]);
	getSetting(pivot_radius, settings["PivotRadius"]);
	getSetting(pivot_speed, settings["PivotSpeed"]);

	pivot_point.setGlobalPosition(pivot_position);
	pivot_point.mesh = &temporary_pivot_mesh;
	pivot_point.mesh->name = "Pivot Mesh for " + name + " LightTesterMover (UID: " + std::to_string(getUID()) + ")";
	pivot_point.mesh->mesh_data_name = GRAPHX_CUBE;
	pivot_point.mesh->setUID(4815 + getUID());
	graphx::gSettings pivot_settings
	{
		{"Name", graphx::gSetting(StringSetting::RAW_DATA, gRawData{std::string("Pivot point Actor for " + name + " LightTesterMover (UID: " + std::to_string(getUID()) + ")")})},
		// {"MeshData", settings["MeshData"]},
	};
	getCurrentTheatre()->actorEnter(&pivot_point, 1623 + getUID(), pivot_settings);
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