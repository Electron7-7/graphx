#include "g_common.hpp"
#include "t_common.hpp"
#include "g_actors.hpp"
#include "g_jolt.hpp"
#include "g_math.hpp"
#include <vector>
#include <Jolt/Physics/Collision/Shape/RotatedTranslatedShape.h>

using namespace graphx;

// GraphXPlayer *current_player = NULL;
long current_player_uid = -1;
glm::vec3 vector3_up = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 vector3_front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 vector3_right = glm::vec3(1.0f, 0.0f, 0.0f);
std::unordered_map<double, Actor *> actor_uid_lookup;

GraphXPlayer *getCurrentPlayer()
{
	// if(current_player_uid == -1)
	return new GraphXPlayer();
}

//
// RenderState
//
RenderState::RenderState(glm::vec3 init_position, glm::quat init_quaternion, glm::vec3 init_scale)
: render_position(init_position), render_quaternion(init_quaternion), render_scale(init_scale)
{}

//
// Actor
//
Actor::Actor(std::string new_name, Mesh *init_mesh, glm::vec3 init_position, glm::vec3 init_euler_degrees, glm::vec3 init_scale)
: mesh(init_mesh), position_global(init_position), scale(init_scale), orientation_front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	actor_type = ACTOR_ACTOR;
	name = new_name;
	world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);
	quaternion = glm::quat(glm::radians(init_euler_degrees));
	current_state = RenderState(init_position, quaternion, init_scale);
	current_state_copy = current_state;
	previous_state = current_state;
	previous_state_copy = current_state;
	current_state_buffer = { current_state, current_state_copy };
	previous_state_buffer = { previous_state, previous_state_copy };
	updateVectors();
}

void Actor::youGotACallBack(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;

	glm::vec3 rotation_degrees = glm::eulerAngles(quaternion);

	setRawData(name, new_settings["Name"]);
	setDevicePointer(mesh, new_settings["Mesh"]);
	setRawData(position_global, new_settings["Position"]);
	setRawData(rotation_degrees, new_settings["RotationDegrees"]);
	setRawData(scale, new_settings["Scale"]);

	quaternion = glm::quat(glm::radians(rotation_degrees));
}

std::string Actor::getType()
{
	return "Actor";
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
	current_state_buffer[state_index].render_position	=	position_global;
	current_state_buffer[state_index].render_quaternion	=	quaternion;
	current_state_buffer[state_index].render_scale		=	scale;

	// Flip state buffer
	state_index = 1 - state_index;
}

void Actor::tick(int current_tick)
{}

void Actor::callToStage(Theatre *parent_theatre)
{
	PRINTLN("\t- " << name << " UID #" << UID);
}

void Actor::takeABow()
{
	PRINTLN("\t- " << name << " UID #" << UID);
}

bool Actor::wantsToBeBuffered()
{
	if(actor_type == ACTOR_LIGHT)
		return (debug_visible);
	return(mesh != NULL);
}

bool Actor::wantsToBeRendered()
{
	return (Actor::wantsToBeBuffered() && visible);
}

//
// PhysicsActor
//
PhysicsActor::PhysicsActor(std::string init_name, Mesh *init_mesh, glm::vec3 init_position, glm::vec3 init_euler_degrees, glm::vec3 init_scale)
: Actor(init_name, init_mesh, init_position, init_euler_degrees, init_scale)
{
	actor_type = ACTOR_PHYSICS;
}

std::string PhysicsActor::getType()
{
	return "PhysicsActor";
}

void PhysicsActor::youGotACallBack(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;
	
	Actor::youGotACallBack(new_settings);

	setRawData(mass, new_settings["Mass"]);
	setDevicePointer(collider, new_settings["Collider"]);
}

void PhysicsActor::callToStage(Theatre *parent_theatre)
{
	Actor::callToStage(parent_theatre);
	collider->position = position_global;
	collider->quaternion = quaternion;
	collider->scale = scale;
	collider->createBody();
}

void PhysicsActor::takeABow()
{
	Actor::takeABow();
}

void PhysicsActor::tick(int current_tick)
{}

//
// RigidBodyActor
//
void RigidBodyActor::youGotACallBack(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;
	PhysicsActor::youGotACallBack(new_settings);
}

std::string RigidBodyActor::getType()
{
	return "RigidBodyActor";
}

void RigidBodyActor::callToStage(Theatre *parent_theatre)
{
	PhysicsActor::callToStage(parent_theatre);

	reset_position = convertMath<JPH::Vec3>(position_global);
	reset_quaternion = convertMath<JPH::Quat>(quaternion);
}

