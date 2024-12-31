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
	for(Actor *actor : actors)
		meshes.push_back(&actor->mesh);
	meshes.push_back(&stage);

	std::sort(meshes.begin(), meshes.end(), [](Mesh *left, Mesh *right)
	{
		return (left->vao_index > right->vao_index);
	});
}

/*void Theatre::sortMeshes() // Only use if copying sorting code a lot
{
	std::sort(meshes.begin(), meshes.end(), [](Mesh *left, Mesh *right)
	{
		return (left->vbo_id > right->vbo_id);
	});
}*/

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