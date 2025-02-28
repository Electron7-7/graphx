#include "g_actors.hpp"
#include "r_common.hpp"
#include "t_settings.hpp"
#include <algorithm>

using namespace graphx;
using namespace graphx::classes;

Theatre current_theatre;

Theatre *getCurrentTheatre()
{
	if(current_theatre.getUID() == -1)
		PRINTNOTE("getCurrentTheatre() called, but current_theatre.getUID() returned -1! This may be a problem, but the engine shouldn't crash... theoretically")
	return &current_theatre;
}

Environment *getCurrentEnvironment()
{
	if(loading_new_main_theatre)
	{
		PRINTNOTE("getCurrentEnvironment called while loading_new_main_theatre == true. Returning a new Environment in order to avoid a crash!")
		return new Environment();
	}

	if(current_theatre.unsafeGetFirstDeviceOfType(ENVIRONMENT) == nullptr)
	{
		PRINTERR("getCurrentEnvironment called, but no Environment Device found in current_theatre! Every Theatre needs an Environment!")
		PRINTNOTE("A new Environment will be created and given a UID of 177013")
		current_theatre.createDevice(ENVIRONMENT, 177013);
	}

	return static_cast<Environment *>(current_theatre.getFirstDeviceOfType(ENVIRONMENT));
}

GraphXPlayer *getCurrentPlayer()
{
	if(loading_new_main_theatre)
	{
		PRINTNOTE("getCurrentPlayer called while loading_new_main_theatre == true. Returning a new GraphXPlayer in order to avoid a crash!")
		return new GraphXPlayer();
	}

	if(current_theatre.unsafeGetFirstActorOfType(GRAPHXPLAYER) == nullptr)
	{
		PRINTERR("getCurrentPlayer called, but no GraphXPlayer Actor found in current_theatre! Every Theatre needs a GraphXPlayer!")
		PRINTNOTE("A new GraphXPlayer will be created and given a UID of 42069")
		current_theatre.createActor(GRAPHXPLAYER, 42069);
		current_theatre.refreshTroupe();
	}

	return static_cast<GraphXPlayer *>(current_theatre.getFirstActorOfType(GRAPHXPLAYER));
}

//
// Theatre
//
Theatre::Theatre(std::string init_name, long new_uid)
: name(init_name), UID(new_uid)
{
	stage_material = new Material(false, glm::vec3(0.5, 0.1, 0.4));
	stage_mesh = new Mesh(stage_material);
	stage_mesh->setName("Stage Mesh for Theatre (" + name + ")");
}

