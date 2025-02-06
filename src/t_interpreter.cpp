#include "sanity.hpp"
#include "t_common.hpp"
#include "r_common.hpp"
#include "theatres.hpp"
#include <set>
// #include <any>
// #include <string>
// #include <iostream>
#include <algorithm>
#include <unordered_map>

// std::unordered_map<int, Theatre> all_theatres;
std::string theatre_name;
std::map<int, std::pair<std::string, std::string>> objects_bucket;
std::multimap<int, std::pair<std::string, std::string>> cpp_references;
std::multimap<int, std::pair<std::string, int>> theatre_references;
std::multimap<int, std::pair<std::string, std::string>> raw_data;
std::multimap<int, std::pair<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>>> layered_definitions;

template<> int translateData(std::string data)
{
	return std::stoi(data);
}

template<> float translateData(std::string data)
{
	return std::stof(data);
}

template<> double translateData(std::string data)
{
	return std::stod(data);
}

template<> long translateData(std::string data)
{
	return std::stol(data);
}

void theatreParser(std::string theatre_data)
{
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
			// PRINT("theatre name: " << buffer);
			buffer = "";
			start_index = i;
			// PRINT("start index: " << start_index);
			break;
		}

		buffer += theatre_data[i];
	}

	for(int i = start_index ; i < theatre_data.size() ; i++)
	{
		char character = theatre_data[i];
		// PRINT("object uid before: " << object_uid << "\nobject_uid_after: " << (object_uid + (character == '{')));

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
				// PRINT("definition caught!\n\t" << buffer << " (Object#" << object_uid << ")");
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

				// Whitespace can show up in numerical values (might not want to keep it, though)
				buffer += character;
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
			// PRINT("value caught!\n\t" << buffer << " (Object#" << object_uid << ")");
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
}

std::string getTheatreStructure()
{
	std::string structure_out = "Internal structure of Theatre \"" + theatre_name + "\":\n-----------------------------------------------------------\n";
	structure_out += "std::map<int, std::pair<std::string, std::string>> objects_bucket =\n{\n";
	for(const auto& elem : objects_bucket)
	{
		structure_out += "\t{\n\t\t" + std::to_string(elem.first) + ",\n\t\t{" + elem.second.first + ", " + elem.second.second + "}\n\t},\n";
	}
	structure_out += "};\n";

	structure_out += "std::multimap<int, std::pair<std::string, std::string>> cpp_references =\n{\n";
	for(const auto& elem : cpp_references)
	{
		structure_out += "\t{\n\t\t" + std::to_string(elem.first) + ",\n\t\t{" + elem.second.first + ", " + elem.second.second + "}\n\t},\n";
	}
	structure_out += "};\n";

	structure_out += "std::multimap<int, std::pair<std::string, int>> theatre_references =\n{\n";
	for(const auto& elem : theatre_references)
	{
		structure_out += "\t{\n\t\t" + std::to_string(elem.first) + ",\n\t\t{" + elem.second.first + ", " + std::to_string(elem.second.second) + "}\n\t},\n";
	}
	structure_out += "};\n";

	structure_out += "std::multimap<int, std::pair<std::string, std::string>> raw_data =\n{\n";
	for(const auto& elem : raw_data)
	{
		structure_out += "\t{\n\t\t" + std::to_string(elem.first) + ",\n\t\t{" + elem.second.first + ", " + elem.second.second + "}\n\t},\n";
	}
	structure_out += "};\n";

	structure_out += "std::multimap<int, std::vector<std::pair<std::string, int>>> layered_definitions =\n{\n";
	for(const auto& elem : layered_definitions) // pair #1
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

std::unordered_map<std::string, std::any> definitions =
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

std::vector<std::string> names =
{};

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

int getClassHash(std::string &class_name)
{
	if(graphx_class_names.contains(class_name))
		return graphx_class_names.at(class_name);
	return -1;
}

void createNewClass(std::string class_name, std::string object_name)
{
	switch(getClassHash(class_name))
	{
	case graphx_classes::THEATRE:
		PRINT("New Theatre [" << object_name << "]");
		break;
	case graphx_classes::ACTOR:
		PRINT("New Actor [" << object_name << "]");
		break;
	case graphx_classes::RIGIDBODYACTOR:
		PRINT("New RigidBodyActor [" << object_name << "]");
		break;
	case graphx_classes::COLLIDER:
		PRINT("New Collider [" << object_name << "]");
		break;
	case graphx_classes::MESH:
		PRINT("New Mesh [" << object_name << "]");
		break;
	case graphx_classes::MATERIAL:
		PRINT("New Material [" << object_name << "]");
		break;
	default:
		PRINT("[ERROR] - Unknown class \"" << class_name << "\"!");
		break;
	}
}

int loadTheatre(std::string embedded_theatre)
{
	theatreParser(embedded_theatre);
#ifdef GRAPHX_DEBUG
	PRINT(getTheatreStructure());
#endif

	// for(const auto &object : objects_bucket)
	// {
	// 	createNewClass(object.second.first, object.second.second);
	// }

	return 0;
}