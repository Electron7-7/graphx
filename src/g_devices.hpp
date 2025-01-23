#ifndef GRAPHX_DEVICES
#define GRAPHX_DEVICES
#include "sanity.hpp"
class Actor; // Forward-declare Actor

struct Collider
{
	glm::vec3 position;
	glm::vec3 size;
	bool sleeping = true;

	Collider(glm::vec3 init_size, glm::vec3 init_position)
	: position(init_position), size(init_size * 2)
	{}

	void bufferCollision(std::vector<Actor *> who_got_bonked);
	// void Tick(int current_tick);
};
#endif