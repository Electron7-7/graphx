#include "sanity.hpp"
#include "g_common.hpp"

bool Collider::checkCollision(Collider *other_collider)
{
	bool axis_X = bottom_right_front[0] >= other_collider->top_left_back[0] && other_collider->bottom_right_front[0] >= top_left_back[0];
	bool axis_Y = bottom_right_front[1] >= other_collider->top_left_back[1] && other_collider->bottom_right_front[1] >= top_left_back[1];
	bool axis_Z = bottom_right_front[2] >= other_collider->top_left_back[2] && other_collider->bottom_right_front[2] >= top_left_back[2];

	sleeping = !(axis_X && axis_Y && axis_Z);
	other_collider->sleeping = sleeping;
	return !sleeping;
}