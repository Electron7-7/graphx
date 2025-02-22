#include "g_actors.hpp"
#include "r_common.hpp" // Remove this once I have a system for loading theatres
#include <algorithm>
using namespace graphx;
using namespace graphx::classes;

Theatre current_theatre;
bool current_troupe_changed = false;

Theatre *getCurrentTheatre()
{
	if(current_theatre.getUID() == -1)
		PRINTERR("getCurrentTheatre() called, but current_theatre.getUID() returned -1! This may be a problem, but the engine shouldn't crash... theoretically")

	return &current_theatre;
}

Environment *getCurrentEnvironment()
{
	if(current_theatre.getEnvironment() == nullptr)
	{
		PRINTDEBUG("No Environment found. \"getEnvironment()\" will now return a new, uninitialized Environment object pointer which shouldn't be an issue, but keep this in mind.")
		return new Environment();
	}

	return current_theatre.getEnvironment();
}

GraphXPlayer *getCurrentPlayer()
{
	if(current_theatre.getPlayer() == nullptr)
	{
		PRINTDEBUG("\"player_uid\" is -1 and no \"GraphXPlayer\" object was found! getCurrentPlayer will now return a new GraphXPlayer object")
		return new GraphXPlayer();
	}

	return current_theatre.getPlayer();
}

//
// Theatre
//
Theatre::Theatre(std::string init_name, long new_uid)
: name(init_name), UID(new_uid)
{
	stage->name = "Stage Mesh for Theatre (" + name + ")";
}

Theatre::~Theatre()
{
	stage->prepForDestruction();
	stage = nullptr;
	delete stage;
}

long Theatre::getUID()
{
	return UID;
}

void Theatre::setUID(long new_uid)
{
	if(new_uid == -1)
		PRINTERR("Attempting to set Theatre \"" << name << "\"'s UID to -1!")
	UID = new_uid;
}

// Keep in mind, Theatre::startPreshow will fire when ANY Theatre is loaded, not just when the main Theatre is
// This is why I make sure to put "keep_physics_alive" and similar things in if statements that only let them run
// under certain conditions that only exist when loading a new main Theatre
void Theatre::startPreshow()
{
	PRINTDEBUG("Entering Theatre (" << name << ")")

	PRINTLN("Devices Present:")
	for(auto &pair : devices)
	{
		if(pair.second->isType(ENVIRONMENT))
			environment_uid = pair.first;
		pair.second->loadSettings();
		pair.second->initialize(this);
	}

	PRINTLN("Actors Present:")
	for(auto &pair : objects)
	{
		if(pair.second->isType(GRAPHXPLAYER))
			player_uid = pair.first;
		pair.second->callToStage(this);
		troupe.insert(troupe.end(), pair.second);
	}

	PRINTNOTE("When a Theatre is initialized, it will go through every Actor and run youGotACallBack before callToStage")

	sortTroupe();
	countLights();
}

void Theatre::dropCurtains()
{
	PRINTDEBUG("Exiting Theatre (" << name << ")")
	time_to_render = false;
	time_to_store_buffers = false;

	PRINTLN("Devices Present:")
	for(auto &pair : devices)
		pair.second->prepForDestruction();
	devices.clear();

	PRINTLN("Actors Present:")
	for(auto &pair : objects)
		pair.second->takeABow();
	objects.clear();
	troupe.clear();
}

