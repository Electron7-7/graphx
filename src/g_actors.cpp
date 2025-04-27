#include "g_actors.hpp"
#include "g_theatre.hpp"
#include "t_settings.hpp"
#include <gmath.hpp>
#include <models.hpp>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

using namespace graphx;

//
// Label
//
RenderCommands Label::getRenderCommands()
{
	RenderCommands render_commands = Actor::getRenderCommands();
	render_commands.render_command.mesh_material->color_alpha = label_alpha;
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
		setGlobalPosition(getGlobalPosition() += 0.01f);
	}
}

void Label::loadSettings()
{
	Actor::loadSettings();

	std::string font_name_catcher = text_render_command.getFontName();

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
	getSetting(font_name_catcher, settings["Font"]);
	getSetting(font_name_catcher, settings["FontName"]);
	getSetting(text_render_command.color, settings["Color"]);
	getSetting(text_render_command.color, settings["TextColor"]);
	getSetting(text_render_command.text, settings["Message"]);
	getSetting(text_render_command.text, settings["Text"]);
	getSetting(text_render_command.text, settings["Label"]);
	getSetting(text_render_command.scale, settings["TextScale"]);

	text_render_command.position_x = 0.0f;
	text_render_command.position_y = 0.0f;
	text_render_command.setFontName(font_name_catcher);
}

//
// Camera
//
void Camera::processMouse(GLFWwindow* window, double x_position_in, double y_position_in)
{
	glm::vec2 mouse_position(static_cast<float>(x_position_in), static_cast<float>(y_position_in));
	glm::vec2 mouse_offset = mouse_position - mouse_last;
	mouse_last = mouse_position;

	glm::vec3 euler_rotation = getGlobalEulerAngles(true) + getLocalEulerAngles(true);
	euler_rotation[0] -= mouse_offset[1];
	euler_rotation[1] -= mouse_offset[0];

	if(std::abs(euler_rotation[0]) > view_pitch_clamp_degrees)
		euler_rotation[0] = view_pitch_clamp_degrees * ((euler_rotation[0] > 0) - euler_rotation[0] < 0);

	setGlobalEulerAngles(euler_rotation, true);
}

void Camera::loadSettings()
{ Actor::loadSettings(); } // Eventually, I will have Camera settings

//
// GraphXPlayer
//
void GraphXPlayer::loadSettings()
{
	Actor::loadSettings();

	getSetting(do_gravity, settings["DoGravity"]);
	getSetting(mouse_sensitivity, settings["MouseSensitivity"]);
	getSetting(movement_speed, settings["MovementSpeed"]);
	getSetting(lerp_speed, settings["MovementAcceleration"]);
	getSetting(friction, settings["Friction"]);
	getSetting(mass, settings["Mass"]);
	getSetting(field_of_view, settings["FOV"]);

	lerp_speed *= (double)1.0 / 120; // Hardcoded until I move TICKLENGTH and TICKRATE out of main.cpp
}

/*void GraphXPlayer::callToStage(Theatre *parent_theatre)
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
}*/

