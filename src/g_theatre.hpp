// *grumble, grumble* I'm still proud of naming these "Spaces" but Theatre is way more clever *grumble grumble*
// :3
#ifndef GRAPHX_THEATRE
#define GRAPHX_THEATRE
#include "sanity.hpp"
#include "g_actors.hpp"
#include "r_common.hpp"
#include <string>
#include <vector>

struct Theatre
{
	std::string name;
	std::vector<Actor *> troupe;
	// std::vector<Light *> catwalk; // Change to "std::vector<Tool *> tools;" later?
	int point_lights_count = 0;
	int spot_lights_count = 0;
	Mesh stage;

	Theatre(std::string init_name = "UNTITLED_THEATRE", std::vector<Actor *> init_troupe = {}, Mesh init_stage = Mesh(NULL));

	void actorEnter(Actor *new_actor);
	void troupeEnter(std::vector<Actor *> new_troupe);
	void actorLeave(Actor *old_actor);

private:
	void sortTroupe();
};

extern Theatre *current_theatre;
#endif