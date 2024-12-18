// *grumble, grumble* I'm still proud of naming these "Spaces" but Theatre is way more clever *grumble grumble*
// :3
#ifndef GRAPHX_THEATRE
#define GRAPHX_THEATRE
#include "sanity.hpp"
#include "g_actors.hpp"
#include <string>

struct Theatre
{
	std::string name;
	std::vector<Actor *> actors;
	std::vector<Mesh *> meshes;
	Mesh stage;

	Theatre(std::string init_name = "UNTITLED_THEATRE", std::vector<Actor *> init_actors = {}, Mesh init_stage = Mesh());

	void addActor(Actor &new_actor);
	void removeActor(Actor &old_actor);
};

extern Theatre *current_theatre;

void loadNewTheatre(Theatre *new_theatre);
#else
struct Theatre;
extern Theatre *current_theatre;
#endif