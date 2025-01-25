#include "g_common.hpp"
#include "g_devices.hpp"
#include "g_actors.hpp"
std::vector<std::vector<Actor *>> to_be_collided;

// CHANGE THIS TO AVOID CHECKING EVERY SINGLE ACTOR IN THE THEATRE
// USE SECTORS OR SOME OTHER FORM OF GROUPING
void P_CheckCollisions(std::vector<Actor *> troupe)
{
	to_be_collided.clear();
	Collider *collider_1 = NULL;
	Collider *collider_2 = NULL;

	for(Actor *iterator_actor : troupe)
	{
		if(iterator_actor->getDevice(DEVICE_COLLIDER) != NULL)
			collider_1 = static_cast<Collider *>(iterator_actor->getDevice(DEVICE_COLLIDER));
		else
			continue;

		// For now, pretend that every Actor in the Theatre is
		// actually just every Actor in a "collision sector"
		std::vector<Actor *> stooges;
		for(Actor *checking_actor : troupe)
		{
			if(iterator_actor->name == checking_actor->name)
				continue;

			if(checking_actor->getDevice(DEVICE_COLLIDER) != NULL)
			{
				collider_2 = static_cast<Collider *>(checking_actor->getDevice(DEVICE_COLLIDER));
				if(!collider_2->sleeping)
					continue;
			}
			else
				continue;

			if(collider_1->checkCollision(collider_2))
				stooges.insert(stooges.end(), checking_actor);
		}

		if(stooges.size() <= 0)
			continue;

		stooges.insert(stooges.begin(), iterator_actor);
		to_be_collided.insert(to_be_collided.end(), stooges);
	}

	for(std::vector<Actor *> collision_buffer : to_be_collided)
		for(Actor *actor : collision_buffer)
			if(actor->getDevice(DEVICE_COLLIDER) != NULL)
			{
				static_cast<Collider *>(actor->getDevice(DEVICE_COLLIDER))->sleeping = true;
				static_cast<PhysicsActor *>(actor)->falling = false; // Testing; make this not a thing, lol
				// Print stuff for debugging
				// std::string collider_position = glm::to_string(static_cast<Collider *>(actor->getDevice(DEVICE_COLLIDER))->position);
				// std::string collider_scale = glm::to_string(static_cast<Collider *>(actor->getDevice(DEVICE_COLLIDER))->scale);
				// PRINT(actor->name << "'s Collider:\n\t- Position: " << collider_position << "\n\t- Scale: " << collider_scale);
			}
}

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
// NOT FINISHED!!
/*glm::vec3 P_CalculateInelasticCollision(Actor *stooge_left, Actor *stooge_right)
{
	glm::vec3 top_half = (stooge_left->mass * stooge_left->velocity) + (stooge_right->mass * stooge_right->velocity);
	float bottom_half = (stooge_left->mass + stooge_right->mass);
	glm::vec3 final_velocity = top_half / bottom_half;

	return final_velocity;
}*/