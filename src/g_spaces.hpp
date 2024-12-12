#ifndef GRAPHX_SPACES
#define GRAPHX_SPACES
#include "sanity.hpp"
#include "g_actors.hpp"

struct Space
{
	std::vector<Actor *> actors;
};

// extern std::vector<Actor *> actors_in_current_space;
extern Space *current_space;

void loadNewSpace(Space *new_space);
#endif