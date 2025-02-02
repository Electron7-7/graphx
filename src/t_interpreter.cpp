#include "t_common.hpp"
#include <cstring>
#include <set>
#include <string>

std::unordered_map<std::string, std::any> interpreter_definitions =
{
	// {std::string("DOOM_TEXTURE_DIFF"), DOOM_TEXTURE_DIFF},
	// {std::string("DOOM_TEXTURE_SPEC"), DOOM_TEXTURE_SPEC},
	// {std::string("VAO_HANDMADE"), VAO_HANDMADE},
	{std::string("GRAPHX_CUBE"), std::vector<std::any>{CUBE_VERTS, CUBE_INDICES}},
};

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

Theatre *loadTheatre(const char *theatre_data)
{
	std::stringstream data_stream(theatre_data);
	char character;

	std::set<char> delimiters =
	{
		' ',
		'\n'
	};

	std::set<char> data_start =
	{
		'[',
		'{',
		'<',
		'('
	};

	std::set<char> data_end =
	{
		']',
		'}',
		'>',
		')'
	};

	std::set<char> data_continue
	{
		':',
		','
	};

	bool ignore_next_space = false;
	bool is_data = false;
	bool is_definition = false;
	bool was_data = false;
	bool was_definition = false;
	std::string buffer;

	while(data_stream.get(character))
	{
		if(delimiters.contains(character))
		{
			continue;
			// if(ignore_next_space)
				// continue;
			// buffer = "";
		}

		if(data_start.contains(character))
		{
			if(!is_data && !is_definition)
				is_definition = true;
			if(!is_data)
			{
				is_definition = false;
				is_data = true;
			}
			if(!is_definition)
			{
				is_data = false;
				is_definition = true;
			}

			buffer = "";
			continue;
		}

		if(data_continue.contains(character))
		{
			const char *comma = ",";
			if(std::strcmp((const char *)character, comma))
			{
				ignore_next_space = true;
				continue;
			}
			if(was_data)
			{
				is_data = true;
				is_definition = false;
			}
			if(was_definition)
			{
				is_data = false;
				is_definition = true;
			}
		}

		if(data_end.contains(character))
		{
			std::cout << "is_data: " << is_data << std::endl;
			std::cout << "is_definition: " << is_definition << std::endl;
			// find a better way to do this VVV
			if(is_data)
			{
				is_data = false;
				is_definition = true;
				was_data = true;
				was_definition = false;
				std::cout << "Data:\n\t" << buffer << std::endl;
			}
			if(is_definition)
			{
				is_definition = false;
				is_data = true;
				was_definition = true;
				was_data = false;
				std::cout << "Definition:\n\t" << buffer << std::endl;
			}
		}

		buffer += character;
	}
}