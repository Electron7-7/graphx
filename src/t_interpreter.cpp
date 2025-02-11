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

std::unordered_map<std::string, int> graphx_class_names
{
	{"Theatre", graphx_classes::THEATRE},
	{"Actor", graphx_classes::ACTOR},
	{"RigidBodyActor", graphx_classes::RIGIDBODYACTOR},
	{"Collider", graphx_classes::COLLIDER},
	{"Mesh", graphx_classes::MESH},
	{"Material", graphx_classes::MATERIAL},
	{"Light", graphx_classes::LIGHT},
};

int getClassHash(std::string class_name)
{
	if(graphx_class_names.contains(class_name))
		return graphx_class_names.at(class_name);
	return -1;
}

std::any getNumber(std::vector<std::string> string_input, char type)
{
	glm::vec3 numbers;

	switch(type)
	{
	case 'f':
		for(int i = 0 ; i < string_input.size() ; i++)
			numbers[i] = std::stof(string_input[i]);
	case 'l':
		for(int i = 0 ; i < string_input.size() ; i++)
			numbers[i] = std::stol(string_input[i]);
	case 'd':
		for(int i = 0 ; i < string_input.size() ; i++)
			numbers[i] = std::stod(string_input[i]);
	case 'i':
		for(int i = 0 ; i < string_input.size() ; i++)
			numbers[i] = std::stoi(string_input[i]);
	}

	switch(string_input.size())
	{
	case 1:
		return numbers[0];
	case 2:
		return glm::vec2(numbers[0], numbers[1]);
	}

	return numbers;
}

std::any extractData(std::string data_in_here)
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
		',',
		'f',
		'd',
		'l',
		'i'
	};

	if(data_in_here == "false" || data_in_here == "true")
		return (data_in_here == "true");

	std::string buffer = "";
	std::vector<std::string> vector_buffer;
	bool is_number = true;

	// i = integer
	// f = float
	// d = double
	// l = long
	char type = 'i';

	for(char &character : data_in_here)
	{
		if(special.contains(character))
		{
			if(character != 'f' && 'd' && 'l' && 'f' && ',')
				buffer += character;

			if(character == 'f')
				type = 'f';
			if(character == '.' && type != 'f')
				type = 'd';
			if(character == 'l')
				type = 'l';
			if(character == 'i' && type != 'l')
				type = 'i';
			
			if(character == ',')
			{
				vector_buffer.insert(vector_buffer.end(), buffer);
				buffer = "";
			}

			continue;
		}

		if(!std::isdigit(character))
		{
			if(forgiveness.contains(character))
				continue;
			is_number = false;
			break;
		}

		buffer += character;
	}

	vector_buffer.insert(vector_buffer.end(), buffer);

	if(is_number)
		return getNumber(vector_buffer, type);
	return data_in_here;
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

gSettings null_settings =
{
	{"NULL", NULL}
};

gSettings actor_settings =
{
	{"Name", std::string("Untitled Actor")},
	{"Visible", true},
	{"Mesh", NULL},
	{"Position", glm::vec3(0.0f)},
	{"RotationDegrees", glm::vec3(0.0f)},
	{"Scale", glm::vec3(1.0f)},
};

gSettings physics_actor_settings =
{
	{"Mass", 1.0f},
	{"Collider", NULL},
};

gSettings rigidbody_actor_settings =
{
};

gSettings camera_settings =
{
	{"Parent", NULL},
	{"LocalPosition", glm::vec3(0.0f, 3.0f, 0.0f)},
	{"LocalRotationDegrees", glm::vec3(0.0f)}
};

gSettings graphxplayer_settings =
{
	{"PlayerMesh", Mesh()},
	{"PlayerCamera", Camera()},
	{"MouseSensitivity", 0.05f},
	{"MovementSpeed", 0.1f},
	{"MaxVelocity", 8.0f}
};

gSettings light_settings =
{
	{"Color", glm::vec3(1.0f)},
	{"Strength", 1.0f},
	{"Range", 100.0f},
	{"Intensity", 1.0f},
	{"Falloff", 0.0f}
};

gSettings light_directional_settings =
{
	{"Direction", glm::vec3(0.0f, -1.0f, 0.0f)}
};

