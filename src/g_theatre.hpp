// *grumble, grumble* I'm still proud of naming these "Spaces" but Theatre is way more clever *grumble grumble*
// :3
#ifndef GRAPHX_THEATRE
#define GRAPHX_THEATRE
// #include "sanity.hpp"
#include "r_common.hpp"
#include <string>
#include <vector>

class Actor;	// forward-declare Actor

struct Theatre
{
	std::string name = "Untitled Theatre";
	std::vector<Actor *> troupe = {};
	// std::vector<Light *> catwalk; // Change to "std::vector<Tool *> tools;" later?
	int point_lights_count = 0;
	int spot_lights_count = 0;
	Mesh stage = Mesh();

	Theatre(std::string init_name = "Untitled Theatre", std::vector<Actor *> init_troupe = {}, Mesh init_stage = Mesh());

	void startPreshow();
	void dropCurtains();

	void actorEnter(Actor *new_actor);
	void troupeEnter(std::vector<Actor *> new_troupe);
	void actorLeave(Actor *old_actor);

private:
	void sortTroupe();
	void countLights();
};

extern Theatre *current_theatre;
extern bool current_troupe_changed;
#endif