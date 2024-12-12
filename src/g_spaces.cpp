#include "g_spaces.hpp"

// std::vector<Actor *> actors_in_current_space;
Space *current_space;

void loadNewSpace(Space *new_space)
{
	// actors_in_current_space = new_space->my_actors;
	current_space = new_space;
	for(auto& actor : new_space->my_actors)
	{
		// actor->
	}
}