#include "sanity.hpp"
#include "g_actors.hpp"
#include <iostream>

//
// Actor
//
Actor::Actor(const char *new_name, glm::vec3 init_position, glm::vec3 init_up, float init_yaw, float init_pitch) : orientation_front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(INIT_SPEED)
{
	name = new_name;
	position_global = init_position;
	world_orientation_up = init_up;
	yaw = init_yaw;
	pitch = init_pitch;

	updateActorVectors();
}

void Actor::updateActorVectors()
{
	glm::vec3 new_front;
	new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	new_front.y = sin(glm::radians(pitch));
	new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	
	orientation_front = glm::normalize(new_front);
	orientation_right = glm::normalize(glm::cross(orientation_front, world_orientation_up));
	orientation_up = glm::normalize(glm::cross(orientation_right, orientation_front));
}

void Actor::doMovement(int direction[2], float delta_time)
{
	position_global += orientation_front * static_cast<float>(direction[0] * movement_speed * delta_time);
	position_global += orientation_right * static_cast<float>(direction[1] * movement_speed * delta_time);

	// std::cout << "Position: " << glm::to_string(position_global) << std::endl;
}

//
// GraphXPlayer
//
GraphXPlayer::GraphXPlayer(const char *new_name, glm::vec3 init_position) : Actor(new_name, init_position), mouse_sensitivity(INIT_SENSITIVITY)
{}

void GraphXPlayer::doMouseMovement(float offset[2], GLboolean constrain_pitch)
{
	offset[0] *= mouse_sensitivity;
	offset[1] *= mouse_sensitivity;

	yaw += offset[0];
	pitch += offset[1];

	if(constrain_pitch)
	{
		if(std::abs(pitch) > 89.0f)
			pitch = 89.0f * ((pitch > 0) - (pitch < 0));
	}
	updateActorVectors();
}

glm::mat4 GraphXPlayer::getViewMatrix()
{
	return glm::lookAt(position_global, position_global + orientation_front, orientation_up);
}