void GraphXPlayer::processMouse(GLFWwindow *window, double x_position_in, double y_position_in)
{
	glm::vec2 mouse_position(static_cast<float>(x_position_in), static_cast<float>(y_position_in));
	glm::vec2 mouse_offset = mouse_position - mouse_last;
	mouse_last = mouse_position;

	setGlobalYaw(getGlobalEulerAngles(true)[1] + getLocalEulerAngles(true)[1] - mouse_offset[0], true);
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

void GraphXPlayer::tick(const int current_tick)
{
	setGlobalPosition(gmath::convertMath<glm::vec3>(jph_character->GetPosition()));
	player_camera.setGlobalPosition(getGlobalPosition());
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

glm::mat4 GraphXPlayer::getViewMatrix()
{
	return glm::lookAt(player_camera.getGlobalPosition(), player_camera.getGlobalPosition() + player_camera.getOrientationFront(), player_camera.getOrientationUp());
}

glm::vec3 GraphXPlayer::getViewPosition()
{
	return player_camera.getGlobalPosition();
}

//
// Light
//
void Light::loadSettings()
{
	Actor::loadSettings();

	getSetting(light_color, settings["Color"]);
	getSetting(light_energy, settings["Energy"]);
	getSetting(light_ambient_strength, settings["AmbientStrength"]);
	getSetting(light_specular_strength, settings["SpecularStrength"]);
	getSetting(light_attenuation, settings["FadeIntensity"]);
	getSetting(light_attenuation, settings["Attenuation"]);
	getSetting(light_range, settings["Range"]);

	// Just to be safe...
	delete mesh;
}

RenderCommands Light::getRenderCommands()
{
	RenderCommands render_commands = Actor::getRenderCommands();

	render_commands.light_render_command.light_type = LightRenderCmd::POINT_LIGHT;
	render_commands.light_render_command.energy = light_energy;
	render_commands.light_render_command.ambient_strength = light_ambient_strength;
	render_commands.light_render_command.specular_strength = light_specular_strength;
	render_commands.light_render_command.color = light_color;
	render_commands.light_render_command.position = getGlobalPosition();
	render_commands.light_render_command.attenuation = light_attenuation;
	render_commands.light_render_command.range = light_range;

	if(debug_visible)
	{
		render_commands.render_command.is_light_debug_mesh = true;
		render_commands.render_command.mesh_data_name = GRAPHX_CUBE;
		render_commands.render_command.mesh_material = &debug_light_mesh_material;
	}

	return(render_commands);
}

//
// LightDirectional
//
void LightDirectional::loadSettings()
{
	Light::loadSettings();

	getSetting(directional_direction, settings["Direction"]);

	// LightDirectional doesn't really need a debug mesh, since it's physical orientation doesn't matter
	delete mesh;
}

RenderCommands LightDirectional::getRenderCommands()
{
	RenderCommands render_commands = Light::getRenderCommands();

	render_commands.light_render_command.light_type = LightRenderCmd::DIRECTIONAL_LIGHT;
	render_commands.light_render_command.direction = directional_direction;

	return(render_commands);
}

//
// LightSpot
//
void LightSpot::loadSettings()
{
	Light::loadSettings();

	getSetting(spot_direction, settings["Direction"]);
	getSetting(spot_angle, settings["Angle"]);
	getSetting(spot_angle_fade, settings["AngleFadeIntensity"]);
}

RenderCommands LightSpot::getRenderCommands()
{
	RenderCommands render_commands = Light::getRenderCommands();

	render_commands.light_render_command.light_type = LightRenderCmd::SPOT_LIGHT;
	render_commands.light_render_command.direction = spot_direction;
	render_commands.light_render_command.spot_cutoff = glm::cos(glm::radians(spot_angle));
	render_commands.light_render_command.spot_cutoff_fade = glm::cos(glm::radians(spot_angle - spot_angle_fade));

	return(render_commands);
}

//
// LightFlashlight
//
void LightFlashlight::loadSettings()
{
	Light::loadSettings();

	glm::vec3 local_position = getLocalPosition();
	glm::quat local_quaternion = getLocalQuaternion();

	getSetting(parent, settings["Parent"]);
	getSetting(start_enabled, settings["StartOn"]);
	getSetting(start_enabled, settings["StartEnabled"]);
	getSetting(local_position, settings["PositionOffset"]);
	getSetting(local_quaternion, settings["RotationOffset"]);

	setLocalPosition(local_position);
	setLocalQuaternion(local_quaternion);

	_color = light_color;
	setLight(start_enabled);

	delete mesh;
}

void LightFlashlight::tick(int current_tick)
{
	// Hardcoding LightFlashlight to only be applicable to the player for now
	if(graphx::current::player == nullptr)
		return;

	setGlobalPosition(dynamic_cast<GraphXPlayer*>(graphx::current::player)->player_camera.getGlobalPosition() + getLocalPosition());
	setGlobalQuaternion(dynamic_cast<GraphXPlayer*>(graphx::current::player)->player_camera.getGlobalQuaternion() * getLocalQuaternion());
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
	Light::loadSettings();

	getSetting(pivot_position, settings["PivotPosition"]);
	getSetting(pivot_radius, settings["PivotRadius"]);
	getSetting(pivot_speed, settings["PivotSpeed"]);

	pivot_point.setGlobalPosition(pivot_position);
	temporary_pivot_mesh.name = "Pivot Model for " + name + " LightTesterMover (UID: " + std::to_string(getUID()) + ")";
	temporary_pivot_mesh.mesh_data_name = GRAPHX_CUBE;
	pivot_point.mesh = &temporary_pivot_mesh;
	parent_theatre->addActor(&pivot_point);
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

//
// Ramiel
//
void Ramiel::loadSettings()
{
	Actor::loadSettings();

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