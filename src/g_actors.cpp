#include "g_actors.hpp"
#include "r_common.hpp"
#include <vector>

//
// Actor
//
Actor::Actor(const char *new_name, Mesh init_mesh, glm::vec3 init_position, float init_yaw, float init_pitch)
: render_command(mesh.vao_id) current_state(RenderState()), movement_speed(INIT_SPEED), orientation_front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	name = new_name;
	position_global = init_position;
	world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);
	rotation_euler.y = init_yaw;
	rotation_euler.x = init_pitch;
	rotation_euler.z = 0.0f; // roll
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

void Actor::updateStates()
{
	// Copy current state into previous state
	previous_state_buffer[state_index] = current_state_buffer[state_index];

	// Update current state
	current_state_buffer[state_index].render_position = position_global;
	current_state_buffer[state_index].render_rotation_euler = rotation_euler;

	// Flip state buffer
	state_index = 1 - state_index;
}

void Actor::Tick()
{
	updateStates();
}

//
// GraphXPlayer
//
void GraphXPlayer::doMovement(int direction[2], float delta_time)
{
	position_global += orientation_front * static_cast<float>(direction[0] * movement_speed * delta_time);
	position_global += orientation_right * static_cast<float>(direction[1] * movement_speed * delta_time);
}

void GraphXPlayer::doMouseMovement(float offset[2], GLboolean constrain_pitch)
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

//
// Tester
//
void Tester::Tick()
{
	position_flip = 1 - position_flip;
	position_global = testing_position[position_flip];

	// Actor::Tick(); // If/when generic Actors have code written to their Tick function, I'll probably want that code to run in any derived class; this is how you call that code
}