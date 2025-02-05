#include "t_common.hpp"
#include "theatres.hpp"
// #include "g_common.hpp"
#include <algorithm>
#include <iostream>
#include <string>
#include <set>

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
	std::vector<std::vector<std::string>> parsed_data = theatreParser(embedded_theatre);
	theatreInterpreter(parsed_data);
	return 0;
}

std::vector<std::vector<std::string>> theatreParser(std::string theatre_data)
{
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
}

std::vector<std::string> variable_definitions =
{
	"Actor",
	"Actor::RigidBodyActor",
	"Mesh",
	"Material"
};

void theatreInterpreter(std::vector<std::vector<std::string>> variable_data_pairs)
{
	// all_theatres[theatre_index] = Theatre(embedded_theatre_names[theatre_index]);
	// Theatre *current_theatre = &all_theatres[theatre_index];

	for(std::vector<std::string> &pair : variable_data_pairs)
	{
		std::cout << pair[0] << " = " << pair[1] << std::endl;
		std::string variable = pair[0];
		std::string data = pair[1];
		
		auto variable_compare = [variable](std::string a)
		{
			return variable == a;
		};

		int variable_index = std::distance(variable_definitions.begin(), std::find(variable_definitions.begin(), variable_definitions.end(), pair[0]));

		switch(variable_index)
		{
		case 0:
		case 1:
			std::cout << "New Actor\n";
			break;
		case 2:
			std::cout << "New Mesh\n";
			break;
		case 3:
			std::cout << "New Material\n";
			break;
		}
	}
}