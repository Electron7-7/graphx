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

std::unordered_map<int, Theatre> all_theatres;

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
	{"VAO_HANDMADE", VAO_HANDMADE},
	{"GRAPHX_CUBE", std::vector<std::any>{CUBE_VERTS, CUBE_INDICES}},
	// {"Dynamic", JPH::EMotionType::Dynamic},
	// {"Static", JPH::EMotionType::Static},
	// {"Kinematic", JPH::EMotionType::Kinematic},
	// {"Moving", Layers::MOVING},
	// {"NonMoving", Layers::NON_MOVING},
	// {"Activate", JPH::EActivation::Activate},
	// {"DontActivate", JPH::EActivation::DontActivate},
	{"BoxShape", ""},
	{"SphereShape", ""},
	{"CapsuleShape", ""},
	{"CylinderShape", ""},
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

gSettings actor_settings =
{
	{"CLASS_TYPE", graphx_classes::ACTOR},
	{"Name", "Untitled Actor"},
	{"Visible", true},
	{"Mesh", NULL},
	{"Position", glm::vec3(0.0f)},
	{"RotationDegrees", glm::vec3(0.0f)},
	{"Scale", glm::vec3(1.0f)},
};

gSettings getSettingsTemplate(std::string class_name)
{
	switch(getClassHash(class_name))
	{
	case graphx_classes::ACTOR:
		// new_theatre->troupe.insert(new_theatre->troupe.end(), new Actor());
		// Actor *new_actor = new_theatre->troupe[new_theatre->troupe.size()];
		return actor_settings;
	case graphx_classes::RIGIDBODYACTOR:
		break;
	case graphx_classes::COLLIDER:
		break;
	case graphx_classes::MESH:
		break;
	case graphx_classes::MATERIAL:
		break;
	default:
		PRINTERR("[ERROR] - Unknown class \"" << class_name << "\"!")
		break;
	}

	return actor_settings; // Default return for now; remove later
}

template<Actor *> std::any getVariableFrom(Actor *object_pointer, std::string variable_name)
{
	// Oh sweet lord god up in heaven above, what am I about to do?
	if(variable_name == "Name")
		return object_pointer->name;
	if(variable_name == "Visible")
		return object_pointer->visible;
	if(variable_name == "Mesh")
		return object_pointer->mesh;
	if(variable_name == "Position")
		return object_pointer->position_global;
	if(variable_name == "RotationDegrees")
		return glm::radians(glm::eulerAngles(object_pointer->quaternion));
	if(variable_name == "Scale")
		return object_pointer->scale;
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

int loadTheatre(std::string embedded_theatre)
{
	gTheatreStorage theatre_data = theatreParser(embedded_theatre);
#ifdef GRAPHX_DEBUG
	PRINT(getTheatreStructure(theatre_data));
#endif

	all_theatres.insert(all_theatres.end(), std::make_pair(all_theatres.size(), Theatre(std::get<0>(theatre_data))));
	Theatre *new_theatre = &all_theatres.end()->second;

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
			new_class_settings[it->second.first] = getVariableFrom(&new_theatre->troupe.at(it->second.second), it->second.first);
		}

		for(auto it = raw_data_range.first ; it != raw_data_range.second ; ++it)
		{
			new_class_settings[it->second.first] = extractData(it->second.second);
		}

		for(auto it = sandwiches_range.first ; it != sandwiches_range.second ; ++it)
		{
			new_class_settings[it->second.first.first] = &new_theatre->objects.at(it->second.first.second);
			for(auto &pair : it->second.second)
			{
				new_class_settings[pair.first] = &new_theatre->objects.at(pair.second);
			}
		}

		createNewClass(object.second.first, object.first, new_class_settings, new_theatre);
	}

	return 0;
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

void createNewClass(std::string class_name, int object_uid, gSettings class_settings, Theatre *parent_theatre)
{
	int class_hash = getClassHash(class_name);

	if(ACTORS[0] <= class_hash && class_hash >= ACTORS[1])
	{
		parent_theatre->objects[object_uid] = actor_map[class_hash]();
		parent_theatre->troupe.insert(parent_theatre->troupe.end(), std::any_cast<Actor *>(parent_theatre->objects[object_uid]));
		std::any_cast<Actor *>(parent_theatre->objects[object_uid])->settings = class_settings;
	}

	if(DEVICES[0] <= class_hash && class_hash >= DEVICES[1])
	{
		parent_theatre->devices[object_uid] = device_map[class_hash]();
		std::any_cast<Device *>(parent_theatre->objects[object_uid])->settings = class_settings;
	}
}