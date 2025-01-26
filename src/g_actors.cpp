#include "g_common.hpp"
#include "g_actors.hpp"
#include "g_devices.hpp"
#include "g_theatre.hpp"
#include <unordered_map>

GraphXPlayer *current_player = NULL;

//
// Actor
//
void Actor::updateRotation(bool override_which)
{
	if(override_which == EULER_CHANGE_QUATERNION)
	{
		rotation_quaternion = JPH::Quat::sEulerAngles(convertMath<JPH::Vec3>(rotation_euler));
		return;
	}

	JPH::Vec3 temp_rotation_euler = rotation_quaternion.GetEulerAngles();
	rotation_euler = convertMath<glm::vec3>(temp_rotation_euler);
}

void Actor::updateVectors()
{
	glm::vec3 new_front;
	new_front[0] = cos(glm::radians(rotation_euler[1])) * cos(glm::radians(rotation_euler[0]));
	new_front[1] = sin(glm::radians(rotation_euler[0]));
	new_front[2] = sin(glm::radians(rotation_euler[1])) * cos(glm::radians(rotation_euler[0]));
	
	orientation_front = glm::normalize(new_front);
	orientation_right = glm::normalize(glm::cross(orientation_front, world_orientation_up));
	orientation_up = glm::normalize(glm::cross(orientation_right, orientation_front));
}

void Actor::updateStates(std::mutex &state_mutex)
{
	std::lock_guard guard(state_mutex);

	// Copy current state into previous state
	previous_state_buffer[state_index] = current_state_buffer[state_index];

	// Update current state
	current_state_buffer[state_index].render_position	=	position_global;
	current_state_buffer[state_index].render_quaternion	=	rotation_quaternion;
	current_state_buffer[state_index].render_scale		=	scale;

	// Flip state buffer
	state_index = 1 - state_index;
}

int Actor::giveDevice(Device *new_device)
{
	switch (new_device->type)
	{
		case DEVICE_DEVICE:
			return -1;
			break;

		case DEVICE_COLLIDER:
			static_cast<Collider *>(new_device)->scale = scale;
			static_cast<Collider *>(new_device)->position = position_global;
			break;
	}

	devices.insert(std::make_pair(new_device->type, new_device));
	return 0;
}

Device *Actor::getDevice(unsigned int device_type)
{
	if(auto pair = devices.find(device_type) ; pair != devices.end())
		return pair->second;

	return NULL;
}

void Actor::Tick(int current_tick)
{}

void Actor::init(Theatre *parent_theatre)
{
	rotation_quaternion = JPH::Quat::sEulerAngles(convertMath<JPH::Vec3>(rotation_euler));
}

bool Actor::wantsToBeRendered()
{
	return ( (actor_type != ACTOR_TOOL && visible) || debug_visible );
}

bool Actor::wantsToBeBuffered()
{
	return( (actor_type != ACTOR_TOOL) || debug_visible );
}

//
// PhysicsActor
//
void PhysicsActor::init(Theatre *parent_theatre)
{
	Actor::init(parent_theatre);
	box_settings = JPH::BodyCreationSettings(new JPH::BoxShape(JPH::Vec3(scale[0], scale[1], scale[2])), JPH::RVec3(JPH::Real3(position_global[0], position_global[1], position_global[2])), rotation_quaternion, JPH::EMotionType::Dynamic, Layers::MOVING);
}

void PhysicsActor::Tick(int current_tick)
{
	JPH::BodyInterface &body_interface = physics_system->GetBodyInterface();
	position_global = glm::vec3(body_interface.GetCenterOfMassPosition(physics_body_id)[0], body_interface.GetCenterOfMassPosition(physics_body_id)[1], body_interface.GetCenterOfMassPosition(physics_body_id)[2]);
	rotation_quaternion = body_interface.GetRotation(physics_body_id);
	updateRotation(QUATERNION_CHANGE_EULER);
}

void PhysicsActor::reset_to_initial_orientation_for_testing()
{
	JPH::BodyInterface &body_interface = physics_system->GetBodyInterface();
	body_interface.SetPositionAndRotation(physics_body_id, JPH::RVec3(convertMath<JPH::Vec3>(position_global)), reset_quaternion, JPH::EActivation::Activate);
	body_interface.SetLinearAndAngularVelocity(physics_body_id, JPH::Vec3::sZero(), JPH::Vec3::sZero());
}

