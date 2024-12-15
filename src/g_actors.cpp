#include "g_actors.hpp"
#include "r_common.hpp"
#include <vector>
#include <mutex>
#include <iostream>

//
// Actor
//
Actor::Actor(const char *new_name, Mesh init_mesh, glm::vec3 init_position, float init_yaw, float init_pitch)
: mesh(init_mesh), vao_id(mesh.vao_id), movement_speed(INIT_SPEED), orientation_front(glm::vec3(0.0f, 0.0f, -1.0f))
{
	name = new_name;
	position_global = init_position;
	world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);
	rotation_euler = glm::vec3(init_pitch, init_yaw, 0.0f);
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

void Actor::updateStates()
{
	// if(strcmp(typeid(*this).name(), "11MoverTester") == 0)
	// {
	// 	std::cout << std::endl << std::endl << "Game Logic is now updating an Actor's state!" << std::endl << "State Index is currently: " << state_index << std::endl;
	// 	std::cout << "Previous State Buffer render_position: " << glm::to_string(previous_state_buffer[state_index].render_position) << std::endl;
	// 	std::cout << "Current State Buffer render_position: " << glm::to_string(current_state_buffer[state_index].render_position) << std::endl << std::endl;
	// }

	// Copy current state into previous state
	previous_state_buffer[state_index] = current_state_buffer[state_index];

	// Update current state
	current_state_buffer[state_index].render_position = position_global;
	current_state_buffer[state_index].render_rotation_euler = rotation_euler;

	// if(strcmp(typeid(*this).name(), "11MoverTester") == 0)
	// {
	// 	std::cout << "Game Logic is finished updating an Actor's state!" << std::endl << "State Index is currently: " << state_index << std::endl;
	// 	std::cout << "Previous State Buffer render_position: " << glm::to_string(previous_state_buffer[state_index].render_position) << std::endl;
	// 	std::cout << "Current State Buffer render_position: " << glm::to_string(current_state_buffer[state_index].render_position) << std::endl << std::endl;
	// }

	// Flip state buffer
	state_index = 1 - state_index;

	// if(strcmp(typeid(*this).name(), "11MoverTester") == 0)
	// {
	// 	std::cout << "State Index flipped!" << std::endl << "State Index is currently: " << state_index << std::endl;
	// 	std::cout << "Previous State Buffer render_position: " << glm::to_string(previous_state_buffer[state_index].render_position) << std::endl;
	// 	std::cout << "Current State Buffer render_position: " << glm::to_string(current_state_buffer[state_index].render_position) << std::endl << std::endl;
	// }
}

void Actor::Tick()
{

}

//
// GraphXPlayer
//
void GraphXPlayer::doMovement(int direction[2], float delta_time)
{
	position_global += orientation_front * static_cast<float>(direction[0] * movement_speed * delta_time);
	position_global += orientation_right * static_cast<float>(direction[1] * movement_speed * delta_time);
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

void GraphXPlayer::Tick()
{}

//
// Testers
//
void FlipperTester::Tick()
{
	position_flip = 1 - position_flip;
	position_global = testing_position[position_flip];

	Actor::Tick();
}


void MoverTester::Tick()
{
	// std::cout << "\n\nMover Tester is moving! (Tick function called)\nGlobal Position: " << glm::to_string(position_global);

	if( (position_global.x >= 3.0f) || (position_global.x <= -3.0f) )
	{
		t_direction = 1 - t_direction;
	}
	if(t_direction == 0)
	{
		position_global.x += t_movement_speed;
	}
	if(t_direction == 1)
	{
		position_global.x -= t_movement_speed;
	}

	// std::cout << "\n\nMover Tester finished moving! (Tick function finished)\nGlobal Position: " << glm::to_string(position_global);
}