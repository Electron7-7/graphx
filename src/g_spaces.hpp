#ifndef GRAPHX_SPACES
#define GRAPHX_SPACES
#include "sanity.hpp"
#include "g_actors.hpp"

struct Space
{
	std::vector<Actor *> actors;

	Space(std::vector<Actor *> init_actors)
	: actors(init_actors)
	{}

	void addActor(Actor *new_actor);
};

extern Space *current_space;

void loadNewSpace(Space *new_space);
#endif