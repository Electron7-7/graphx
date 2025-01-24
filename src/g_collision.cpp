#include "sanity.hpp"
#include "g_common.hpp"
#include "g_actors.hpp"
std::vector<std::vector<Actor *>> to_be_collided;

// CHANGE THIS TO AVOID CHECKING EVERY SINGLE ACTOR IN THE THEATRE
// USE SECTORS OR SOME OTHER FORM OF GROUPING
void P_CheckCollisions(std::vector<Actor *> troupe)
{
	to_be_collided.clear();

	for(Actor *iterator_actor : troupe)
	{
		if(iterator_actor->collider == NULL)
			continue;

		// For now, pretend that every Actor in the Theatre is
		// actually just every Actor in a "collision sector"
		std::vector<Actor *> stooges;
		for(Actor *checking_actor : troupe)
		{
			if(checking_actor->collider == NULL || !checking_actor->collider->sleeping || iterator_actor->name == checking_actor->name)
				continue;

			if(iterator_actor->collider->checkCollision(checking_actor->collider))
				stooges.insert(stooges.end(), checking_actor);

			// PRINT(iterator_actor->name << " AABB:\n\ttop_left_back: " << glm::to_string(AABB_1->top_left_back) << "\n\tbottom_right_front: " << glm::to_string(AABB_1->bottom_right_front));
			// PRINT(checking_actor->name << " AABB:\n\ttop_left_back: " << glm::to_string(AABB_2->top_left_back) << "\n\tbottom_right_front: " << glm::to_string(AABB_2->bottom_right_front));
			// PRINT("Collision check for [" << iterator_actor->name << "] and [" << checking_actor->name << "]");
			// PRINT("X-Axis: " << collision_check[0] << "\nY-Axis: " << collision_check[1] << "\nZ-Axis: " << collision_check[2]);
		}

		if(stooges.size() <= 0)
			continue;

		stooges.insert(stooges.begin(), iterator_actor);
		to_be_collided.insert(to_be_collided.end(), stooges);
	}

	for(std::vector<Actor *> collision_buffer : to_be_collided)
	{
		for(unsigned int i = 0 ; i < collision_buffer.size(); i++)
		{
			collision_buffer[i]->collider->sleeping = true;
		}
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