void Theatre::raiseCurtains()
{
	PRINTDEBUG("Entering Theatre (" << name << ")")

	PRINTLN("Devices Present:")
	for(auto &pair : devices)
	{
		if(pair.second->isType(ENVIRONMENT))
			environment_uid = pair.first;
		pair.second->initialize();
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

void Theatre::loadStageSettings(graphx::gSettings stage_settings)
{
	glm::vec3 stage_euler_degrees = glm::vec3(0.0f);
	getSetting(stage_position, stage_settings["Position"]);
	getSetting(stage_scale, stage_settings["Scale"]);
	getSetting(stage_euler_degrees, stage_settings["Rotation"]);
	stage_quaternion = glm::quat(glm::radians(stage_euler_degrees));
}

void Theatre::delegateKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	for(auto &pair : objects)
		pair.second->processKey(window, key, scancode, action, mods);
}

void Theatre::delegateMouseInput(GLFWwindow *window, double x_position_in, double y_position_in)
{
	for(Actor *actor : troupe)
		actor->processMouse(window, x_position_in, y_position_in);
}

std::string getSettingName(std::any any_value)
{
	if(any_value.type() == typeid(Actor*))
	{
		return(std::any_cast<Actor *>(any_value)->getName());
	}

	if(any_value.type() == typeid(Device*))
	{
		return(std::any_cast<Device *>(any_value)->getName());
	}

	if(any_value.type() == typeid(graphx::gRawData))
	{
		std::string buffer = "";
		graphx::gRawData raw_data = std::any_cast<graphx::gRawData>(any_value);
		for(int i = 0 ; i < raw_data.size() ; i++)
		{
			buffer += raw_data[i];
			if(i != raw_data.size() - 1)
				buffer += ", ";
		}

		return buffer;
	}

	return "C++ Reference (check the GraphXTheatre file)";
}

std::string Theatre::giveMeAPrettyListOfAllActorsOrDevices(bool show_actors)
{
	std::string buffer = "";
	std::string name_string = "Name: ";
	std::string typename_string = "\n\tTypename: ";
	std::string uid_string = "\n\tUID: ";

	if(show_actors)
	{
		for(auto &pair : objects)
		{
			buffer += name_string + pair.second->getName();
			buffer += typename_string + pair.second->getTypeName();
			buffer += uid_string + std::to_string(pair.second->getUID());
			for(auto &setting : pair.second->settings)
			{
				if(!setting.first.compare("Name"))
					continue;
				buffer += std::string("\n\t") + setting.first + ": " + getSettingName(setting.second);
			}
			buffer += "\n\n";
		}
	}

	else
	{
		for(auto &pair : devices)
		{
			buffer += name_string + pair.second->getName();
			buffer += typename_string + pair.second->getTypeName();
			buffer += uid_string + std::to_string(pair.second->getUID());
			for(auto &setting : pair.second->settings)
			{
				if(!setting.first.compare("Name"))
					continue;
				buffer += std::string("\n\t") + setting.first + ": " + getSettingName(setting.second);
			}
			buffer += "\n\n";
		}
	}

	return buffer;
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

Actor *Theatre::getFirstActorOfType(int type_name)
{
	if(!classnames.contains(type_name))
	{
		PRINTERR("Theatre::getFirstActorOfType(int type_name) type_name is not a valid type! Returning new Actor() to try and avoid a crash")
		return new Actor();
	}

	for(auto &pair : objects)
		if(pair.second->isType(type_name))
			return pair.second;

	PRINTERR("Theatre::getFirstActorOfType could not find an Actor of type \"" << classnames.at(type_name) << "\"! This function will return a new Actor() instead of a nullptr, to try and prevent a crash")
	return new Actor();
}

Device *Theatre::getFirstDeviceOfType(int type_name)
{
	if(!classnames.contains(type_name))
	{
		PRINTERR("Theatre::getFirstDeviceOfType(int type_name) type_name is not a valid type! Returning new Device() to try and avoid a crash")
		return new Device();
	}

	for(auto &pair : devices)
		if(pair.second->isType(type_name))
			return pair.second;
	PRINTERR("Theatre::getFirstDeviceOfType could not find a Device of type \"" << classnames.at(type_name) << "\"! This function will return a new Device() instead of a nullptr, to try and prevent a crash")
	return new Device();
}

Actor *Theatre::unsafeGetFirstActorOfType(int type_name)
{
	if(!classnames.contains(type_name))
	{
		PRINTDEBUG("Theatre::unsafeGetFirstActorOfType(int type_name) type_name is not a valid type! This function will return a nullptr")
		return nullptr;
	}

	for(auto &pair : objects)
		if(pair.second->isType(type_name))
			return pair.second;

	PRINTDEBUG("Theatre::unsafeGetFirstActorOfType could not find an Actor of type \"" << classnames.at(type_name) << "\"! This function will return a nullptr")
	return nullptr;
}

Device *Theatre::unsafeGetFirstDeviceOfType(int type_name)
{
	if(!classnames.contains(type_name))
	{
		PRINTDEBUG("Theatre::unsafeGetFirstDeviceOfType(int type_name) type_name is not a valid type! This function will return a nullptr")
		return nullptr;
	}

	for(auto &pair : devices)
		if(pair.second->isType(type_name))
			return pair.second;
	PRINTDEBUG("Theatre::unsafeGetFirstDeviceOfType could not find a Device of type \"" << classnames.at(type_name) << "\"! This function will return a nullptr")
	return nullptr;
}

void Theatre::refreshTroupe()
{
	troupe.clear();
	for(auto &pair : objects)
		troupe.insert(troupe.end(), pair.second);
	sortTroupe();
	countLights();
}

void Theatre::createActor(int actor_type, long uid, gSettings new_settings)
{
	if(objects.contains(uid))
	{
		PRINTERR("Tried adding a new Actor with UID " << std::to_string(uid) << " to Theatre " << name << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
		return;
	}

	if(actor_type == GRAPHXPLAYER)
		player_uid = uid;

	objects[uid] = actor_map[actor_type]();
	objects.at(uid)->setUID(uid);
	objects.at(uid)->youGotACallBack(new_settings);

	sortTroupe();
	countLights();

	if(time_to_render)
	{
		objects.at(uid)->callToStage(this);
	}

	time_to_store_buffers = time_to_render;
}

void Theatre::createDevice(int device_type, long uid, gSettings new_settings)
{
	if(devices.contains(uid))
	{
		PRINTERR("Tried adding a new Device with UID " << std::to_string(uid) << " to Theatre " << name << " but a Device with that UID already exists! Aborting addition of this Device! If there are problems or crashes, this may be the cause!")
		return;
	}

	if(device_type ==ENVIRONMENT)
		environment_uid = uid;

	devices[uid] = device_map[device_type]();
	devices.at(uid)->setUID(uid);
	devices.at(uid)->loadSettings(new_settings);
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
	time_to_store_buffers = time_to_render;
}

void Theatre::actorEnter(Actor *new_actor, long uid, gSettings new_settings)
{
	if(objects.contains(uid))
	{
		PRINTERR("Tried adding a new Actor with UID " << std::to_string(uid) << " to Theatre " << name << " but an Actor with that UID already exists! Aborting addition of this Actor! If there are problems or crashes, this may be the cause!")
		return;
	}

	objects[uid] = new_actor;
	troupe.insert(troupe.end(), objects.at(uid));
	new_actor->setUID(uid);

	if(new_actor->isType(GRAPHXPLAYER))
		player_uid = uid;

	new_actor->youGotACallBack(new_settings);

	sortTroupe();
	countLights();

	if(time_to_render)
		new_actor->callToStage(this);

	time_to_store_buffers = time_to_render;
}

void Theatre::actorLeave(Actor *old_actor)
{
	if(old_actor->isType(GRAPHXPLAYER))
		player_uid = -1;

	if(auto it = objects.find(old_actor->getUID()) ; it != objects.end())
	{
		it->second = NULL;
		objects.erase(it);
		int i = 0;
		for(auto it = troupe.begin() ; it != troupe.end() ; it++,i++)
		{
			if (troupe[i] == old_actor)
			{
				troupe[i] = NULL;
				troupe.erase(it);
			}
		}

		countLights();
		time_to_store_buffers = time_to_render;
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
		it->second = NULL;
		objects.erase(it);
		int i = 0;
		for(auto iter = troupe.begin() ; iter != troupe.end() ; iter++,i++)
		{
			if(troupe[i]->getUID() == uid)
			{
				troupe[i] = NULL;
				troupe.erase(iter);
			}
		}

		countLights();
		time_to_store_buffers = time_to_render;
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

	new_device->loadSettings(new_settings);

	if(time_to_render)
		new_device->initialize();
}

void Theatre::removeDevice(Device *old_device)
{
	if(auto it = devices.find(old_device->getUID()) ; it != devices.end())
	{
		if(it->second->isType(ENVIRONMENT))
			environment_uid = -1;

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
		if(pair.second->getName().compare(actor_name) == 0)
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
		if(pair.second->getName().compare(device_name))
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