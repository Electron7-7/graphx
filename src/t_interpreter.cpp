// #include "r_common.hpp"
#include "t_common.hpp"
#include "g_jolt.hpp"
#include "theatres.hpp"
#include "sanity.hpp"
// #include "g_common.hpp"
#include <set>
#include <any>
#include <string>
#include <iostream>
#include <algorithm>
#include <unordered_map>
#include <Jolt/Physics/Collision/Shape/BoxShape.h>
#include <Jolt/Physics/Collision/Shape/SphereShape.h>
#include <Jolt/Physics/Collision/Shape/CapsuleShape.h>
#include <Jolt/Physics/Collision/Shape/CylinderShape.h>

#define VAO_HANDMADE 0
#define DOOM_TEXTURE_DIFF 3
#define DOOM_TEXTURE_SPEC 4

// std::unordered_map<int, Theatre> all_theatres;
std::string theatre_name;
std::map<int, std::pair<std::string, std::string>> objects_bucket;
std::multimap<int, std::pair<std::string, std::string>> cpp_references;
std::multimap<int, std::pair<std::string, int>> theatre_references;
std::multimap<int, std::pair<std::string, std::string>> raw_data;
std::multimap<int, std::vector<std::pair<std::string, int>>> layered_definitions;

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
	std::string pair_definition_buffer = {}; // {definition, value}
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
			PRINT("theatre name: " << buffer);
			buffer = "";
			start_index = i;
			PRINT("start index: " << start_index);
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
			if(reading_definition)
			{
				PRINT("definition caught!\n\t" << buffer << " (Object#" << object_uid << ")");
				reading_definition = !whitespace.contains(character);
				layered = (character == ':');
				if(layered)
					layered_pairs_buffer.insert(layered_pairs_buffer.end(), std::make_pair(buffer, (int)0));
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
			PRINT("value caught!\n\t" << buffer << " (Object#" << object_uid << ")");
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
					layered_pairs_buffer[layer_index].second = linked_object_uid;
					buffer = "";
					layer_index++;

					if(!reading_value)
					{
						layered_definitions.insert(layered_definitions.end(), std::make_pair(object_uid, layered_pairs_buffer));
						layer_index = 0;
						continue;
					}

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

#define UID_THEATRE			0
#define UID_ACTOR			1
#define UID_RIGIDBODYACTOR	2
#define UID_MESH			3
#define UID_MATERIAL		4

std::unordered_map<std::string, int> class_names =
{
	{"Theatre", UID_THEATRE},
	{"Actor", UID_ACTOR},
	{"Actor::RigidBodyActor", UID_RIGIDBODYACTOR},
	{"Mesh", UID_MESH},
	{"Material", UID_MATERIAL}
};

std::unordered_map<std::string, std::any> definitions =
{
	{"DOOM_TEXTURE_DIFF", DOOM_TEXTURE_DIFF},
	{"DOOM_TEXTURE_SPEC", DOOM_TEXTURE_SPEC},
	{"VAO_HANDMADE", VAO_HANDMADE},
	{"GRAPHX_CUBE", std::vector<std::any>{CUBE_VERTS, CUBE_INDICES}},
	{"Dynamic", JPH::EMotionType::Dynamic},
	{"Static", JPH::EMotionType::Static},
	{"Kinematic", JPH::EMotionType::Kinematic},
	{"Moving", Layers::MOVING},
	{"NonMoving", Layers::NON_MOVING},
	{"Activate", JPH::EActivation::Activate},
	{"DontActivate", JPH::EActivation::DontActivate},
	{"BoxShape", ""},
	{"SphereShape", ""},
	{"CapsuleShape", ""},
	{"CylinderShape", ""},
};

std::vector<std::string> names =
{};

void createNewClass(int class_uid, std::string object_name)
{
	switch(class_uid)
	{
	case UID_THEATRE:
		PRINT("New Theatre [" << object_name << "]");
		break;
	case UID_ACTOR:
		PRINT("New Actor [" << object_name << "]");
		break;
	case UID_RIGIDBODYACTOR:
		PRINT("New RigidBodyActor [" << object_name << "]");
		break;
	case UID_MESH:
		PRINT("New Mesh [" << object_name << "]");
		break;
	case UID_MATERIAL:
		PRINT("New Material [" << object_name << "]");
		break;
	}
}

int loadTheatre(std::string embedded_theatre)
{
	theatreParser(embedded_theatre);
	PRINT("Interpreting Theatre (" << theatre_name << ")");
	for(int i = 0 ; i < objects_bucket.size() ; i++)
	{
		auto it = objects_bucket[i];
		PRINT("New " << objects_bucket[i].first << " named \"" << objects_bucket[i].second << "\" with UID #" << i);
		
		for(auto[itr, end] = cpp_references.equal_range(i) ; itr != end ; ++itr)
			std::cout << "\t" << itr->second.first << " = " << itr->second.second << std::endl;
		
		for(auto[itr, end] = theatre_references.equal_range(i) ; itr != end ; ++itr)
			std::cout << "\t" << itr->second.first << " = " << objects_bucket[itr->second.second].second << "." << itr->second.first << std::endl;
		
		for(auto[itr, end] = raw_data.equal_range(i) ; itr != end ; ++itr)
			std::cout << "\t" << itr->second.first << " = " << itr->second.second << std::endl;
		
		for(auto[itr, end] = layered_definitions.equal_range(i) ; itr != end ; ++itr)
		{
			std::cout << "\t" << itr->second[0].first << " [" << itr->second[0].second << "] =\n";
			for(auto &pair : itr->second)
				std::cout << "\t\t" << pair.first << " = " << objects_bucket[pair.second].first << "." << pair.first << std::endl;
		}
	}

	// all_theatres[theatre_index] = Theatre();
	// Theatre *current_theatre = &all_theatres[theatre_index];

	// for(std::vector<std::string> &pair : variable_data_pairs)
	// {
	// 	std::cout << pair[0] << " = " << pair[1] << std::endl;
	// 	std::string variable = pair[0];
	// 	std::string data = pair[1];

	// 	if(class_names.contains(variable))
	// 		createNewClass(class_names[variable], data);
	// }

	return 0;
}

/*
OLD PARSER CODE:
// variables:
	// (?<=^|^\s)(\w+(:\w+)?)
	// data:
	// (?<=\[|\(|\<)((-?\w+(\.|,\s)?)+)(?=\]|\)|\>)(:(?<=\[|\(|\<)((-?\w+(\.|,\s)?)+)(?=\]|\)|\>))?

	std::set<char> data_start =
	{
		'[',
		'(',
		'<'
	};

	std::set<char> data_end =
	{
		']',
		')',
		'>'
	};

	std::set<char> whitespace =
	{
		' ',
		'	',
		'\n',
		'\t',
		'{',
		'}',
		':'
	};

	std::string buffer = "";
	bool is_data = false;
	bool is_variable = false;
	bool is_in_object = false;
	bool is_theatre_name = false;
	std::vector<std::string> variables;
	std::vector<std::string> data;
	std::vector<std::vector<std::string>> variable_data_pairs;
	std::string object_variable;
	std::string variable_variable;

	for(char &character : theatre_data)
	{
		if(character == '@' && &character != &theatre_data.back())
		{
			is_theatre_name = true;
			buffer = "";
			continue;
		}

		if(is_theatre_name)
		{
			if(whitespace.contains(character))
			{
				variables.insert(variables.end(), "Theatre");
				data.insert(data.end(), buffer);
				buffer = "";
				is_theatre_name = false;
				continue;
			}

			buffer += character;
			continue;
		}

		if(character == '{')
		{
			is_in_object = true;
		}
		if(character == '}')
		{
			is_in_object = false;
			object_variable = "";
		}
		if(character == ':' && is_variable)
		{
			if(is_in_object)
			{
				object_variable += "." + buffer;
				continue;
			}
			if(!is_in_object)
			{
				buffer += "::";
				continue;
			}
		}

		if(whitespace.contains(character))
		{
			if(is_variable)
			{
				is_variable = false;
				if(!is_in_object)
				{
					object_variable += buffer;
				}

				if(is_in_object)
					buffer = object_variable + "." + buffer;

				variables.insert(variables.end(), buffer);

				buffer = "";
			}
			continue;
		}

		if(data_start.contains(character))
		{
			is_data = true;
			is_variable = false;
			continue;
		}

		if(data_end.contains(character))
		{
			is_data = false;
			data.insert(data.end(), buffer);
			buffer = "";
			continue;
		}

		if(is_data)
		{
			buffer += character;
			continue;
		}

		is_variable = true;
		buffer += character;
	}

	for(int i = 0 ; i < variables.size() ; i++)
		variable_data_pairs.insert(variable_data_pairs.end(), std::vector<std::string>{variables[i], data[i]});

	return variable_data_pairs;
*/