#include "sanity.hpp"
#include "g_common.hpp"

void Collider::bufferCollision(std::vector<Actor *> who_got_bonked)
{
	sleeping = false;
	CollisionBuffer collision_buffer(who_got_bonked);
}