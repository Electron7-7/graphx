#include "sanity.hpp"
#include "t_common.hpp"
#include "cube.graphxmodel"
// #include "ERROR.graphxmodel"
// #include "pyramid.graphxmodel"
// #include "quad.graphxmodel"
#include "g_actors.hpp"
#include "r_common.hpp"
#include "g_jolt.hpp"
#include <set>
#include <any>
#include <unordered_map>

gTheatreStorage theatreParser(std::string theatre_data)
{
	gObjectStore objects_bucket;
	gSourceRefStore cpp_references;
	gTheatreRefStore theatre_references;
	gRawDataStore raw_data;
	gSandwichStore layered_definitions;
	std::string theatre_name;

	std::set<char> whitespace =
	{
		' ',
		'	',
		'\n',
		'\t'
	};

	std::set<char> begin_value =
	{
		'[',
		'<',
		'('
	};

	std::set<char> end_value =
	{
		']',
		'>',
		')'
	};

	bool in_curly_brackets = false;
	bool reading_definition = false;
	bool reading_value = false;
	bool layered = false;

	std::string buffer = "";
	std::string pair_definition_buffer = "";
	std::vector<std::string> layered_pairs_definitions_buffer = {};
	std::pair<std::string, int> layered_pairs_first_definition = {};
	std::vector<std::pair<std::string, int>> layered_pairs_buffer = {};

	int object_uid = 0;
	int layer_index = 0;
	int start_index = 0;

	for(int i = 1 ; i < theatre_data.size() ; i++)
	{
		if(theatre_data[0] != '@')
		{
			theatre_name = std::string("untitled_theatre");
			break;
		}

		if(whitespace.contains(theatre_data[i]))
		{
			theatre_name = buffer;
			buffer = "";
			start_index = i;
			break;
		}

		buffer += theatre_data[i];
	}

	for(int i = start_index ; i < theatre_data.size() ; i++)
	{
		char character = theatre_data[i];

		if(character == '{' || character == '}')
		{
			object_uid += (character == '}');
			in_curly_brackets = (character == '{');
			buffer = "";
			continue;
		}

		if(whitespace.contains(character) || character == ':')
		{
			if(character == ':' || layered)
			{
				layered = true;
			}

			if(reading_definition)
			{
				reading_definition = !whitespace.contains(character);				
				if(layered)
					layered_pairs_definitions_buffer.insert(layered_pairs_definitions_buffer.end(), buffer);
				else
					pair_definition_buffer = std::string(buffer);
				buffer = "";
				continue;
			}

			if(reading_value)
			{
				if(character == ':')
				{
					reading_value = true;
					continue;
				}

				buffer += character; // Whitespace can show up in numerical values (might not want to keep it, though)
				continue;
			}

			continue;
		}

		if(begin_value.contains(character))
		{
			reading_value = true;
			buffer = "";
			continue;
		}

		if(end_value.contains(character))
		{
			reading_value = (theatre_data[i+1] == ':');

			if(!in_curly_brackets)
			{
				objects_bucket.insert(objects_bucket.end(), std::make_pair(object_uid, std::make_pair(pair_definition_buffer, buffer)));
				buffer = "";
				continue;
			}

			switch(character)
			{
			case ']':
				cpp_references.insert(cpp_references.end(), std::make_pair(object_uid, std::make_pair(pair_definition_buffer, buffer)));
				break;
			case ')':
				raw_data.insert(raw_data.end(), std::make_pair(object_uid, std::make_pair(pair_definition_buffer, buffer)));
				break;
			case '>':
				int linked_object_uid;

				for(auto it = objects_bucket.begin(); it != objects_bucket.end() ; ++it)
				{
					if(it->second.second == buffer)
					{
						linked_object_uid = it->first;
					}
				}

				if(layered)
				{

					if(layer_index == 0)
					{
						layered_pairs_first_definition = std::make_pair(layered_pairs_definitions_buffer[0], linked_object_uid);
						buffer = "";
						layer_index++;
						continue;
					}

					if(!reading_value)
					{
						layered_pairs_buffer.insert(layered_pairs_buffer.end(), std::make_pair(layered_pairs_definitions_buffer.back(), linked_object_uid));
						layered_definitions.insert(layered_definitions.end(), std::make_pair(object_uid, std::make_pair(layered_pairs_first_definition, layered_pairs_buffer)));
						layer_index = 0;
						layered = false;
						layered_pairs_definitions_buffer = {};
						layered_pairs_buffer = {};
						layered_pairs_first_definition = {};
						buffer = "";
						continue;
					}

					layered_pairs_buffer.insert(layered_pairs_buffer.end(), std::make_pair(layered_pairs_definitions_buffer[layer_index], linked_object_uid));
					layer_index++;
					buffer = "";
					continue;
				}

				theatre_references.insert(theatre_references.end(), std::make_pair(object_uid, std::make_pair(pair_definition_buffer, linked_object_uid)));	
				break;
			}

			buffer = "";
			continue;
		}

		reading_definition = !reading_value;
		buffer += character;
	}

	return std::make_tuple
	(
		theatre_name,
		objects_bucket,
		cpp_references,
		theatre_references,
		raw_data,
		layered_definitions
	);
}