void RigidBodyActor::tick(int current_tick)
{
	PhysicsActor::tick(current_tick);

	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	JPH::Vec3 body_position = body_interface.GetCenterOfMassPosition(collider->getBodyID());
	JPH::Quat body_quaternion = body_interface.GetRotation(collider->getBodyID());

	position_global = convertMath<glm::vec3>(body_position);
	quaternion = convertMath<glm::quat>(body_quaternion);
	updateVectors();
}

void RigidBodyActor::reset_to_initial_orientation_for_testing()
{
	JPH::BodyInterface &body_interface = jolt_physics_system.GetBodyInterface();
	body_interface.SetPositionAndRotation(collider->getBodyID(), reset_position, reset_quaternion, JPH::EActivation::Activate);
	body_interface.SetLinearAndAngularVelocity(collider->getBodyID(), JPH::Vec3::sZero(), JPH::Vec3::sZero());
}

//
// Camera
//
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

std::string Camera::getType()
{
	return "Camera";
}

void Camera::youGotACallBack(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;
	Actor::youGotACallBack(new_settings);

	setActorPointer(parent, new_settings["Parent"]);
	setRawData(position_local, new_settings["LocalPosition"]);
	setRawData(euler_rotation_local, new_settings["LocalRotationDegrees"]);
}

void Camera::tick(int current_tick)
{
	position_global = parent->position_global + position_local;
}

//
// GraphXPlayer
//
GraphXPlayer::GraphXPlayer(std::string new_name, glm::vec3 init_position, glm::vec3 init_rotation_euler)
: Actor(new_name, &player_mesh, init_position, init_rotation_euler, glm::vec3(1.0f))
{
	actor_type = ACTOR_PLAYER;
	debug_visible = false;
	player_camera.euler_rotation = glm::radians(init_rotation_euler);
	player_camera.position_global = init_position;
	player_camera.parent = this;
}

std::string GraphXPlayer::getType()
{
	return "GraphXPlayer";
}

void GraphXPlayer::youGotACallBack(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;

	Actor::youGotACallBack(new_settings);

	setRawData(mouse_sensitivity, new_settings["MouseSensitivity"]);
	setRawData(movement_speed, new_settings["MovementSpeed"]);
	setRawData(lerp_speed, new_settings["MovementAcceleration"]);
	setRawData(friction, new_settings["Friction"]);

	lerp_speed *= (double)1.0 / 120; // Hardcoded until I move TICKLENGTH and TICKRATE out of main.cpp
}

void GraphXPlayer::callToStage(Theatre *parent_theatre)
{
	Actor::callToStage(parent_theatre);

	player_settings = new JPH::CharacterSettings;
	player_settings->mMaxSlopeAngle = JPH::DegreesToRadians(45.0f);
	player_settings->mLayer = Layers::MOVING;
	player_settings->mShape = JPH::RotatedTranslatedShapeSettings(JPH::Vec3::sZero(), JPH::Quat::sIdentity(), new JPH::CapsuleShape(scale[1], scale[0])).Create().Get();
	player_settings->mFriction = friction;
	player_settings->mSupportingVolume = JPH::Plane(JPH::Vec3::sAxisY(), scale[0]);
	jph_character = new JPH::Character(player_settings, convertMath<JPH::Vec3>(position_global), JPH::Quat::sIdentity(), 0, &jolt_physics_system);
	jph_character->AddToPhysicsSystem(JPH::EActivation::Activate);
}

void GraphXPlayer::tick(int current_tick)
{
	player_camera.tick(current_tick);
	position_global = convertMath<glm::vec3>(jph_character->GetPosition());
}

void GraphXPlayer::doMovement(int direction[2])
{
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
	return glm::lookAt(player_camera.position_global, player_camera.position_global + player_camera.orientation_front, player_camera.orientation_up);
}

bool GraphXPlayer::wantsToBeRendered()
{
	return false;
}

//
// Light
//
Light::Light(std::string init_name, float init_intensity, float init_range, float init_falloff, float init_strength, glm::vec3 init_color, glm::vec3 init_position, glm::vec3 init_rotation, glm::vec3 init_scale)
: Actor(init_name, &temporary_light_mesh, init_position, init_rotation, init_scale), light_color(init_color), light_strength(init_strength), range(init_range), intensity(init_intensity), falloff(init_falloff)
{
	actor_type = ACTOR_LIGHT;
	light_type = LIGHT_POINT;
	debug_visible = true;
}

std::string Light::getType()
{
	return "Light";
}

void Light::youGotACallBack(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;
	Actor::youGotACallBack(new_settings);

	setRawData(light_color, new_settings["Color"]);
	setRawData(light_strength, new_settings["Strength"]);
	setRawData(range, new_settings["Range"]);
	setRawData(intensity, new_settings["Intensity"]);
	setRawData(falloff, new_settings["Falloff"]);
}

