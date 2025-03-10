#include "g_actors.hpp"
#include "t_settings.hpp"
#include "g_jolt.hpp"
#include "g_math.hpp"
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

glm::vec3 vector3_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 vector3_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 vector3_right = glm::vec3(1.0f, 0.0f, 0.0f);

std::map<int, Actor*(*)()> actor_map =
{
	{graphx::classes::ACTOR, &createNewActor<Actor>},
	{graphx::classes::PHYSICSACTOR, &createNewActor<PhysicsActor>},
	{graphx::classes::RIGIDBODYACTOR, &createNewActor<RigidBodyActor>},
	{graphx::classes::STATICBODYACTOR, &createNewActor<StaticBodyActor>},
	{graphx::classes::CAMERA, &createNewActor<Camera>},
	{graphx::classes::GRAPHXPLAYER, &createNewActor<GraphXPlayer>},
	{graphx::classes::LIGHT, &createNewActor<Light>},
	{graphx::classes::LIGHTDIRECTIONAL, &createNewActor<LightDirectional>},
	{graphx::classes::LIGHTSPOT, &createNewActor<LightSpot>},
	{graphx::classes::LIGHTFLASHLIGHT, &createNewActor<LightFlashlight>},
	{graphx::classes::LIGHTTESTERMOVER, &createNewActor<LightTesterMover>},
};

//
// Actor
//
Actor::Actor(std::string new_name, Mesh *init_mesh, glm::vec3 init_position, glm::vec3 init_euler_degrees, glm::vec3 init_scale)
: mesh(init_mesh), scale(init_scale), position_global(init_position)
{
	my_type = graphx::classes::ACTOR;
	name = new_name;
	mesh = new Mesh();
	quaternion = glm::quat(glm::radians(init_euler_degrees));
	current_state = RenderState(init_position, quaternion, init_scale);
	current_state_copy = current_state;
	previous_state = current_state;
	previous_state_copy = current_state;
	current_state_buffer = { current_state, current_state_copy };
	previous_state_buffer = { previous_state, previous_state_copy };
	updateVectors();
}

std::string Actor::getTypeName()
{
	return graphx::classnames.at(my_type);
}

long Actor::getType()
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
	return convertMath<JPH::Vec3>(position_global) + convertMath<JPH::Vec3>(position_local);
}

template<> glm::vec3 Actor::getRotation()
{
	return glm::eulerAngles(quaternion * local_quaternion);
}

template<> JPH::Vec3 Actor::getRotation()
{
	return convertMath<JPH::Vec3>(glm::eulerAngles(quaternion * local_quaternion));
}

template<> glm::quat Actor::getRotation()
{
	return quaternion * local_quaternion;
}

template<> JPH::Quat Actor::getRotation()
{
	return convertMath<JPH::Quat>(quaternion) * convertMath<JPH::Quat>(local_quaternion);
}

template<> void Actor::setGlobalPosition(glm::vec3 new_value)
{
	position_global = new_value;
}

template<> void Actor::setGlobalPosition(JPH::Vec3 new_value)
{
	position_global = convertMath<glm::vec3>(new_value);
}

template<> void Actor::setGlobalRotation(glm::quat new_value)
{
	quaternion = new_value;
}

template<> void Actor::setGlobalRotation(JPH::Quat new_value)
{
	quaternion = convertMath<glm::quat>(new_value);
}

template<> void Actor::setGlobalRotation(glm::vec3 new_value)
{
	quaternion = glm::quat(new_value);
}

template<> void Actor::setGlobalRotation(JPH::Vec3 new_value)
{
	quaternion = glm::quat(convertMath<glm::vec3>(new_value));
}

template<> void Actor::setLocalPosition(glm::vec3 new_value)
{
	position_local = new_value;
}

template<> void Actor::setLocalPosition(JPH::Vec3 new_value)
{
	position_local = convertMath<glm::vec3>(new_value);
}

template<> void Actor::setLocalRotation(glm::quat new_value)
{
	local_quaternion = new_value;
}

template<> void Actor::setLocalRotation(JPH::Quat new_value)
{
	local_quaternion = convertMath<glm::quat>(new_value);
}

template<> void Actor::setLocalRotation(glm::vec3 new_value)
{
	local_quaternion = glm::quat(new_value);
}