std::string getTheatreStructure(gTheatreStorage theatre_storage)
{
	std::string structure_out = "Internal structure of Theatre \"" + std::get<0>(theatre_storage) + "\":\n-----------------------------------------------------------\n";
	structure_out += "std::map<int, std::pair<std::string, std::string>> objects_bucket =\n{\n";
	for(const auto& elem : std::get<1>(theatre_storage))
	{
		structure_out += "\t{\n\t\t" + std::to_string(elem.first) + ",\n\t\t{" + elem.second.first + ", " + elem.second.second + "}\n\t},\n";
	}
	structure_out += "};\n";

	structure_out += "std::multimap<int, std::pair<std::string, std::string>> cpp_references =\n{\n";
	for(const auto& elem : std::get<2>(theatre_storage))
	{
		structure_out += "\t{\n\t\t" + std::to_string(elem.first) + ",\n\t\t{" + elem.second.first + ", " + elem.second.second + "}\n\t},\n";
	}
	structure_out += "};\n";

	structure_out += "std::multimap<int, std::pair<std::string, int>> theatre_references =\n{\n";
	for(const auto& elem : std::get<3>(theatre_storage))
	{
		structure_out += "\t{\n\t\t" + std::to_string(elem.first) + ",\n\t\t{" + elem.second.first + ", " + std::to_string(elem.second.second) + "}\n\t},\n";
	}
	structure_out += "};\n";

	structure_out += "std::multimap<int, std::pair<std::string, std::string>> raw_data =\n{\n";
	for(const auto& elem : std::get<4>(theatre_storage))
	{
		structure_out += "\t{\n\t\t" + std::to_string(elem.first) + ",\n\t\t{" + elem.second.first + ", " + elem.second.second + "}\n\t},\n";
	}
	structure_out += "};\n";

	structure_out += "std::multimap<int, std::vector<std::pair<std::string, int>>> layered_definitions =\n{\n";
	for(const auto& elem : std::get<5>(theatre_storage)) // pair #1
	{
		structure_out += "\t{\n\t\t" + std::to_string(elem.first) /*int*/ + ",\n"; // int
		structure_out += "\t\t{\n"; // pair #2
		structure_out += "\t\t\t{\n\t\t\t\t" + elem.second.first.first + ", " + std::to_string(elem.second.first.second) + "\n\t\t\t},\n";
		structure_out += "\t\t\t{\n"; // vector
		for(auto &pair : elem.second.second)
		{
			structure_out += "\t\t\t\t{\n\t\t\t\t\t" + pair.first + ", " + std::to_string(pair.second) + "\n\t\t\t\t},\n";
		}
		structure_out += "\t\t\t},\n\t\t},\n\t},\n";
	}
	structure_out += "};\n";

	return structure_out;
}

std::unordered_map<std::string, std::any> cpp_definitions =
{
	{"DOOM_TEXTURE_DIFF", DOOM_TEXTURE_DIFF},
	{"DOOM_TEXTURE_SPEC", DOOM_TEXTURE_SPEC},
	{"MISSING_TEXTURE_DIFF", MISSING_TEXTURE_DIFF},
	{"MISSING_TEXTURE_SPEC", MISSING_TEXTURE_SPEC},
	{"GRAPHX_CUBE", gMeshData(CUBE_VERTS, CUBE_INDICES, VAO_HANDMADE)},
	{"Dynamic", JPH::EMotionType::Dynamic},
	{"Static", JPH::EMotionType::Static},
	{"Kinematic", JPH::EMotionType::Kinematic},
	{"Moving", Layers::MOVING},
	{"NonMoving", Layers::NON_MOVING},
	{"Activate", JPH::EActivation::Activate},
	{"DontActivate", JPH::EActivation::DontActivate},
	{"BoxShape", ColliderShapes::BOX},
	{"SphereShape", ColliderShapes::SPHERE},
	{"CapsuleShape", ColliderShapes::CAPSULE},
	{"CylinderShape", ColliderShapes::CYLINDER},
};

