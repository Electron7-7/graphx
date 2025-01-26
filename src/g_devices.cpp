#include "g_devices.hpp"

bool Collider::checkCollision(Collider *other_collider)
{
	glm::vec3 top_left_back = position - scale;
	glm::vec3 bottom_right_front = position + scale;

	glm::vec3 other_top_left_back = other_collider->position - other_collider->scale;
	glm::vec3 other_bottom_right_front = other_collider->position + other_collider->scale;

	bool axis_X = bottom_right_front[0] >= other_top_left_back[0] && other_bottom_right_front[0] >= top_left_back[0];
	bool axis_Y = bottom_right_front[1] >= other_top_left_back[1] && other_bottom_right_front[1] >= top_left_back[1];
	bool axis_Z = bottom_right_front[2] >= other_top_left_back[2] && other_bottom_right_front[2] >= top_left_back[2];

	sleeping = !(axis_X && axis_Y && axis_Z);
	other_collider->sleeping = sleeping;
	return !sleeping;
}