//
// GraphXPlayer
//
void GraphXPlayer::doMovement(int direction[2])
{
	position_global += orientation_front * static_cast<float>(direction[0] * movement_speed);
	position_global += orientation_right * static_cast<float>(direction[1] * movement_speed);
}

void GraphXPlayer::doMouseMovement(std::vector<float> offset, bool constrain_pitch)
{
	offset[0] *= mouse_sensitivity;
	offset[1] *= mouse_sensitivity;

	rotation_euler.y += offset[0];
	rotation_euler.x += offset[1];

	if(constrain_pitch)
	{
		if(std::abs(rotation_euler.x) > 89.0f)
			rotation_euler.x = 89.0f * ((rotation_euler.x > 0) - (rotation_euler.x < 0));
	}

	updateRotation(EULER_CHANGE_QUATERNION);
	updateVectors();
}

glm::mat4 GraphXPlayer::getViewMatrix()
{
	return glm::lookAt(position_global, position_global + orientation_front, orientation_up);
}

bool GraphXPlayer::wantsToBeRendered()
{
	return false;
}

void GraphXPlayer::init(Theatre *parent_theatre)
{
	box_settings = JPH::BodyCreationSettings(new JPH::BoxShape(JPH::Vec3(scale[0], scale[1], scale[2])), JPH::RVec3(JPH::Real3(position_global[0], position_global[1], position_global[2])), rotation_quaternion, JPH::EMotionType::Dynamic, Layers::MOVING);
	physics_body_id = physics_system->GetBodyInterface().CreateAndAddBody(box_settings, JPH::EActivation::Activate);
}

void GraphXPlayer::Tick(int current_tick)
{
	JPH::BodyInterface &body_interface = physics_system->GetBodyInterface();
	body_interface.SetPositionAndRotation(physics_body_id, JPH::RVec3(JPH::Real3(position_global[0], position_global[1], position_global[2])), rotation_quaternion, JPH::EActivation::Activate);
}

//
// Testers
//
void MoverTester::Tick(int current_tick)
{
	if( (position_global.x >= 3.0f) || (position_global.x <= -3.0f) )
		t_direction = 1 - t_direction;

	if(t_direction == 0)
		position_global.x += movement_speed;

	if(t_direction == 1)
		position_global.x -= movement_speed;
}

void ControlledTester::Tick(int current_tick)
{
	PhysicsActor::Tick(current_tick);

	position_global[2] -= static_cast<float>(movement_direction[0] * movement_speed);
	position_global[0] += static_cast<float>(movement_direction[1] * movement_speed);
	position_global[1] += static_cast<float>(movement_direction[2] * movement_speed);
}

//
// Lights
//
glm::vec2 LightSpot::getCutoffAngles()
{
	return glm::vec2
	{
		glm::cos(glm::radians(inner_cutoff_angle)),
		glm::cos(glm::radians(outer_cutoff_angle)),
	};
}

void LightFlashlight::Tick(int current_tick)
{
	if(parent == NULL)
	{
		if(current_player == NULL)
			return;
		parent = current_player;
	}

	position_global = parent->position_global + position_offset;
	rotation_euler = parent->rotation_euler + rotation_offset;
	updateVectors();
	updateRotation(EULER_CHANGE_QUATERNION);
	direction = orientation_front;
}

void LightFlashlight::setLight(bool is_off)
{
	intensity = _intensity + (100.0f * is_off);
}

void LightTesterMover::Tick(int current_tick)
{
	pivot_point.position_global = pivot_position;

	position_global[0] = pivot_position[0] + pivot_radius * glm::cos(glm::radians(pivot_theta));
	position_global[1] = pivot_position[1];
	position_global[2] = pivot_position[2] + pivot_radius * glm::sin(glm::radians(pivot_theta));

	pivot_theta += pivot_speed;
	if(pivot_theta >= 360.0f)
		pivot_theta = 0.0f;
}

void LightTesterMover::init(Theatre *parent_theatre)
{
	parent_theatre->actorEnter(&pivot_point);
}