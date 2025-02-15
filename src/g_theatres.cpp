#include "g_actors.hpp"
#include "r_common.hpp" // Remove this once I have a system for loading theatres
#include <algorithm>
using namespace graphx;

std::unordered_map<int, Theatre> all_theatres = {};

int current_theatre_uid = -1;
bool current_troupe_changed = false;

Theatre *getCurrentTheatre()
{
	return &all_theatres[current_theatre_uid];
}

Environment *getCurrentEnvironment()
{
	if(current_theatre_uid == -1 || getCurrentTheatre()->environment_uid == -1)
		return new Environment();
	return static_cast<Environment *>(getCurrentTheatre()->getDevice(getCurrentTheatre()->environment_uid));
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
	PRINTDEBUG("Entering Theatre (" << name << ")\nActors Present:")
	for(auto &pair : devices)
		pair.second->loadSettings();

	for(auto &pair : objects)
	{
		pair.second->youGotACallBack();
		pair.second->callToStage(this);
	}
	PRINTDEBUG("Note:\n\tWhen a Theatre is initialized, it will go through every Actor and run youGotACallBack before callToStage")

	sortTroupe();
	countLights();
}

void Theatre::dropCurtains()
{
	PRINTDEBUG("Exiting Theatre (" << name << ")\nActors Present:")
	for(auto &pair : objects)
		pair.second->takeABow();

	PRINTIMPORTANT("HEY! HEY! DON'T FORGET! DEVICES NEED TO BE TOLD TO EXIT, TOO!!")

	// for(auto &pair : devices)
		// EXIT DEVICES
}

void Theatre::actorEnter(Actor *new_actor, long uid, gSettings new_settings)
{
	if(objects.contains(uid))
	{
		PRINTERR("Tried adding a new Actor with UID " << std::quoted(std::to_string(uid)) << " to Theatre " << std::quoted(name) << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
		return;
	}

	objects[uid] = new_actor;
	new_actor->setUID(uid);
	if(!new_settings.contains("IDONTUNDERSTANDTHEQUESTIONANDIWONTRESPONDTOIT"))
		new_actor->settings = new_settings;
	troupe.insert(troupe.end(), objects.at(uid));

	sortTroupe();
	countLights();

	new_actor->youGotACallBack();

	if(time_to_render)
	{
		new_actor->callToStage(this);
	}

	current_troupe_changed = time_to_render;
}

void Theatre::createActor(Actor *new_actor_function(), long uid, gSettings new_settings)
{
	if(objects.contains(uid))
	{
		PRINTERR("Tried adding a new Actor with UID " << std::quoted(std::to_string(uid)) << " to Theatre " << std::quoted(name) << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
		return;
	}

	objects[uid] = new_actor_function();
	objects.at(uid)->setUID(uid);
	objects.at(uid)->settings = new_settings;
	troupe.insert(troupe.end(), objects.at(uid));
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

void Theatre::troupeEnter(std::vector<std::pair<Actor *, long>> new_troupe)
{
	for(auto &pair : new_troupe)
	{
		if(objects.contains(pair.second))
		{
			PRINTERR("Tried adding a new Actor with UID " << std::quoted(std::to_string(pair.second)) << " to Theatre " << std::quoted(name) << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
			return;
		}

		objects[pair.second] = pair.first;
		pair.first->setUID(pair.second);
		troupe.insert(troupe.end(), objects.at(pair.second));

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

void Theatre::actorLeave(Actor *old_actor)
{
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

	PRINTERR("Request to remove an Actor by pointer failed!\n\tUID of Actor given to function: " << std::quoted(std::to_string(old_actor->getUID())))
}

void Theatre::actorLeave(long uid)
{
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

	PRINTERR("Request to remove an Actor with UID " << std::quoted(std::to_string(uid)) << " failed!")
}

void Theatre::placeDevice(Device *new_device, long uid, gSettings new_settings)
{
	if(devices.contains(uid))
	{
		PRINTERR("Tried adding a new Device with UID " << std::quoted(std::to_string(uid)) << " to Theatre " << std::quoted(name) << " but a Device with that UID already exists! Aborting addition of this Device! If there are problems or crashes, this may be the cause!")
		return;
	}

	devices[uid] = new_device;
	if(!new_settings.contains("IDONTUNDERSTANDTHEQUESTIONANDIWONTRESPONDTOIT"))
	{
		new_device->settings = new_settings;
		new_device->loadSettings();
	}

	if(time_to_render)
		new_device->initialize(this);
}

void Theatre::createDevice(Device *new_device_function(), long uid, gSettings new_settings)
{
	if(devices.contains(uid))
	{
		PRINTERR("Tried adding a new Device with UID " << std::quoted(std::to_string(uid)) << " to Theatre " << std::quoted(name) << " but a Device with that UID already exists! Aborting addition of this Device! If there are problems or crashes, this may be the cause!")
		return;
	}

	devices[uid] = new_device_function();
	devices.at(uid)->settings = new_settings;
	devices.at(uid)->loadSettings(new_settings);

	PRINTDEBUG("New Device " << devices.at(uid)->name << " with UID " << devices.at(uid)->getUID())
}

void Theatre::removeDevice(Device *old_device)
{
	if(auto it = devices.find(old_device->getUID()) ; it != devices.end())
	{
		// delete it->second;
		it->second = NULL;
		devices.erase(it);
		return;
	}

	PRINTERR("Request to remove a Device by pointer failed!\n\tUID of Device given to function: " << std::quoted(std::to_string(old_device->getUID())))
}

void Theatre::removeDevice(long uid)
{
	if(auto it = devices.find(uid) ; it != devices.end())
	{
		// delete it->second;
		it->second = NULL;
		devices.erase(it);
		return;
	}
	
	PRINTERR("Request to remove a Device with UID " << std::quoted(std::to_string(uid)) << " failed!")
}

Actor *Theatre::getActor(long actor_uid)
{
	if(objects.contains(actor_uid))
		return objects.at(actor_uid);

	PRINTERR("Hey! Someone asked for an Actor with the UID " << std::quoted(std::to_string(actor_uid)) << ", but none were found! The \"getActor\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
	return nullptr;
}

Actor *Theatre::getActor(std::string actor_name)
{
	for(auto &pair : objects)
	{
		if(pair.second->name.compare(actor_name) == 0)
			return pair.second;
	}

	PRINTERR("Hey! Someone asked for an Actor named " << std::quoted(actor_name) << ", but none were found! The \"getActor\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
	return nullptr;
}

Device *Theatre::getDevice(long device_uid)
{
	if(devices.contains(device_uid))
		return devices.at(device_uid);

	PRINTERR("Hey! Someone asked for a Device with the UID " << std::quoted(std::to_string(device_uid)) << ", but none were found! The \"getDevice\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
	return nullptr;
}

Device *Theatre::getDevice(std::string device_name)
{
	for(auto &pair : devices)
		if(pair.second->name.compare(device_name))
			return pair.second;

	PRINTERR("Hey! Someone asked for a Device named " << std::quoted(device_name) << ", but none were found! The \"getDevice\" function will now return a nullptr; if the engine crashed or something wrong is happening, this may be why!")
	return nullptr;
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