gRawData extractData(std::string data_in_here)
{
	std::set<char> forgiveness =
	{
		' ',
		'	',
		'\n',
		'\t'
	};

	std::set<char> special =
	{
		'-',
		'.',
		','
	};

	if(data_in_here == "false" || data_in_here == "true")
		return gRawData{data_in_here};

	std::string buffer = "";
	gRawData vector_buffer;
	bool is_number = true;

	for(char &character : data_in_here)
	{
		if(!std::isdigit(character))
		{
			if(special.contains(character))
			{
				if(character == ',')
				{
					vector_buffer.insert(vector_buffer.end(), buffer);
					buffer = "";
					continue;
				}

				buffer += character;
				continue;
			}

			if(forgiveness.contains(character))
			{
				continue;
			}

			is_number = false;
			break;
		}

		buffer += character;
	}

	if(is_number)
	{
		vector_buffer.insert(vector_buffer.end(), buffer);
		return vector_buffer;
	}

	return gRawData{data_in_here};
}

using namespace graphx_classes;
gActorMap actor_map =
{
	{ACTOR, &createNewObject<Actor, Actor>},
	{PHYSICSACTOR, &createNewObject<PhysicsActor, Actor>},
	{RIGIDBODYACTOR, &createNewObject<RigidBodyActor, Actor>},
	{CAMERA, &createNewObject<Camera, Actor>},
	{GRAPHXPLAYER, &createNewObject<GraphXPlayer, Actor>},
	{LIGHT, &createNewObject<Light, Actor>},
	{LIGHTDIRECTIONAL, &createNewObject<LightDirectional, Actor>},
	{LIGHTSPOT, &createNewObject<LightSpot, Actor>},
	{LIGHTFLASHLIGHT, &createNewObject<LightFlashlight, Actor>},
	{LIGHTTESTERMOVER, &createNewObject<LightTesterMover, Actor>},
};

gDeviceMap device_map =
{
	{ENVIRONMENT, &createNewObject<Environment, Device>},
	{MATERIAL, &createNewObject<Material, Device>},
	{MESH, &createNewObject<Mesh, Device>},
	{SPRITE, &createNewObject<Sprite, Device>},
	{COLLIDER, &createNewObject<Collider, Device>},
};

std::unordered_map<std::string, int> graphx_class_names
{
	{"Theatre", THEATRE},
	{"Actor", ACTOR},
	{"PhysicsActor", PHYSICSACTOR},
	{"RigidBodyActor", RIGIDBODYACTOR},
	{"Camera", CAMERA},
	{"GraphXPlayer", GRAPHXPLAYER},
	{"Light", LIGHT},
	{"LightDirectional", LIGHTDIRECTIONAL},
	{"LightSpot", LIGHTSPOT},
	{"LightFlashlight", LIGHTFLASHLIGHT},
	{"LightTesterMover", LIGHTTESTERMOVER},
	{"Device", DEVICE},
	{"Environment", ENVIRONMENT},
	{"Material", MATERIAL},
	{"Mesh", MESH},
	{"Sprite", SPRITE},
	{"Collider", COLLIDER},
};

int getClassHash(std::string class_name)
{
	if(graphx_class_names.contains(class_name))
		return graphx_class_names.at(class_name);
	PRINTERR("[ERROR] Class name " << std::quoted(class_name) << " not found in \"graphx_class_names\"!\n\tSolution 1: Add it!\n\tSolution 2: Fix typo!\n\tSolution 3: Uhoh...")
	return -1;
}

gSettings null_settings =
{
	{"NULL", nullptr}
};

gSettings actor_settings =
{
	{"Name", nullptr},
	{"Visible", nullptr},
	{"Mesh", nullptr},
	{"Position", nullptr},
	{"RotationDegrees", nullptr},
	{"Scale", nullptr},
};

gSettings physics_actor_settings =
{
	{"Mass", nullptr},
	{"Collider", nullptr},
};

gSettings rigidbody_actor_settings =
{
};

gSettings camera_settings =
{
	{"Parent", nullptr},
	{"LocalPosition", nullptr},
	{"LocalRotationDegrees", nullptr}
};

gSettings graphxplayer_settings =
{
	{"PlayerMesh", nullptr},
	{"PlayerCamera", nullptr},
	{"MouseSensitivity", nullptr},
	{"MovementSpeed", nullptr},
	{"MaxVelocity", nullptr}
};