//
// LightDirectional
//
LightDirectional::LightDirectional(std::string init_name, glm::vec3 init_direction, float init_strength, glm::vec3 init_color)
: Light(init_name, 1.0f, 100.0f, 0.0f, init_strength, init_color), direction(init_direction)
{ light_type = LIGHT_DIRECTIONAL; }

void LightDirectional::youGotACallBack(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;
	Light::youGotACallBack(new_settings);

	setRawData(direction, new_settings["Direction"]);
}

std::string LightDirectional::getType()
{
	return "LightDirectional";
}

//
// LightSpot
//
LightSpot::LightSpot(std::string init_name, float init_intensity, float init_range, float init_falloff, float init_strength, glm::vec3 init_color, float init_inner_cutoff_angle, float init_outer_cutoff_angle, glm::vec3 init_direction, glm::vec3 init_position, glm::vec3 init_rotation)
: Light(init_name, init_intensity, init_range, init_falloff, init_strength, init_color, init_position, init_rotation), direction(init_direction), inner_cutoff_angle(init_inner_cutoff_angle), outer_cutoff_angle(init_outer_cutoff_angle)
{
	light_type = LIGHT_SPOT;
}

std::string LightSpot::getType()
{
	return "LightSpot";
}

void LightSpot::youGotACallBack(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;
	Light::youGotACallBack(new_settings);

	setRawData(inner_cutoff_angle, new_settings["InnerCutoffAngle"]);
	setRawData(outer_cutoff_angle, new_settings["OuterCutoffAngle"]);
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
	light_type = LIGHT_SPOT;
	debug_visible = false;
}

std::string LightFlashlight::getType()
{
	return "LightFlashlight";
}

void LightFlashlight::youGotACallBack(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;
	Light::youGotACallBack(new_settings);

	setActorPointer(parent, new_settings["Parent"]);
	setRawData(position_offset, new_settings["PositionOffset"]);
	setRawData(rotation_offset, new_settings["RotationOffset"]);
}

void LightFlashlight::tick(int current_tick)
{
	// if(current_player == NULL)
	// 	return;

	// if(parent == NULL)
	// 	parent = getCurrentPlayer();

	position_global = parent->player_camera.position_global + position_offset;
	quaternion = parent->player_camera.quaternion * glm::quat(glm::radians(rotation_offset));
	direction = quaternion * vector3_front;
}

void LightFlashlight::setLight(bool is_off)
{
	intensity = _intensity + (100.0f * is_off);
}

//
// LightTesterMover
//
LightTesterMover::LightTesterMover(std::string init_name, glm::vec3 init_pivot_position, float init_pivot_radius, float init_pivot_speed, float init_intensity, float init_range, float init_falloff, float init_strength, glm::vec3 init_color)
: Light(init_name, init_intensity, init_range, init_falloff, init_strength, init_color), pivot_position(init_pivot_position), pivot_radius(init_pivot_radius), pivot_speed(init_pivot_speed)
{
	pivot_point.position_global = init_pivot_position;
}

std::string LightTesterMover::getType()
{
	return "LightTesterMover";
}

void LightTesterMover::youGotACallBack(graphx::gSettings new_settings)
{
	if(new_settings.contains("FUCKYOU"))
		new_settings = settings;

	Light::youGotACallBack(new_settings);

	setRawData(pivot_position, new_settings["PivotPosition"]);
	setRawData(pivot_radius, new_settings["PivotRadius"]);
	setRawData(pivot_speed, new_settings["PivotSpeed"]);
}

void LightTesterMover::tick(int current_tick)
{
	pivot_point.position_global = pivot_position;

	position_global[0] = pivot_position[0] + pivot_radius * glm::cos(glm::radians(pivot_theta));
	position_global[1] = pivot_position[1];
	position_global[2] = pivot_position[2] + pivot_radius * glm::sin(glm::radians(pivot_theta));

	pivot_theta += pivot_speed;
	if(pivot_theta >= 360.0f)
		pivot_theta = 0.0f;
}

void LightTesterMover::callToStage(Theatre *parent_theatre)
{
	// Manual UID created; check here if problems arise, just in case
	pivot_point.name = "Pivot point Actor for " + name + " LightTesterMover (UID: " + std::to_string(UID) + ")";
	pivot_point.mesh->name = "Pivot Mesh for " + name + " LightTesterMover (UID: " + std::to_string(UID) + ")";
	pivot_point.position_global = pivot_position;
	parent_theatre->actorEnter(&pivot_point, 4815 + UID);

	// pivot_point.callToStage(parent_theatre); // Might be calling callToStage() twice here, will have to test
}