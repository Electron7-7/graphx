#include "g_actors.hpp"
#include "r_common.hpp" // Remove this once I have a system for loading theatres
#include <algorithm>

std::unordered_map<int, Theatre> all_theatres;

int current_theatre_uid = -1;
bool current_troupe_changed = false;

Theatre *getCurrentTheatre()
{
	return &all_theatres[current_theatre_uid];
}

Environment *getCurrentEnvironment()
{
	if(getCurrentTheatre()->environment_uid == -1)
		return new Environment();
	return static_cast<Environment *>(getCurrentTheatre()->devices[getCurrentTheatre()->environment_uid]);
}

//
// Theatre
//
Theatre::Theatre(std::string init_name)
: name(init_name)
{
	stage.name = "Stage Mesh for Theatre (" + name + ")";
}

void Theatre::startPreshow()
{
	for(auto &pair : objects)
		troupe.insert(troupe.end(), pair.second);

	sortTroupe();
	countLights();

	PRINTLN("Entering Theatre (" << name << ")\nActors Present:")
	for(Actor *actor : troupe)
		actor->callToStage(this);
	sortTroupe();
}

void Theatre::dropCurtains()
{
	PRINTLN("Exiting Theatre (" << name << ")\nActors Present:")
	for(auto &pair : objects)
		pair.second->takeABow();

	// for(auto &pair : devices)
		// EXIT DEVICES
}

void Theatre::addActor(Actor *new_actor)
{
	int object_uid = objects.end()->first + 1;
	objects[object_uid] = new_actor;
	troupe.insert(troupe.end(), new_actor);
}

void Theatre::removeActor(Actor *old_actor)
{
	int i = 0;
	for(auto it = troupe.begin() ; it != troupe.end() ; it++,i++)
	{
		if (troupe[i] == old_actor)
		{
			delete troupe[i];
			troupe[i] = NULL;
			troupe.erase(it);
		}
	}

	if(auto it = objects.find(old_actor->getUID()) ; it != objects.end())
	{
		delete it->second;
		it->second = NULL;
		objects.erase(it);
	}
}

void Theatre::actorEnter(Actor *new_actor)
{
	addActor(new_actor);
	sortTroupe();
	countLights();
	new_actor->callToStage(this);
	current_troupe_changed = time_to_render;
}

void Theatre::troupeEnter(std::vector<Actor *> new_troupe)
{
	for(Actor *actor : new_troupe)
		addActor(actor);

	sortTroupe();
	countLights();
	for(Actor *actor : new_troupe)
		actor->callToStage(this);
	current_troupe_changed = time_to_render;
}

void Theatre::actorLeave(Actor *old_actor)
{
	removeActor(old_actor);
	countLights();
	current_troupe_changed = time_to_render;
}

void Theatre::sortTroupe()
{
	std::sort(troupe.begin(), troupe.end(), [](Actor *left, Actor *right)
	{
		return (left->actor_type > right->actor_type);
	});
}

void Theatre::countLights()
{
	for(Actor *actor : troupe)
	{
		if(actor->actor_type != ACTOR_LIGHT)
			continue;

		if(static_cast<Light *>(actor)->light_type == LIGHT_POINT)
			point_lights_count++;

		if(static_cast<Light *>(actor)->light_type == LIGHT_SPOT)
			spot_lights_count++;
	}
}