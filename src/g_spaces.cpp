#include "g_spaces.hpp"
#include "r_common.hpp"

// std::vector<Actor *> actors_in_current_space;
Space *current_space;

void loadNewSpace(Space *new_space)
{
	// actors_in_current_space = new_space->actors;
	current_space = new_space;
	for(auto& actor : new_space->actors)
	{
		// meshes.push_back(actor->mesh);
	}
}