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

int loadTheatre(std::string embedded_theatre)
{
	std::unordered_map<std::string, std::string> parsed_data = theatreParser(embedded_theatre);
	theatreInterpreter(parsed_data);
	return 0;
}

std::unordered_map<std::string, std::string> theatreParser(std::string theatre_data)
{
	std::set<char> whitespace =
	{
		' ',
		'	',
		'\n',
		'\t'
	};

	std::unordered_map<std::string, std::string> definition_value_pairs;
	bool in_curly_brackets = false;
	bool reading_definition = false;
	bool reading_value = false;

	std::string buffer = "";
	std::pair<std::string, std::string> pair_buffer = {};

	for(int i = 0 ; i < theatre_data.size() ; i++)
	{
		char character = theatre_data[i];
		if(whitespace.contains(character))
		{
			if(reading_definition)
			{
				reading_definition = false;
				pair_buffer.first = buffer;
				buffer = "";
			}

			else if(reading_value)
			{
				reading_value = false;
				pair_buffer.second = buffer;
				buffer = "";
			}

			continue;
		}

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

void theatreInterpreter(std::unordered_map<std::string, std::string> variable_data_pairs)
{
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