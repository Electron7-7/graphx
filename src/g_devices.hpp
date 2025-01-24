#ifndef GRAPHX_DEVICES
#define GRAPHX_DEVICES
#include "sanity.hpp"
class Actor; // Forward-declare Actor

struct Collider
{
	glm::vec3 top_left_back;
	glm::vec3 bottom_right_front;
	bool sleeping = true;

	Collider(glm::vec3 init_top_left_back, glm::vec3 init_bottom_right_front)
	: top_left_back(init_top_left_back), bottom_right_front(init_bottom_right_front)
	{}

	bool checkCollision(Collider *other_collider);
};
#endif