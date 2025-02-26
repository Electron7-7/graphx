#include "sanity.hpp"
#include "t_common.hpp"
#include "g_jolt.hpp"
#include "g_common.hpp"
#include "images.h"
#include "cube.graphxmodel"
#include "ERROR.graphxmodel"
#include "pyramid.graphxmodel"
#include "quad.graphxmodel"
#include <theatres.hpp>
#include <set>
#include <filesystem> // Yes, the devil hath been invoked... I truly am sorry
#include <fstream>
#include <sstream>

using namespace graphx;
using namespace graphx::classes;

bool loading_new_main_theatre = true;
std::string empty_settings_identifier = "FUCKYOU";
graphx::gSettings empty_settings = {{empty_settings_identifier, {}}};

std::map<std::string, std::any> cpp_definitions =
{
	{"DOOM_TEXTURE_DIFF", COMP04_5_png},
	{"DOOM_TEXTURE_SPEC", COMP04_5_SPECULAR_jpg},
	{"MISSING_TEXTURE_DIFF", MISSING_jpg},
	{"NO_TEXTURE", NO_TEXTURE_jpg},
	{"FLAT_SPEC", FLAT_SPEC_jpg},
	{"SOURCE_ORANGE", SOURCE_ORANGE_png},
	{"SOURCE_LIGHT_GREY", SOURCE_LIGHT_GREY_png},
	{"GRAPHX_CUBE", gMeshData(CUBE_VERTS, CUBE_INDICES, VAO_HANDMADE)},
	{"GRAPHX_ERROR", gMeshData(ERROR_VERTS, ERROR_INDICES, VAO_HANDMADE)},
	{"GRAPHX_PYRAMID", gMeshData(PYRAMID_VERTS, PYRAMID_INDICES, VAO_HANDMADE)},
	{"GRAPHX_QUAD", gMeshData(QUAD_VERTS, QUAD_INDICES, VAO_HANDMADE)},
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

#ifdef GRAPHX_DEBUG
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
#else
std::string getTheatreStructure(gTheatreStorage theatre_storage)
{
	return "Parsed Theatre \"" + std::get<0>(theatre_storage) + "\"";
}
#endif

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

int getClassHash(std::string class_name, bool dont_print_error)
{
	for(auto &pair : graphx::classnames)
		if(!pair.second.compare(class_name)) // true if equal
			return pair.first;
	if(!dont_print_error)
		PRINTERR("Class name " << std::quoted(class_name) << " not found in \"graphx::classnames\"!\n\tSolution 1: Add it!\n\tSolution 2: Fix typo!\n\tSolution 3: Uhoh...")
	return -1;
}

// loadTheatre should not be called directly, which is why it's not in the header file
Theatre loadTheatre(long theatre_uid)
{
	if(!embedded_theatres.count(theatre_uid))
	{
		PRINTERR("Tried to load a Theatre with UID " << std::quoted(std::to_string(theatre_uid)) << " but no Theatre with that UID exists!")
		return Theatre("DEFAULT ERROR RETURN THEATRE RETURNED BY \"loadTheatre\"");
	}

	gTheatreStorage theatre_data = theatreParser(embedded_theatres.at(theatre_uid));
	PRINTDEBUG(getTheatreStructure(theatre_data));

	Theatre new_theatre = Theatre(std::get<0>(theatre_data), theatre_uid);

	auto objects_bucket = std::get<1>(theatre_data);
	auto cpp_references = std::get<2>(theatre_data);
	auto theatre_references = std::get<3>(theatre_data);
	auto raw_data = std::get<4>(theatre_data);
	auto layered_definitions = std::get<5>(theatre_data);

	for(const auto &object : objects_bucket)
	{
		gSettings new_class_settings = 
		{
			{"Name", gRawData{object.second.second}},
		};

		auto cpp_refs_range = std::get<2>(theatre_data).equal_range(object.first);
		auto theatre_refs_range = std::get<3>(theatre_data).equal_range(object.first);
		auto raw_data_range = std::get<4>(theatre_data).equal_range(object.first);
		auto sandwiches_range = std::get<5>(theatre_data).equal_range(object.first);

		for(auto it = cpp_refs_range.first ; it != cpp_refs_range.second ; ++it)
		{
			new_class_settings[it->second.first] = cpp_definitions.at(it->second.second);
		}

		for(auto it = theatre_refs_range.first ; it != theatre_refs_range.second ; ++it)
		{
			if(getClassHash(it->second.first, true) != -1)
			{
				if(ACTORS[0] <= getClassHash(it->second.first) && getClassHash(it->second.first) <= ACTORS[1])
				{
					new_class_settings[it->second.first] = new_theatre.getActor(it->second.second);
					continue;
				}

				new_class_settings[it->second.first] = new_theatre.getDevice(it->second.second);
				continue;
			}

			int reference_class_hash = getClassHash(objects_bucket.at(it->second.second).first);

			if(ACTORS[0] <= reference_class_hash && reference_class_hash <= ACTORS[1])
			{
				if(!new_theatre.getActor(objects_bucket.at(it->second.second).second)->settings.contains(it->second.first))
				{
					PRINTERR(std::quoted(object.second.second) << " (" << object.second.first << ") set variable " << std::quoted(it->second.first) << " to reference a variable that was not set! Skipping this variable!")
					continue;
				}

				new_class_settings[it->second.first] = new_theatre.getActor(objects_bucket.at(it->second.second).second)->settings.at(it->second.first);
			}

			if(DEVICES[0] <= reference_class_hash && reference_class_hash <= DEVICES[1])
			{
				if(!new_theatre.getDevice(objects_bucket.at(it->second.second).second)->settings.contains(it->second.first))
				{
					PRINTERR(std::quoted(object.second.second) << " (" << object.second.first << ") set variable " << std::quoted(it->second.first) << " to reference a variable that was not set! Skipping this variable!")
					continue;
				}
			
				new_class_settings[it->second.first] = new_theatre.getDevice(objects_bucket.at(it->second.second).second)->settings.at(it->second.first);
			}
		}

		for(auto it = raw_data_range.first ; it != raw_data_range.second ; ++it)
		{
			new_class_settings[it->second.first] = extractData(it->second.second);
		}

		for(auto it = sandwiches_range.first ; it != sandwiches_range.second ; ++it)
		{
			gSettings settings_to_modify;
			int reference_class_hash = getClassHash(it->second.first.first);
			if(ACTORS[0] <= reference_class_hash && reference_class_hash <= ACTORS[1])
			{
				new_class_settings[it->second.first.first] = actor_map[reference_class_hash]();
				settings_to_modify = new_theatre.getActor(it->second.first.second)->settings;
				for(auto &pair : it->second.second)
				{
					if(settings_to_modify.contains(pair.first))
					{
						if(ACTORS[0] <= reference_class_hash && reference_class_hash <= ACTORS[1])
						{
							settings_to_modify.at(pair.first) = new_theatre.getActor(pair.second);
							continue;
						}

						settings_to_modify.at(pair.first) = new_theatre.getDevice(pair.second);
					}
				}

				// std::any_cast<Actor *>(new_class_settings.at(it->second.first.first))->settings = settings_to_modify;
				std::any_cast<Actor *>(new_class_settings.at(it->second.first.first))->youGotACallBack(settings_to_modify);
				continue;
			}

			new_class_settings[it->second.first.first] = device_map[reference_class_hash]();
			settings_to_modify = new_theatre.getDevice(it->second.first.second)->settings;
			for(auto &pair : it->second.second)
			{
				if(ACTORS[0] <= getClassHash(pair.first) && getClassHash(pair.first) <= ACTORS[1])
				{
					settings_to_modify[pair.first] = new_theatre.getActor(pair.second);
					continue;
				}

				settings_to_modify[pair.first] = new_theatre.getDevice(pair.second);
			}

			// std::any_cast<Device *>(new_class_settings.at(it->second.first.first))->settings = settings_to_modify;
			std::any_cast<Device *>(new_class_settings.at(it->second.first.first))->loadSettings(settings_to_modify);
		}

		int class_hash = getClassHash(object.second.first);

		if(ACTORS[0] <= class_hash && class_hash <= ACTORS[1])
		{
			new_theatre.createActor(class_hash, object.first, new_class_settings);
			continue;
		}

		if(!object.second.first.compare("Stage"))
		{
			// new_theatre.stage->settings = new_class_settings;
			new_theatre.stage->loadSettings(new_class_settings);
			new_theatre.loadStageSettings(new_class_settings);
			PRINTDEBUG("Theatre Stage \"" << new_theatre.stage->getName() << "\" given custom settings")
			continue;
		}

		new_theatre.createDevice(class_hash, object.first, new_class_settings);
	}

	return new_theatre;
}

void loadMainTheatre(long theatre_uid)
{
	if(getCurrentTheatre()->getUID() == theatre_uid)
	{
		PRINTERR("A Theatre with UID " << std::quoted(std::to_string(theatre_uid)) << " cannot be loaded because it's already the current Theatre (or the current Theatre has the same UID)!")
		return;
	}

	loading_new_main_theatre = true;
	time_to_render = false;
	time_to_store_buffers = false;

	// This fucking sucks, don't do this; I'm autistic and that's why I'm doing this (I don't want the printouts from calling stage.prepForDeletion() to show up)
	if(current_theatre.getUID() == -1)
	{
		current_theatre.stage->material = nullptr;
		delete current_theatre.stage->material;
		current_theatre.stage = nullptr;
		delete current_theatre.stage;
	}

	else
	{
		PRINTDEBUG("DROP CURTAINS")
		current_theatre.dropCurtains();
	}

	PRINTDEBUG("LOAD THEATRE")
	current_theatre = loadTheatre(theatre_uid);
	PRINTDEBUG("START PRESHOW")
	current_theatre.raiseCurtains();
	jolt_physics_system.OptimizeBroadPhase();
	time_to_store_buffers = true;
	loading_new_main_theatre = false;
}

void loadChildTheatre(long theatre_uid, Theatre *parent_theatre)
{
	if(parent_theatre == nullptr || parent_theatre->getUID() == -1)
	{
		PRINTERR("Tried loading a child Theatre for an invalid parent Theatre (either nullptr or with a UID of -1)!")
		return;
	}

	PRINTNOTE("loadChildTheatre called but this function is empty currently")
	// NEEDS TO BE FILLED OUT
}

#ifdef GRAPHX_WINDOWS
#include <windows.h>
#include <libloaderapi.h>
std::string getBinaryPath()
{
	char out_path[MAX_PATH] = {0};
	GetModuleFileNameA(NULL, out_path, MAX_PATH);
	std::string buffer = "";
	bool filename_removed = false;
	for(int i = (sizeof(out_path)/sizeof(out_path[0])) ; i >= 0 ; i--)
	{
		if(out_path[i] == 0x00)
			continue;
		if(((out_path[i] == '\\' && !(out_path[i+1] == ' ')) || out_path[i] == '/') && !filename_removed)
		{
			buffer = "";
			filename_removed = true;
		}

		buffer += out_path[i];
	}

	std::string new_buffer = "";

	for(int i = buffer.length() - 1 ; i > 0 ; i--)
		new_buffer += buffer[i];

	return new_buffer;
}
#define BINARY_PATH getBinaryPath()
#define EXTERNAL_THEATRES_DIRECTORY std::string("\\theatres")
#else
#define BINARY_PATH std::filesystem::read_symlink(std::filesystem::path("/proc/self/exe")).remove_filename().string()
#define EXTERNAL_THEATRES_DIRECTORY std::string("theatres")
#endif

std::string binary_path = BINARY_PATH;
std::string theatres_directory = std::string(BINARY_PATH) + EXTERNAL_THEATRES_DIRECTORY;

bool checkForExternalTheatres()
{
	std::string binary_path = BINARY_PATH;

	if(std::filesystem::is_directory(std::filesystem::path(theatres_directory)))
		for(const auto &entry : std::filesystem::directory_iterator(std::filesystem::path(theatres_directory)))
			if(entry.path().extension().string().compare(GRAPHXTHEATRE_EXTENSION) == 0)
				return true;

	return false;
}

void embedExternalTheatre(std::filesystem::path theatre_file_path)
{
	if(!std::isdigit(theatre_file_path.filename().string()[0]))
	{
		PRINTERR("GraphXTheatre file was found, but the filename did not start with a valid load number! Skipping this file!")
		return;
	}

	PRINTNOTE("Loading eternal GraphXTheatre file!")

	std::ifstream theatre_file_stream;

	theatre_file_stream.open(theatre_file_path);
	std::stringstream theatre_file_data_stream;

	theatre_file_data_stream << theatre_file_stream.rdbuf();

	theatre_file_stream.close();

	long theatre_uid;
	std::string buffer = "";

	for(char character : theatre_file_path.filename().string())
	{
		if(character == '.')
		{
			theatre_uid = std::stol(buffer);
			break;
		}

		buffer += character;
	}

	if(embedded_theatres.contains(theatre_uid))
		PRINTNOTE("Loaded external Theatre is overriding embedded Theatre #" << theatre_uid)

	embedded_theatres[theatre_uid] = theatre_file_data_stream.str();
}

bool checkForAndLoadExternalTheatres()
{
	if(!checkForExternalTheatres())
	{
		PRINTNOTE("No external GraphXTheatre directory found.")
		return false;
	}

	bool has_theatre_file = false;

	for(const auto &entry : std::filesystem::directory_iterator(std::filesystem::path(theatres_directory)))
	{
		if(entry.path().extension().string().compare(GRAPHXTHEATRE_EXTENSION) == 0)
		{
			has_theatre_file = true;
			embedExternalTheatre(entry.path());
		}
	}

	if(!has_theatre_file)
		PRINTNOTE("External GraphXTheatre directory exists, but no GraphXTheatre files were found.")

	return has_theatre_file;
}