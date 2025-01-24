#ifndef GRAPHX_ENGINE_COMMON
#define GRAPHX_ENGINE_COMMON
#include "sanity.hpp"
#include "g_devices.hpp"
class Actor; // Forward-declare Actor

/*
	Inelastic Collision Formula (https://byjus.com/physics/inelastic-collision)

	When two objects collide under inelastic conditions,
	the final velocity with which the object moves is
	given by:
		V = (M1V1 + M2V2) / (M1 + M2)

	Where:
		- V  = final velocity
		- M1 = mass of the first object in kgs
		- M2 = mass of the second object in kgs
		- V1 = initial velocity of the first object in m/s
		- V2 = initial velocity of the second object in m/s
*/

void P_CheckCollisions(std::vector<Actor *> troupe);
glm::vec3 P_CalculateInelasticCollision(Actor *stooge_left, Actor *stooge_right); // Naming colliding Actors "stooges"

extern std::vector<std::vector<Actor *>> to_be_collided;
#endif