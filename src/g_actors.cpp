#include "g_actors.hpp"

//
// Actor
//
Actor::Actor(std::string new_name, Mesh init_mesh, glm::vec3 init_position, glm::vec3 init_scale, float init_yaw, float init_pitch)
: mesh(init_mesh), orientation_front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	name = new_name;
	world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);
	position_global = init_position;
	rotation_euler = glm::vec3(init_pitch, init_yaw, 0.0f);
	scale = init_scale;
	current_state = RenderState(init_position, rotation_euler);
	updateVectors();
}

void Actor::updateVectors()
{
	glm::vec3 new_front;
	new_front.x = cos(glm::radians(rotation_euler.y)) * cos(glm::radians(rotation_euler.x));
	new_front.y = sin(glm::radians(rotation_euler.x));
	new_front.z = sin(glm::radians(rotation_euler.y)) * cos(glm::radians(rotation_euler.x));
	
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
	current_state_buffer[state_index].render_position = position_global;
	current_state_buffer[state_index].render_rotation_euler = rotation_euler;

	// Flip state buffer
	state_index = 1 - state_index;
}

void Actor::Tick(int current_tick)
{}

bool Actor::gatekeepRenderer()
{
	return (!visible); // Todo: account for Sprites
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

	updateVectors();
}

glm::mat4 GraphXPlayer::getViewMatrix()
{
	return glm::lookAt(position_global, position_global + orientation_front, orientation_up);
}

bool GraphXPlayer::gatekeepRenderer() // NOT MULTIPLAYER FRIENDLY (but I'm not touching that with a ten foot pole... yet)
{
	return true;
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