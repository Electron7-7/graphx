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
	std::vector<Actor *> actors;
	std::vector<Mesh *> meshes;
	Mesh stage;

	Theatre(std::string init_name = "UNTITLED_THEATRE", std::vector<Actor *> init_actors = {}, Mesh init_stage = Mesh(NULL, BUFFER_FLATS));

	void actorEnter(Actor *new_actor);
	void actorLeave(Actor *old_actor);

/*protected:
	void sortMeshes(); // Only use if copying sorting code a lot*/
};

extern Theatre *current_theatre;

#else
struct Theatre;
extern Theatre *current_theatre;
#endif