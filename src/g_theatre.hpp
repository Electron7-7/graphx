// *grumble, grumble* I'm still proud of naming these "Spaces" but Theatre is way more clever *grumble grumble*
// :3
#ifndef GRAPHX_THEATRE
#define GRAPHX_THEATRE
#include "sanity.hpp"
#include "g_actors.hpp"
#include <string>

struct Theatre
{
	std::vector<Actor *> actors;
	std::string name;

	Theatre(std::vector<Actor *> init_actors, std::string init_name = "UNNAMED")
	: actors(init_actors), name(init_name)
	{}

	void addActor(Actor *new_actor);
};

extern Theatre *current_theatre;

void loadNewTheatre(Theatre *new_theatre);
#endif