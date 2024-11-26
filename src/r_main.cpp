// r_main.cpp - GraphX-specific rendering
#include "sanity.hpp"
#include "r_common.hpp"

//
// Cube
//
void Cube::makeCube()
{
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Idea: set GL_FALSE to GL_TRUE and use ints instead of floats for more efficient storage?
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void Cube::drawCube()
{
	glDrawElements(GL_TRIANGLES, sizeof(indices), GL_UNSIGNED_INT, 0);
}

//
// Debug Camera (not for in-game use)
//
DebugCamera::DebugCamera(glm::vec3 init_position, glm::vec3 init_up, float init_yaw, float init_pitch) : front(glm::vec3(0.0f, 0.0f, -1.0f)), movement_speed(SPEED), mouse_sensitivity(SENSITIVITY)
{
	position = init_position;
	world_up = init_up;
	yaw = init_yaw;
	pitch = init_pitch;

	updateCameraVectors();
}

glm::mat4 DebugCamera::getViewMatrix()
{
	return glm::lookAt(position, position + front, up);
}

void DebugCamera::doMovement(int direction[2], float delta_time)
{
	position += front * static_cast<float>(direction[0] * movement_speed * delta_time);
	position += right * static_cast<float>(direction[1] * movement_speed * delta_time);
}

void DebugCamera::doMouseMovement(float offset[2], GLboolean constrain_pitch)
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
	updateCameraVectors();
}

void DebugCamera::updateCameraVectors()
{
	glm::vec3 new_front;
	new_front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	new_front.y = sin(glm::radians(pitch));
	new_front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	
	front = glm::normalize(new_front);
	right = glm::normalize(glm::cross(front, world_up));
	up = glm::normalize(glm::cross(right, front));
}