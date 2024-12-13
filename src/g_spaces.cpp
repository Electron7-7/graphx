#include "g_spaces.hpp"

Space *current_space;

Space::Space(std::vector<Actor *> init_actors) : actors(init_actors)
{}

void loadNewSpace(Space *new_space)
{
	current_space = new_space;
	for(auto &actor : new_space->actors)
	{
		meshes.push_back(&actor->mesh);
	}
}