gSettings light_settings =
{
	{"Color", nullptr},
	{"Strength", nullptr},
	{"Range", nullptr},
	{"Intensity", nullptr},
	{"Falloff", nullptr}
};

gSettings light_directional_settings =
{
	{"Direction", nullptr}
};

gSettings light_spot_settings =
{
	{"InnerCutoffAngle", nullptr},
	{"OuterCutoffAngle", nullptr}
};

gSettings light_flashlight_settings =
{
	{"Parent", nullptr},
	{"PositionOffset", nullptr},
	{"RotationOffset", nullptr}
};

gSettings light_tester_mover_settings =
{
	{"PivotPosition", nullptr},
	{"PivotRadius", nullptr},
	{"PivotSpeed", nullptr}
};

gSettings device_settings =
{
	{"Name", nullptr}
};

gSettings environment_settings =
{
	{"AmbientLightingEnabled", nullptr},
	{"AmbientLightingColor", nullptr},
	{"AmbientLightingStrength", nullptr}
};

gSettings material_settings =
{
	{"DiffuseTexture", nullptr},
	{"SpecularTexture", nullptr},
	{"Color", nullptr},
	{"SpecularSharpness", nullptr},
	{"SpecularStrength", nullptr},
	{"mat_fullbright", nullptr}
};

gSettings mesh_settings =
{
	{"Name", nullptr},
	{"Material", nullptr},
	{"MeshData", nullptr}
};

gSettings sprite_settings =
{};

gSettings collider_settings =
{
	{"Position", nullptr},
	{"Scale", nullptr},
	{"Quaternion", nullptr},
	{"MotionType", nullptr},
	{"ObjectLayer", nullptr},
	{"Activation", nullptr},
	{"Shape", nullptr}
};

std::unordered_map<int, std::pair<int, gSettings>> settings_map =
{
	{ACTOR, {-1, actor_settings}},
	{PHYSICSACTOR, {ACTOR, physics_actor_settings}},
	{RIGIDBODYACTOR, {PHYSICSACTOR, rigidbody_actor_settings}},
	{CAMERA, {ACTOR, camera_settings}},
	{GRAPHXPLAYER, {ACTOR, graphxplayer_settings}},
	{LIGHT, {ACTOR, light_settings}},
	{LIGHTDIRECTIONAL, {LIGHT, light_directional_settings}},
	{LIGHTSPOT, {LIGHT, light_spot_settings}},
	{LIGHTFLASHLIGHT, {LIGHT, light_flashlight_settings}},
	{LIGHTTESTERMOVER, {LIGHT, light_tester_mover_settings}},
	{DEVICE, {-1, device_settings}},
	{ENVIRONMENT, {DEVICE, environment_settings}},
	{MATERIAL, {DEVICE, material_settings}},
	{MESH, {DEVICE, mesh_settings}},
	{SPRITE, {MESH, sprite_settings}},
	{COLLIDER, {DEVICE, collider_settings}},
};

gSettings getSettingsTemplate(std::string class_name)
{
	int class_hash = getClassHash(class_name);
	std::pair<int, gSettings> settings_pair = settings_map.at(class_hash);
	gSettings all_settings = {};
	int abort = 0;
	while(settings_pair.first != -1 && abort != 50) // abort != 50 is a fail-safe
	{
		// Find out how to insert an unordered map into an unordered map (without a for loop, duh)
		for(auto &pair : settings_pair.second)
			all_settings[pair.first] = pair.second;
		settings_pair = settings_map.at(settings_pair.first);
		abort++;
	}

	for(auto &pair : settings_pair.second)
		all_settings[pair.first] = pair.second;

	return all_settings;
}

void createNewClass(std::string class_name, int object_uid, gSettings class_settings, Theatre &parent_theatre)
{
	int class_hash = getClassHash(class_name);

	if(ACTORS[0] <= class_hash && class_hash <= ACTORS[1])
	{
		parent_theatre.actorEnter(actor_map[class_hash], object_uid);
		parent_theatre.getActor(object_uid)->settings = class_settings;
		return;
	}

	// Leaving this if{} here for now (paranoia); remove it to use early return
	if(DEVICES[0] <= class_hash && class_hash <= DEVICES[1])
	{
		parent_theatre.placeDevice(device_map[class_hash], object_uid);
		parent_theatre.getDevice(object_uid)->settings = class_settings;
		if(class_name == "Environment")
			parent_theatre.environment_uid = object_uid;
	}
}