gSettings light_spot_settings =
{
	{"InnerCutoffAngle", 12.5f},
	{"OuterCutoffAngle", 17.5f}
};

gSettings light_flashlight_settings =
{
	{"Parent", NULL},
	{"PositionOffset", glm::vec3(0.0f)},
	{"RotationOffset", glm::vec3(0.0f)}
};

gSettings light_tester_mover_settings =
{
	{"PivotPosition", glm::vec3(0.0f)},
	{"PivotRadius", 3.0f},
	{"PivotSpeed", 1.0f}
};

gSettings environment_settings =
{
	{"AmbientLightingEnabled", true},
	{"AmbientLightingColor", glm::vec3(1.0f)},
	{"AmbientLightingStrength", 0.05f}
};

gSettings material_settings =
{
	{"DiffuseTexture", MISSING_TEXTURE_DIFF},
	{"SpecularTexture", MISSING_TEXTURE_SPEC},
	{"Color", glm::vec3(1.0f)},
	{"SpecularSharpness", 32},
	{"SpecularStrength", 0.5f},
	{"mat_fullbright", false}
};

gSettings mesh_settings =
{
	{"Name", std::string("Untitled Mesh")},
	{"Material", NULL},
	{"MeshData", gMeshData(CUBE_VERTS, CUBE_INDICES, VAO_HANDMADE)}
};

gSettings sprite_settings =
{};

gSettings collider_settings =
{
	{"Position", glm::vec3(0.0f)},
	{"Scale", glm::vec3(1.0f)},
	{"Quaternion", glm::quat()},
	{"MotionType", JPH::EMotionType::Dynamic},
	{"ObjectLayer", Layers::MOVING},
	{"Activation", JPH::EActivation::Activate},
	{"Shape", ColliderShapes::BOX}
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
	{ENVIRONMENT, {-1, environment_settings}},
	{MATERIAL, {-1, material_settings}},
	{MESH, {-1, mesh_settings}},
	{SPRITE, {MESH, sprite_settings}},
	{COLLIDER, {-1, collider_settings}},
};

gSettings getSettingsTemplate(std::string class_name)
{
	int class_hash = getClassHash(class_name);
	std::pair<int, gSettings> settings_pair = settings_map[class_hash];
	gSettings all_settings = {};
	int abort = 0;
	while(settings_pair.first != -1 && abort != 50) // abort != 50 is a fail-safe
	{
		// Find out how to insert an unordered map into an unordered map (without a for loop, duh)
		for(auto &pair : settings_map[settings_pair.first].second)
			all_settings[pair.first] = pair.second;
		settings_pair = settings_map[settings_pair.first];
		abort++;
	}

	for(auto &pair : settings_pair.second)
		all_settings[pair.first] = pair.second;

	if(!all_settings.contains("Name"))
		return all_settings;
	std::cout << "Name = ";
	std::cout << std::any_cast<std::string>(all_settings["Name"]) << std::endl;

	return all_settings;
}

void createNewClass(std::string class_name, int object_uid, gSettings class_settings, Theatre &parent_theatre)
{
	int class_hash = getClassHash(class_name);

	if(ACTORS[0] <= class_hash && class_hash <= ACTORS[1])
	{
		parent_theatre.objects[object_uid] = actor_map[class_hash]();
		std::any_cast<Actor *>(parent_theatre.objects[object_uid])->settings = class_settings;
		std::any_cast<Actor *>(parent_theatre.objects[object_uid])->setUID(object_uid);
	}

	if(DEVICES[0] <= class_hash && class_hash <= DEVICES[1])
	{
		parent_theatre.devices[object_uid] = device_map[class_hash]();
		std::any_cast<Device *>(parent_theatre.devices[object_uid])->settings = class_settings;
		std::any_cast<Device *>(parent_theatre.devices[object_uid])->setUID(object_uid);
		if(class_name == "Environment")
			parent_theatre.environment_uid = object_uid;
	}
}

