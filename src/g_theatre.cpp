#include "g_theatre.hpp"
#include "r_common.hpp" // Remove this once I have a system for loading theatres
#include "g_math.hpp"
#include <algorithm>

Theatre *current_theatre;

Theatre::Theatre(std::string init_name, std::vector<Actor *> init_actors, Mesh init_stage)
: name(init_name), actors(init_actors), stage(init_stage)
{
	for(Actor *actor : actors)
		meshes.push_back(&actor->mesh);
	meshes.push_back(&stage);

	std::sort(meshes.begin(), meshes.end(), gmath::compareVAOID);
}

void loadNewTheatre(Theatre *new_theatre) // Honestly, remove this whole function once I have a system for loading theatres
{
	current_theatre = new_theatre;
	time_to_store_buffers = true; // Remove this once I have a system for loading theatres
}