Theatre *loadTheatre(std::string embedded_theatre, long theatre_uid)
{
	gTheatreStorage theatre_data = theatreParser(embedded_theatre);
	PRINTDEBUG(getTheatreStructure(theatre_data));

	current_theatre_uid = theatre_uid;
	if(all_theatres.contains(theatre_uid))
	{
		PRINTERR("ERROR! A Theatre with UID " << std::quoted(std::to_string(theatre_uid)) << " cannot be loaded as that UID already exists! Returning a pointer to the Theatre with the greatest UID")
		return &all_theatres.end()->second;
	}

	all_theatres[theatre_uid] = Theatre(std::get<0>(theatre_data));
	// Theatre *new_theatre = all_theatres.at(current_theatre_uid);
	Theatre &new_theatre = all_theatres.at(current_theatre_uid);

	// auto theatre_name = std::get<0>(theatre_data); // not needed, but leaving here so u know what the first tuple element is, lol
	auto objects_bucket = std::get<1>(theatre_data);
	auto cpp_references = std::get<2>(theatre_data);
	auto theatre_references = std::get<3>(theatre_data);
	auto raw_data = std::get<4>(theatre_data);
	auto layered_definitions = std::get<5>(theatre_data);

	for(const auto &object : objects_bucket)
	{
		gSettings new_class_settings = getSettingsTemplate(object.second.first);
		new_class_settings.at("Name") = gRawData{object.second.second};

		auto cpp_refs_range = std::get<2>(theatre_data).equal_range(object.first);
		auto theatre_refs_range = std::get<3>(theatre_data).equal_range(object.first);
		auto raw_data_range = std::get<4>(theatre_data).equal_range(object.first);
		auto sandwiches_range = std::get<5>(theatre_data).equal_range(object.first);

		for(auto it = cpp_refs_range.first ; it != cpp_refs_range.second ; ++it)
		{
			new_class_settings.at(it->second.first) = cpp_definitions[it->second.second];
		}

		for(auto it = theatre_refs_range.first ; it != theatre_refs_range.second ; ++it)
		{
			std::string reference_name = objects_bucket.at(it->second.second).first;
			gSettings referenced_settings;

			if(ACTORS[0] <= getClassHash(reference_name) && getClassHash(reference_name) <= ACTORS[1])
			{
				new_class_settings.at(it->second.first) = new_theatre.getActor(it->second.second);
			}

			if(DEVICES[0] <= getClassHash(reference_name) && getClassHash(reference_name) <= DEVICES[1])
			{
				new_class_settings.at(it->second.first) = new_theatre.getDevice(it->second.second);
			}
		}

		for(auto it = raw_data_range.first ; it != raw_data_range.second ; ++it)
		{
			new_class_settings.at(it->second.first) = extractData(it->second.second);
		}

		for(auto it = sandwiches_range.first ; it != sandwiches_range.second ; ++it)
		{
			gSettings settings_to_modify = null_settings;
			std::string reference_name = objects_bucket.at(it->second.first.second).first;
			if(ACTORS[0] <= getClassHash(reference_name) && getClassHash(reference_name) <= ACTORS[1])
			{
				new_class_settings.at(it->second.first.first) = actor_map[getClassHash(it->second.first.first)]();
				settings_to_modify = new_theatre.getActor(it->second.first.second)->settings;
				for(auto &pair : it->second.second)
				{
					if(settings_to_modify.contains(pair.first))
					{
						if(ACTORS[0] <= getClassHash(reference_name) && getClassHash(reference_name) <= ACTORS[1])
						{
							settings_to_modify.at(pair.first) = new_theatre.getDevice(pair.second);
							continue;
						}

						settings_to_modify.at(pair.first) = new_theatre.getDevice(pair.second);
					}
				}

				std::any_cast<Actor *>(new_class_settings.at(it->second.first.first))->youGotACallBack(settings_to_modify);
				continue;
			}

			new_class_settings.at(it->second.first.first) = device_map[getClassHash(it->second.first.first)]();
			settings_to_modify = new_theatre.getDevice(it->second.first.second)->settings;
			for(auto &pair : it->second.second)
			{
				if(settings_to_modify.contains(pair.first))
				{
					if(ACTORS[0] <= getClassHash(pair.first) && getClassHash(pair.first) <= ACTORS[1])
					{
						settings_to_modify.at(pair.first) = new_theatre.getActor(pair.second);
						continue;
					}

					settings_to_modify.at(pair.first) = new_theatre.getDevice(pair.second);
				}
			}

			std::any_cast<Device *>(new_class_settings.at(it->second.first.first))->loadSettings(settings_to_modify);
		}

		createNewClass(object.second.first, object.first, new_class_settings, new_theatre);
	}

	return getCurrentTheatre();
}
