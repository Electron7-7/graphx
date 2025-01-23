#include "sanity.hpp"
#include "g_common.hpp"
#include "g_actors.hpp"
std::vector<CollisionBuffer> to_be_collided;

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
		Collider *AABB_1 = iterator_actor->collider;
		for(Actor *checking_actor : troupe)
		{
			if(checking_actor->collider == NULL || iterator_actor->name == checking_actor->name || !checking_actor->sleeping)
				continue;

			Collider *AABB_2 = checking_actor->collider;
			glm::vec3 check_1 = AABB_1->position + AABB_1->size;
			glm::vec3 check_2 = AABB_2->position + AABB_2->size;

			std::array<bool, 3> collision_check;
			
			collision_check[0] = check_1[0] >= AABB_2->position[0] && check_2[0] >= AABB_1->position[0];
			collision_check[1] = check_1[1] >= AABB_2->position[1] && check_2[1] >= AABB_1->position[1];
			collision_check[2] = check_1[2] >= AABB_2->position[2] && check_2[2] >= AABB_1->position[2];

			if(collision_check[0] == true && collision_check[1] == true && collision_check[2] == true)
			{
				stooges.insert(stooges.end(), checking_actor);
				checking_actor->sleeping = false;
			}
		}

		if(stooges.size() <= 0)
			continue;

		iterator_actor->sleeping = false;
		stooges.insert(stooges.begin(), iterator_actor);
		to_be_collided.insert(to_be_collided.end(), CollisionBuffer(stooges));
	}

	for(CollisionBuffer col_buf : to_be_collided)
	{
		for(unsigned int i = 0 ; i < col_buf.stooges.size() / 2 ; i+=2)
		{
			col_buf.stooges[i]->position_global -= glm::vec3(0.01f, 0.01f, -0.01f);
			col_buf.stooges[i+1]->position_global += glm::vec3(0.01f, 0.01f, -0.01f);
			col_buf.stooges[i]->sleeping = true;
			col_buf.stooges[i+1]->sleeping = true;
			PRINT("Collisions done for " << col_buf.stooges[i]->name << " and " << col_buf.stooges[i+1]->name);
		}
	}
}
/*
std::vector<Actor *> collision_troupe;

	for(Actor *actor : troupe)
	{
		if(actor->collider == NULL)
			continue;

		collision_troupe.insert(collision_troupe.end(), actor);
	}

	int stooge_group = 0;

	for(unsigned int i = 0 ; i < collision_troupe.size() ; i++)
	{
		Collider *AABB_1 = collision_troupe[i]->collider;
		collision_troupe[i]->stooge_group = stooge_group;
		std::vector<Actor *> stooges;

		for(unsigned int j = 0 ; j < collision_troupe.size() ; j++)
		{
			PRINT(collision_troupe[j]->name);
			if(collision_troupe[j]->name == collision_troupe[i]->name)
				continue;

			Collider *AABB_2 = collision_troupe[j]->collider;
			glm::vec3 check_1 = AABB_1->position + AABB_1->size;
			glm::vec3 check_2 = AABB_2->position + AABB_2->size;

			std::array<bool, 3> collision_check;
			
			for(unsigned int k = 0 ; k < 3 ; k++)
				collision_check[k] = check_1[k] >= AABB_2->position[k] && check_2[k] >= AABB_1->position[k];

			if(collision_check == std::array<bool, 3>{true, true, true})
				stooges.insert(stooges.end(), collision_troupe[j]);
		}

		if(stooges.size() <= 0)
			continue;
		
		stooges.insert(stooges.end(), collision_troupe[i]);
		to_be_collided.insert(to_be_collided.begin() + stooge_group, stooges);
		stooge_group++;
	}

	for(CollisionBuffer group_of_stooges : to_be_collided)
	{
		for(unsigned int i = 0 ; i < group_of_stooges.stooges.size() - 1 ; i++)
		{
			group_of_stooges.stooges[i]->velocity = P_CalculateInelasticCollision(group_of_stooges.stooges[i], group_of_stooges.stooges[i+1]);
		}
	}*/
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
glm::vec3 P_CalculateInelasticCollision(Actor *stooge_left, Actor *stooge_right)
{
	glm::vec3 top_half = (stooge_left->mass * stooge_left->velocity) + (stooge_right->mass * stooge_right->velocity);
	float bottom_half = (stooge_left->mass + stooge_right->mass);
	glm::vec3 final_velocity = top_half / bottom_half;

	return final_velocity;
}