Theatre *loadTheatre(std::string embedded_theatre)
{
	gTheatreStorage theatre_data = theatreParser(embedded_theatre);
#ifdef GRAPHX_DEBUG
	PRINTLN(getTheatreStructure(theatre_data));
#endif

	current_theatre_uid++;
	all_theatres.insert(all_theatres.end(), std::make_pair(current_theatre_uid, new Theatre(std::get<0>(theatre_data))));
	Theatre *new_theatre = all_theatres.at(current_theatre_uid);

	auto theatre_name = std::get<0>(theatre_data);
	auto objects_bucket = std::get<1>(theatre_data);
	auto cpp_references = std::get<2>(theatre_data);
	auto theatre_references = std::get<3>(theatre_data);
	auto raw_data = std::get<4>(theatre_data);
	auto layered_definitions = std::get<5>(theatre_data);

	for(const auto &object : objects_bucket)
	{
		gSettings new_class_settings = getSettingsTemplate(object.second.first);

		auto cpp_refs_range = std::get<2>(theatre_data).equal_range(object.first);
		auto theatre_refs_range = std::get<3>(theatre_data).equal_range(object.first);
		auto raw_data_range = std::get<4>(theatre_data).equal_range(object.first);
		auto sandwiches_range = std::get<5>(theatre_data).equal_range(object.first);

		for(auto it = cpp_refs_range.first ; it != cpp_refs_range.second ; ++it)
		{
			new_class_settings[it->second.first] = cpp_definitions[it->second.second];
		}

		for(auto it = theatre_refs_range.first ; it != theatre_refs_range.second ; ++it)
		{
			std::string reference_name = objects_bucket.at(it->second.second).first;
			gSettings referenced_settings;

			if(ACTORS[0] <= getClassHash(reference_name) && getClassHash(reference_name) <= ACTORS[1])
			{
				new_class_settings[it->second.first] = new_theatre->objects.at(it->second.second);
				// referenced_settings = new_theatre->objects.at(it->second.second)->settings;
			}

			if(DEVICES[0] <= getClassHash(reference_name) && getClassHash(reference_name) <= DEVICES[1])
			{
				new_class_settings[it->second.first] = new_theatre->devices.at(it->second.second);
				// referenced_settings = new_theatre->devices.at(it->second.second)->settings;
			}
			
			/*if(referenced_settings.contains(it->second.first))
			{
				new_class_settings[it->second.first] = referenced_settings.at(it->second.first);
				break;
			}*/
		}

		for(auto it = raw_data_range.first ; it != raw_data_range.second ; ++it)
		{
			new_class_settings[it->second.first] = extractData(it->second.second);
		}

		for(auto it = sandwiches_range.first ; it != sandwiches_range.second ; ++it)
		{
			gSettings settings_to_modify = null_settings;
			std::string reference_name = objects_bucket.at(it->second.first.second).first;
			if(ACTORS[0] <= getClassHash(reference_name) && getClassHash(reference_name) <= ACTORS[1])
			{
				new_class_settings[it->second.first.first] = actor_map[getClassHash(it->second.first.first)]();
				settings_to_modify = new_theatre->objects.at(it->second.first.second)->settings;
				for(auto &pair : it->second.second)
				{
					if(settings_to_modify.contains(pair.first))
					{
						if(ACTORS[0] <= getClassHash(reference_name) && getClassHash(reference_name) <= ACTORS[1])
						{
							settings_to_modify.at(pair.first) = new_theatre->objects.at(pair.second);
							continue;
						}

						settings_to_modify.at(pair.first) = new_theatre->devices.at(pair.second);
					}
				}

				std::any_cast<Actor *>(new_class_settings.at(it->second.first.first))->youGotACallBack(settings_to_modify);
				continue;
			}

			new_class_settings[it->second.first.first] = device_map[getClassHash(it->second.first.first)]();
			settings_to_modify = new_theatre->devices.at(it->second.first.second)->settings;
			for(auto &pair : it->second.second)
			{
				if(settings_to_modify.contains(pair.first))
				{
					if(ACTORS[0] <= getClassHash(reference_name) && getClassHash(reference_name) <= ACTORS[1])
					{
						settings_to_modify.at(pair.first) = new_theatre->objects.at(pair.second);
						continue;
					}

					settings_to_modify.at(pair.first) = new_theatre->devices.at(pair.second);
				}
			}

			std::any_cast<Device *>(new_class_settings.at(it->second.first.first))->loadSettings(settings_to_modify);
		}

		createNewClass(object.second.first, object.first, new_class_settings, *new_theatre);
	}

	return getCurrentTheatre();
}
