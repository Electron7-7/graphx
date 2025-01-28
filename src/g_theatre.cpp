#include "g_theatre.hpp"
#include "r_common.hpp" // Remove this once I have a system for loading theatres
#include "g_actors.hpp"
#include <algorithm>

Theatre *current_theatre;
bool current_troupe_changed = false;

//
// Theatre
//
Theatre::Theatre(std::string init_name, std::vector<Actor *> init_troupe, Mesh init_stage)
: name(init_name), troupe(init_troupe), stage(init_stage)
{
	stage.name = "Stage Mesh for Theatre (" + name + ")";
	sortTroupe();
	countLights();
}

void Theatre::startPreshow()
{
	PRINT("Entering Theatre (" << name << ")\nActors Present:");
	for(Actor *actor : troupe)
	{
		actor->init(this);
		PRINT("\t- " << actor->name);
	}

	sortTroupe();
}

void Theatre::dropCurtains()
{
	PRINT("Exiting Theatre (" << name << ")\nActors Present:");
	for(Actor *actor : troupe)
	{
		PRINT("\t- " << actor->name);
		/*if(actor->actor_type == ACTOR_PHYSICS)
		{
			JPH::BodyInterface &body_interface = physics_system->GetBodyInterface();
			body_interface.RemoveBody(static_cast<PhysicsActor *>(actor)->physics_body_id);
			body_interface.DestroyBody(static_cast<PhysicsActor *>(actor)->physics_body_id);
		}*/
	}
}

void Theatre::actorEnter(Actor *new_actor)
{
	troupe.insert(troupe.end(), new_actor); // This can be expanded to multiple Actors
	sortTroupe();
	countLights();
	new_actor->init(this);
	current_troupe_changed = time_to_render;
}

void Theatre::troupeEnter(std::vector<Actor *> new_troupe)
{
	troupe.insert(troupe.end(), new_troupe.begin(), new_troupe.end());
	sortTroupe();
	countLights();
	for(Actor *actor : new_troupe)
		actor->init(this);
	current_troupe_changed = time_to_render;
}

void Theatre::actorLeave(Actor *old_actor)
{
	for(int i = 0 ; i < troupe.size() ; i++)
	{
		if(troupe[i] == old_actor)
			troupe.erase(troupe.begin() + i);
	}

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