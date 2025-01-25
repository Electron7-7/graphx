#ifndef GRAPHX_DEVICES
#define GRAPHX_DEVICES
#include "g_common.hpp"

struct Collider : public Device
{
	glm::vec3 scale;
	glm::vec3 position;

	bool sleeping = true;

	Collider(glm::vec3 init_scale = glm::vec3(1.0f), glm::vec3 init_position = glm::vec3(0.0f))
	: scale(init_scale), position(init_position)
	{ type = DEVICE_COLLIDER; }

	Collider(Device &sample)
	{
		type = DEVICE_COLLIDER;
		scale = glm::vec3(1.0f);
		position = glm::vec3(0.0f);
	}

	Collider(Collider &sample)
	{
		type = DEVICE_COLLIDER;
		scale = sample.scale;
		position = sample.position;
	}

	bool checkCollision(Collider *other_collider);
};
#endif