template<> void Actor::setLocalRotation(JPH::Vec3 new_value)
{
	local_quaternion = glm::quat(convertMath<glm::vec3>(new_value));
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

bool Actor::isType(int class_type)
{
	return class_type == my_type;
}

bool Actor::isType(std::initializer_list<int> const &class_types)
{
	for(int type : class_types)
		if(my_type == type)
			return true;
	return false;
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
{
	// PRINTLN("\t- Name: " << name << "\n\t- UID: " << UID << "\n\t- Type: " << std::to_string(my_type))
}

void Actor::takeABow()
{
	if(mesh != nullptr)
		mesh->prepForDestruction();

	// PRINTLN("\t- Name: " << name << "\n\t- UID: " << UID << "\n\t- Type: " << std::to_string(my_type))
}

bool Actor::wantsToBeRendered()
{
	return (mesh != nullptr && mesh->is_buffered && visible);
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

	position_global = convertMath<glm::vec3>(body_position);
	quaternion = convertMath<glm::quat>(body_quaternion);
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
	position_global = convertMath<glm::vec3>(jph_character->GetPosition());
	player_camera.setGlobalPosition(position_global);
}

void GraphXPlayer::processKey(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	if(key == GLFW_KEY_F && action == GLFW_PRESS && player_flashlight != nullptr)
	{
		flashlight_toggle = !flashlight_toggle;
		player_flashlight->setLight(flashlight_toggle);
		if(flashlight_toggle)
			PRINTNOTE("Flashlight Off")
		else
			PRINTNOTE("Flashlight On")
	}

	if(key == GLFW_KEY_Q && action == GLFW_PRESS && player_flashlight != nullptr)
	{
		flashlight_color_toggle = !flashlight_color_toggle;
		player_flashlight->setLightColor(flashlight_color_toggle);
		if(flashlight_color_toggle)
			PRINTNOTE("Flashlight Red")
		else
			PRINTNOTE("Flashlight Not Red")
	}
}

void GraphXPlayer::doMovement(int direction[2])
{
	if(jph_character == nullptr)
		return;
	JPH::Vec3 current_velocity = jph_character->GetLinearVelocity();
	JPH::Vec3 wish_velocity = JPH::Vec3(0.0f, 0.0f, 0.0f);
	wish_velocity += convertMath<JPH::Vec3>(orientation_grounded_front) * static_cast<float>(direction[0] * movement_speed);
	wish_velocity += convertMath<JPH::Vec3>(orientation_right) * static_cast<float>(direction[1] * movement_speed);

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

	JPH::Vec3 new_velocity = linearInterpolate(current_velocity, wish_velocity, movement_lerp);
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

bool GraphXPlayer::wantsToBeRendered()
{
	return false;
}

void GraphXPlayer::takeABow()
{
	Actor::takeABow();
	jolt_physics_system.GetBodyInterface().RemoveBody(jph_character->GetBodyID());
}

//
// Light
//
Light::Light(std::string init_name, float init_intensity, float init_range, float init_falloff, float init_strength, glm::vec3 init_color, glm::vec3 init_position, glm::vec3 init_rotation, glm::vec3 init_scale)
: Actor(init_name, &temporary_light_mesh, init_position, init_rotation, init_scale), light_color(init_color), light_strength(init_strength), range(init_range), intensity(init_intensity), falloff(init_falloff)
{
	my_type = graphx::classes::LIGHT;
	my_light_type = graphx::classes::LIGHT;
	scale = glm::vec3(0.2f);
}

void Light::youGotACallBack(graphx::gSettings new_settings)
{
	Actor::youGotACallBack(new_settings);

	getSetting(light_color, settings["Color"]);
	getSetting(light_strength, settings["Strength"]);
	getSetting(light_ambient_strength, settings["AmbientStrength"]);
	getSetting(range, settings["Range"]);
	getSetting(intensity, settings["Intensity"]);
	getSetting(falloff, settings["Falloff"]);

	if(mesh != nullptr)
	{
		mesh->prepForDestruction();
		mesh = nullptr;
		delete mesh;
	}

	temporary_light_mesh.material = new Material(LIGHT_jpg, NO_TEXTURE_jpg, 4, 0.0f, light_color);
	mesh = &temporary_light_mesh;
}

bool Light::isLightType(int light_type)
{
	return light_type == my_light_type;
}

//
// LightDirectional
//
LightDirectional::LightDirectional(std::string init_name, glm::vec3 init_direction, float init_strength, glm::vec3 init_color)
: Light(init_name, 1.0f, 100.0f, 0.0f, init_strength, init_color), direction(init_direction)
{
	my_type = graphx::classes::LIGHTDIRECTIONAL;
	my_light_type = graphx::classes::LIGHTDIRECTIONAL;
}

void LightDirectional::youGotACallBack(graphx::gSettings new_settings)
{
	Light::youGotACallBack(new_settings);

	getSetting(direction, settings["Direction"]);

	if(mesh != nullptr)
		mesh->prepForDestruction();
	mesh = nullptr;
	delete mesh;
}

//
// LightSpot
//
LightSpot::LightSpot(std::string init_name, float init_intensity, float init_range, float init_falloff, float init_strength, glm::vec3 init_color, float init_inner_cutoff_angle, float init_outer_cutoff_angle, glm::vec3 init_direction, glm::vec3 init_position, glm::vec3 init_rotation)
: Light(init_name, init_intensity, init_range, init_falloff, init_strength, init_color, init_position, init_rotation), direction(init_direction), inner_cutoff_angle(init_inner_cutoff_angle), outer_cutoff_angle(init_outer_cutoff_angle)
{
	my_type = graphx::classes::LIGHTSPOT;
	my_light_type = graphx::classes::LIGHTSPOT;
}

void LightSpot::youGotACallBack(graphx::gSettings new_settings)
{
	Light::youGotACallBack(new_settings);

	getSetting(inner_cutoff_angle, settings["InnerCutoffAngle"]);
	getSetting(outer_cutoff_angle, settings["OuterCutoffAngle"]);
	getSetting(direction, settings["Direction"]);
}

glm::vec2 LightSpot::getCutoffAngles()
{
	return glm::vec2
	{
		glm::cos(glm::radians(inner_cutoff_angle)),
		glm::cos(glm::radians(outer_cutoff_angle)),
	};
}

//
// LightFlashlight
//
LightFlashlight::LightFlashlight(std::string init_name, float init_intensity, float init_range, float init_falloff, float init_strength, glm::vec3 init_color, float init_inner_cutoff_angle, float init_outer_cutoff_angle, glm::vec3 init_position_offset, glm::vec3 init_rotation_offset)
: LightSpot(init_name, init_intensity, init_range, init_falloff, init_strength, init_color, init_inner_cutoff_angle, init_outer_cutoff_angle), position_offset(init_position_offset), rotation_offset(init_rotation_offset), _intensity(init_intensity)
{
	my_type = graphx::classes::LIGHTFLASHLIGHT;
	my_light_type = graphx::classes::LIGHTSPOT;
	_color = light_color;
}

void LightFlashlight::youGotACallBack(graphx::gSettings new_settings)
{
	Light::youGotACallBack(new_settings);

	getSetting(position_offset, settings["PositionOffset"]);
	getSetting(rotation_offset, settings["RotationOffset"]);

	_color = light_color;
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

	position_global = getCurrentTheatre()->getPlayer()->player_camera.getPosition<glm::vec3>() + position_offset;
	quaternion = getCurrentTheatre()->getPlayer()->player_camera.getRotation<glm::quat>() * glm::quat(glm::radians(rotation_offset));
	direction = quaternion * vector3_front;
}

void LightFlashlight::setLight(bool is_off)
{
	intensity = _intensity + (100.0f * is_off);
}

void LightFlashlight::setLightColor(glm::vec3 color)
{
	light_color = color;
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
LightTesterMover::LightTesterMover(std::string init_name, glm::vec3 init_pivot_position, float init_pivot_radius, float init_pivot_speed, float init_intensity, float init_range, float init_falloff, float init_strength, glm::vec3 init_color)
: Light(init_name, init_intensity, init_range, init_falloff, init_strength, init_color), pivot_position(init_pivot_position), pivot_radius(init_pivot_radius), pivot_speed(init_pivot_speed)
{
	my_type = graphx::classes::LIGHTTESTERMOVER;
	my_light_type = graphx::classes::LIGHTSPOT;
}

void LightTesterMover::youGotACallBack(graphx::gSettings new_settings)
{
	Light::youGotACallBack(new_settings);

	getSetting(pivot_position, settings["PivotPosition"]);
	getSetting(pivot_radius, settings["PivotRadius"]);
	getSetting(pivot_speed, settings["PivotSpeed"]);

	pivot_point.setGlobalPosition(pivot_position);
	pivot_point.setName("Pivot point Actor for " + name + " LightTesterMover (UID: " + std::to_string(UID) + ")");
	pivot_point.mesh->setName("Pivot Mesh for " + name + " LightTesterMover (UID: " + std::to_string(UID) + ")");
	getCurrentTheatre()->actorEnter(&pivot_point, 4815 + UID, settings);
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
