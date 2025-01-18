#include "g_actors.hpp"

GraphXPlayer *current_player = NULL;

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

void Actor::init(Theatre *parent_theatre)
{}

bool Actor::wantsToBeRendered()
{
	return (actor_type != ACTOR_TOOL && visible) || debug_visible;
}

bool Actor::wantsToBeBuffered()
{
	return( (actor_type != ACTOR_TOOL) || debug_visible );
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
	// updateRotation(EULER_CHANGE_QUATERNION);
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