void Theatre::createActor(int actor_type, long uid, gSettings new_settings)
{
	PRINTDEBUG("ACTOR CREATE")
	PRINTDEBUG(uid)
	if(objects.contains(uid))
	{
		PRINTERR("Tried adding a new Actor with UID " << std::to_string(uid) << " to Theatre " << name << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
		return;
	}

	objects[uid] = actor_map[actor_type]();
	objects.at(uid)->setUID(uid);
	objects.at(uid)->settings = new_settings;

	if(objects.at(uid)->isType(GRAPHXPLAYER))
		player_uid = uid;

	PRINTDEBUG("New Actor " << objects.at(uid)->name << " with UID " << objects.at(uid)->getUID())

	sortTroupe();
	countLights();

	objects.at(uid)->youGotACallBack(new_settings);

	if(time_to_render)
	{
		objects.at(uid)->callToStage(this);
	}

	current_troupe_changed = time_to_render;
}

void Theatre::createDevice(int device_type, long uid, gSettings new_settings)
{
	if(devices.contains(uid))
	{
		PRINTERR("Tried adding a new Device with UID " << std::to_string(uid) << " to Theatre " << name << " but a Device with that UID already exists! Aborting addition of this Device! If there are problems or crashes, this may be the cause!")
		return;
	}

	devices[uid] = device_map[device_type]();
	devices.at(uid)->setUID(uid);
	devices.at(uid)->settings = new_settings;

	if(devices.at(uid)->isType(ENVIRONMENT))
		environment_uid = uid;

	devices.at(uid)->loadSettings(new_settings);

	PRINTDEBUG("New Device " << devices.at(uid)->name << " with UID " << devices.at(uid)->getUID())
}

void Theatre::troupeEnter(std::vector<std::pair<Actor *, long>> new_troupe)
{
	for(auto &pair : new_troupe)
	{
		if(objects.contains(pair.second))
		{
			PRINTERR("Tried adding a new Actor with UID " << std::to_string(pair.second) << " to Theatre " << name << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
			return;
		}

		objects[pair.second] = pair.first;
		pair.first->setUID(pair.second);
		troupe.insert(troupe.end(), objects.at(pair.second));

		if(pair.first->isType(GRAPHXPLAYER))
			player_uid = pair.second;

		if(time_to_render)
		{
			pair.first->youGotACallBack();
			pair.first->callToStage(this);
		}
	}

	sortTroupe();
	countLights();
	current_troupe_changed = time_to_render;
}

void Theatre::actorEnter(Actor *new_actor, long uid, gSettings new_settings)
{
	PRINTDEBUG("ACTOR ENTER")
	PRINTDEBUG(new_actor->name)
	if(objects.contains(uid))
	{
		PRINTERR("Tried adding a new Actor with UID " << std::to_string(uid) << " to Theatre " << name << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
		return;
	}

	objects[uid] = new_actor;
	new_actor->setUID(uid);

	if(new_actor->isType(GRAPHXPLAYER))
		player_uid = uid;

	if(!new_settings.contains("IDONTUNDERSTANDTHEQUESTIONANDIWONTRESPONDTOIT"))
		new_actor->settings = new_settings;

	sortTroupe();
	countLights();

	new_actor->youGotACallBack();

	if(time_to_render)
	{
		new_actor->callToStage(this);
	}

	current_troupe_changed = time_to_render;
	PRINTDEBUG("ALL ACTORS:")
	for(auto &pair : objects)
	{
		PRINTDEBUG("\tName: " << pair.second->name)
		PRINTDEBUG("\t\tUID: " << pair.second->getUID())
		PRINTDEBUG("\t\tMap Key: " << pair.first)
	}
}

void Theatre::actorLeave(Actor *old_actor)
{
	if(old_actor->isType(GRAPHXPLAYER))
		player_uid = -1;

	if(auto it = objects.find(old_actor->getUID()) ; it != objects.end())
	{
		// delete it->second;
		it->second = NULL;
		objects.erase(it);
		int i = 0;
		for(auto it = troupe.begin() ; it != troupe.end() ; it++,i++)
		{
			if (troupe[i] == old_actor)
			{
				// delete troupe[i];
				troupe[i] = NULL;
				troupe.erase(it);
			}
		}

		countLights();
		current_troupe_changed = time_to_render;
		return;
	}

	PRINTERR("Request to remove an Actor by pointer failed!\n\tUID of Actor given to function: " << std::to_string(old_actor->getUID()))
}

void Theatre::actorLeave(long uid)
{
	if(!objects.contains(uid))
	{
		PRINTERR("Theatre was requested to delete the Actor with UID " << std::to_string(uid) << " but that UID does not exist!")
		return;
	}

	if(objects.at(uid)->isType(GRAPHXPLAYER))
		player_uid = -1;

	if(auto it = objects.find(uid) ; it != objects.end())
	{
		// delete it->second;
		it->second = NULL;
		objects.erase(it);
		int i = 0;
		for(auto iter = troupe.begin() ; iter != troupe.end() ; iter++,i++)
		{
			if(troupe[i]->getUID() == uid)
			{
				// delete troupe[i];
				troupe[i] = NULL;
				troupe.erase(iter);
			}
		}

		countLights();
		current_troupe_changed = time_to_render;
		return;
	}

	PRINTERR("Request to remove an Actor with UID " << std::to_string(uid) << " failed!")
}

void Theatre::placeDevice(Device *new_device, long uid, gSettings new_settings)
{
	if(devices.contains(uid))
	{
		PRINTERR("Tried adding a new Device with UID " << std::to_string(uid) << " to Theatre " << name << " but a Device with that UID already exists! Aborting addition of this Device! If there are problems or crashes, this may be the cause!")
		return;
	}

	devices[uid] = new_device;
	devices.at(uid)->setUID(uid);

	if(devices.at(uid)->isType(ENVIRONMENT))
		environment_uid = uid;

	if(!new_settings.contains("IDONTUNDERSTANDTHEQUESTIONANDIWONTRESPONDTOIT"))
	{
		new_device->settings = new_settings;
		new_device->loadSettings();
	}

	if(time_to_render)
		new_device->initialize(this);
}

void Theatre::removeDevice(Device *old_device)
{
	if(auto it = devices.find(old_device->getUID()) ; it != devices.end())
	{
		if(it->second->isType(ENVIRONMENT))
			environment_uid = -1;

		// delete it->second;
		it->second = NULL;
		devices.erase(it);
		return;
	}

	PRINTERR("Request to remove a Device by pointer failed!\n\tUID of Device given to function: " << std::to_string(old_device->getUID()))
}

void Theatre::removeDevice(long uid)
{
	if(!devices.contains(uid))
	{
		PRINTERR("Theatre was requested to delete the Device with UID " << std::to_string(uid) << " but that UID does not exist!")
		return;
	}

	if(auto it = devices.find(uid) ; it != devices.end())
	{
		if(it->second->isType(ENVIRONMENT))
			environment_uid = -1;

		// delete it->second;
		it->second = NULL;
		devices.erase(it);
		return;
	}

	PRINTERR("Request to remove a Device with UID " << std::to_string(uid) << " failed!")
}

Actor *Theatre::getActor(long actor_uid)
{
	if(objects.contains(actor_uid))
		return objects.at(actor_uid);

	PRINTERR("Hey! Someone asked for an Actor with the UID " << std::to_string(actor_uid) << ", but none were found! The \"getActor\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
	return nullptr;
}

Actor *Theatre::getActor(std::string actor_name)
{
	for(auto &pair : objects)
	{
		if(pair.second->name.compare(actor_name) == 0)
			return pair.second;
	}

	PRINTERR("Hey! Someone asked for an Actor named " << actor_name << ", but none were found! The \"getActor\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
	return nullptr;
}

Device *Theatre::getDevice(long device_uid)
{
	if(devices.contains(device_uid))
		return devices.at(device_uid);

	PRINTERR("Hey! Someone asked for a Device with the UID " << std::to_string(device_uid) << ", but none were found! The \"getDevice\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
	return nullptr;
}

Device *Theatre::getDevice(std::string device_name)
{
	for(auto &pair : devices)
		if(pair.second->name.compare(device_name))
			return pair.second;

	PRINTERR("Hey! Someone asked for a Device named " << device_name << ", but none were found! The \"getDevice\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
	return nullptr;
}

GraphXPlayer *Theatre::getPlayer()
{
	if(player_uid == -1)
	{
		for(auto &pair : objects)
		{
			if(pair.second->isType(GRAPHXPLAYER))
			{
				player_uid = pair.first;
				return static_cast<GraphXPlayer *>(pair.second);
			}
		}
	}

	if(objects.contains(player_uid))
		return static_cast<GraphXPlayer *>(objects.at(player_uid));

	return nullptr;
}

Environment *Theatre::getEnvironment()
{
	if(environment_uid == -1)
	{
		for(auto &pair : devices)
		{
			if(pair.second->isType(ENVIRONMENT))
			{
				environment_uid = pair.first;
				return static_cast<Environment *>(pair.second);
			}
		}

		return new Environment();
	}

	if(devices.contains(environment_uid))
		return static_cast<Environment *>(devices.at(environment_uid));

	return nullptr;
}

void Theatre::sortTroupe()
{
	std::sort(troupe.begin(), troupe.end(), [](Actor *left, Actor *right)
	{
		return (left->isType(LIGHTS) > right->isType(LIGHTS));
	});
	for(Actor *actor : troupe)
		PRINTDEBUG(actor->name << " " << actor->isType(LIGHTS))
}

void Theatre::countLights()
{
	for(Actor *actor : troupe)
	{
		if(!actor->isType(LIGHTS))
			continue;

		if(static_cast<Light *>(actor)->isLightType(LIGHTSPOT))
		{
			spot_lights_count++;
			continue;
		}

		point_lights_count++;
	}
}