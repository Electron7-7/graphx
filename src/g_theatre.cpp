#include "g_theatre.hpp"
#include "r_common.hpp" // Remove this once I have a system for loading theatres
#include <algorithm>

Theatre *current_theatre;

//
// Theatre
//
Theatre::Theatre(std::string init_name, std::vector<Actor *> init_actors, Mesh init_stage)
: name(init_name), actors(init_actors), stage(init_stage)
{
	std::sort(actors.begin(), actors.end(), [](Actor *left, Actor *right)
	{
		return (left->type < right->type);
	});

	for(Actor *actor : actors)
		meshes.push_back(&actor->mesh);

	std::sort(meshes.begin(), meshes.end(), [](Mesh *left, Mesh *right)
	{
		// return (left->vao_index < right->vao_index);
		return (left->owner->type < right->owner->type);
	});

	meshes.push_back(&stage);
}

void Theatre::actorEnter(Actor *new_actor)
{
	actors.push_back(new_actor);
	meshes.push_back(&new_actor->mesh);
}

void Theatre::actorLeave(Actor *old_actor)
{
	for(int i = 0 ; i < actors.size() ; i++)
	{
		if(actors[i] == old_actor)
			actors.erase(actors.begin() + i);
	}
}