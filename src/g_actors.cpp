#include "g_actors.hpp"

//
// Actor
//
void Actor::updateRotation(bool override_which)
{
	if(override_which == EULER_CHANGE_QUATERNION)
	{
		rotation_quaternion = glm::quat(rotation_euler);
		return;
	}
	rotation_euler = glm::eulerAngles(rotation_quaternion);
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

void Actor::Tick(int current_tick)
{}

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

//
// Lights
//
void LightActorMoving::Tick(int current_tick)
{
	if( (position_global.z >= (starting_position.z + 10.0f)) || (position_global.z <= (starting_position.z - 10.0f)) )
		t_direction = 1 - t_direction;

	if(t_direction == 0)
		position_global.z += movement_speed;

	if(t_direction == 1)
		position_global.z -= movement_speed;
}

void LightActorControllable::doHorizontalMovement(int direction[2])
{
	position_global[0] -= static_cast<float>(direction[1] * _movement_speed);
	position_global[2] -= static_cast<float>(direction[0] * _movement_speed);
}

void LightActorControllable::doVerticalMovement(int direction)
{
	position_global[1] += static_cast<float>(direction * _movement_speed);
}