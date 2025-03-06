#include "sanity.hpp"
#include "t_common.hpp"
#include "g_jolt.hpp"
#include "g_common.hpp"
#include "r_common.hpp"
#include "images.h"
#include <models.hpp>
#include <theatres.hpp>
#include <set>
#include <filesystem> // Yes, the devil hath been invoked... I'm sorry
#include <fstream>
#include <sstream>

using namespace graphx;
using namespace graphx::classes;

bool loading_new_main_theatre = true;
std::string empty_settings_identifier = "FUCKYOU";
graphx::gSettings empty_settings = {{empty_settings_identifier, gSetting(-1, {})}};

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
	{"GRAPHX_PYRAMID", gMeshData(PYRAMID_VERTS, PYRAMID_INDICES, VAO_HANDMADE)},
	{"GRAPHX_QUAD", gMeshData(QUAD_VERTS, QUAD_INDICES, VAO_HANDMADE)},
	{"OBJ_ERROR", M_LoadOBJ(ERROR_obj)},
	{"OBJ_SUZANNE", M_LoadOBJ(suzanne_obj)},
	{"notapenis", M_LoadOBJ(purely_for_testing_obj)},
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

gStringSettings theatreParser(std::string theatre_data)
{
	std::set<char> whitespace =
	{
		' ',
		'	',
		'\r',
		'\n',
		'\t'
	};

	char begin_settings = '{';
	char end_settings = '}';

	char begin_cxx_reference = '[';
	char begin_theatre_reference = '<';
	char begin_raw_data = '(';
	char begin_external_reference = '"';

	char end_cxx_reference = ']';
	char end_theatre_reference = '>';
	char end_raw_data = ')';
	char end_external_reference = '"';

	char sandwich_layer = ':';

	std::vector<gKey> keys;
	std::vector<gValue> values;
	std::vector<gStringSetting> object_settings;

	gStringSettings all_settings;

	bool reading_settings = false;
	std::string buffer = "";

	for(int i = 0 ; i < theatre_data.size() ; i++)
	{
		if(theatre_data[i] == '@')
		{
			i++;
			while(!whitespace.contains(theatre_data[i]))
			{
				buffer += theatre_data[i++];
			}

			all_settings.insert(all_settings.end(), {gStringSetting("Theatre", gValue(RAW_DATA, buffer))});
			buffer = "";
		}

		if(!theatre_data.substr(i, 2).compare("//"))
		{ // This is a comment (in the .gt file, not this C++ comment...)
			while(theatre_data[i] != '\n')
				i++;
		}

		if(reading_settings)
		{
			if(theatre_data[i] == end_settings)
			{
				reading_settings = false;

				for(int it = 0 ; it < keys.size() ; it++)
					if(it < values.size())
						object_settings.insert(object_settings.end(), gStringSetting(keys[it], values[it]));

				all_settings.insert(all_settings.end(), object_settings);
				keys.clear();
				values.clear();
				object_settings.clear();
				continue;
			}

			if(theatre_data[i] == begin_cxx_reference || theatre_data[i] == begin_theatre_reference || theatre_data[i] == begin_raw_data || theatre_data[i] == begin_external_reference)
			{
				i++;

				while(theatre_data[i] != end_cxx_reference && theatre_data[i] != end_theatre_reference && theatre_data[i] != end_raw_data && theatre_data[i] != end_external_reference)
					buffer += theatre_data[i++];

				int value_type;

				if(theatre_data[i] == end_cxx_reference)
					value_type = CPP_REFERENCE;
				if(theatre_data[i] == end_theatre_reference)
					value_type = THEATRE_REFERENCE;
				if(theatre_data[i] == end_raw_data)
					value_type = RAW_DATA;
				if(theatre_data[i] == end_external_reference)
					value_type = EXTERNAL_REFERENCE;

				if(keys.at(values.size()).find(':') != std::string::npos)
				{
					if(theatre_data[i+1] != ':')
					{
						keys.at(values.size()).pop_back();
						value_type = SANDWICH;
					}

					else
						value_type += SANDWICH;
				}


				values.insert(values.end(), gValue(value_type, buffer));
				buffer = "";
				i++;

				if(theatre_data[i] == sandwich_layer)
					continue;
			}

			if(whitespace.contains(theatre_data[i]))
			{
				if(buffer.size() == 0)
					continue;

				keys.insert(keys.end(), buffer);
				buffer = "";
				continue;
			}

			if(!whitespace.contains(theatre_data[i]))
			{
				if(theatre_data[i] == sandwich_layer)
				{
					buffer += theatre_data[i];
					keys.insert(keys.end(), buffer);
					continue;
				}

				buffer += theatre_data[i];
			}
		}

		if(!reading_settings)
		{
			if(whitespace.contains(theatre_data[i]))
			{
				if(buffer.size() == 0)
					continue;

				keys.insert(keys.end(), buffer);
				buffer = "";
				continue;
			}

			if(theatre_data[i] == begin_raw_data)
			{
				i++;

				while(theatre_data[i] != end_raw_data)
					buffer += theatre_data[i++];

				values.insert(values.end(), gValue(RAW_DATA, buffer));
				buffer = "";
				object_settings.insert(object_settings.end(), gStringSetting(keys[0], values[0]));
				keys.clear();
				values.clear();
				continue;
			}

			if(theatre_data[i] == begin_settings)
			{
				buffer = "";
				reading_settings = true;
				continue;
			}

			buffer += theatre_data[i];
		}
	}

	return all_settings;
}

std::string getVariableTypeName(int variable_type)
{
	std::string type_return;

	switch(variable_type)
	{
	case CPP_REFERENCE:
		type_return = "CPP_REFERENCE";
		break;
	case THEATRE_REFERENCE:
		type_return = "THEATRE_REFERENCE";
		break;
	case RAW_DATA:
		type_return = "RAW_DATA";
		break;
	case EXTERNAL_REFERENCE:
		type_return = "EXTERNAL_REFERENCE";
		break;
	case SANDWICH:
		type_return = "SANDWICH";
		break;
	default:
		type_return = "UNKNOWN";
		break;
	}

	return type_return + " (" + std::to_string(variable_type) + ")";
}

std::string getTheatreStructure(gStringSettings theatre_storage)
{
	std::string structure_out = "Theatre Structure\n\nTheatre \"" + theatre_storage[0][0].second.second + "\"\n";

	for(int i = 1 ; i < theatre_storage.size() ; i++)
	{
		structure_out += "\n\t" + theatre_storage[i][0].first + " \"" + theatre_storage[i][0].second.second + "\"\n";
		for(int it = 1 ; it < theatre_storage[i].size() ; it++)
		{
			gStringSetting setting = theatre_storage[i][it];
			structure_out += "\t\t" + setting.first + " = " + setting.second.second + " (type: " + getVariableTypeName(setting.second.first) + ")\n";
		}
	}

	return structure_out;
}

int getClassHash(std::string class_name, bool dont_print_error)
{
	std::string class_name_checked = class_name;

	if(class_name.back() == ':')
		class_name_checked = class_name.substr(0, class_name.size() - 1);

	for(auto &pair : graphx::classnames)
		if(!pair.second.compare(class_name_checked)) // true if equal
			return pair.first;
	if(!dont_print_error)
		PRINTERR("Class name \"" << class_name_checked << "\" not found in \"graphx::classnames\"!\n\tSolution 1: Add it!\n\tSolution 2: Fix typo!\n\tSolution 3: Uhoh...")
	return -1;
}

void interpretCppReference(gSettings &current_object_settings, std::string variable_name, std::string cpp_reference)
{
	if(!cpp_definitions.contains(cpp_reference))
	{
		PRINTERR("Tried to load a non-existing C++ Reference Variable \"" << cpp_reference << "\"!")
		return;
	}

	current_object_settings[variable_name] = gSetting(CPP_REFERENCE, cpp_definitions.at(cpp_reference));
}

void interpretRawData(gSettings &current_object_settings, std::string variable_name, std::string raw_data)
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

	std::string buffer = "";
	gRawData vector_buffer;
	bool is_number = true;

	for(char &character : raw_data)
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
				continue;

			is_number = false;
			break;
		}

		buffer += character;
	}

	if(is_number)
	{
		vector_buffer.insert(vector_buffer.end(), buffer);
		current_object_settings[variable_name] = gSetting(RAW_DATA, vector_buffer);
		return;
	}

	current_object_settings[variable_name] = gSetting(RAW_DATA, gRawData{raw_data});
}

// This is how I keep track of supported file types/extensions without having to write them out more than once.
// I define specific file types as strings that contain all the supported file extensions and I
// add all these strings to "valid_extensions", which is what "loadExternalFile" uses to check if a
// setting is referencing a supported file type.
std::string three_dee_model_extensions = "obj";
std::string graphx_theatre_extensions = "gt";
std::string image_extensions = "png jpg jpeg bmp webp";

std::string valid_extensions =   \
	three_dee_model_extensions + \
	graphx_theatre_extensions  + \
	image_extensions;

// This is just me making the error printout easier to find and add to
std::string what_are_the_valid_extensions =              \
	"(GraphXTheatre)\n\t" + graphx_theatre_extensions  + \
	"(3D Model)\n\t"      + three_dee_model_extensions + \
	"(Image)\n\t"         + image_extensions;

void interpretExternalReference(gSettings &current_object_settings, std::string variable_name, std::string external_reference)
{
	std::string file_extension = external_reference.substr(external_reference.find_last_of(".") + 1);

	if(valid_extensions.find(file_extension) == std::string::npos)
	{
		PRINTERR("Tried to interpret an External Reference setting for a file type that is not supported! Supported files are:\n" << what_are_the_valid_extensions)
		return;
	}

	if(three_dee_model_extensions.find(file_extension) != std::string::npos)
	{
		current_object_settings[variable_name] = gSetting(EXTERNAL_REFERENCE, M_LoadModelFile(external_reference, file_extension));
	}

	else if(graphx_theatre_extensions.find(file_extension) != std::string::npos)
	{
		PRINTERR("Tried to interpret an External Reference for a GraphXTheatre file, but support for child Theatres hasn't yet been implemented!")
		return;
	}

	else if(image_extensions.find(file_extension) != std::string::npos)
	{
		PRINTERR("Tried to interpret an External Reference for an image file, but support for images hasn't yet been implemented!")
		return;
	}
}

/*void interpretSandwich(gSettings &current_object_settings, std::string variable_name, std::string theatre_reference, Theatre &new_theatre)
{
	int class_hash = getClassHash(variable_name);

	if(ACTORS[0] <= class_hash && class_hash <= ACTORS[1])
	{
		current_object_settings[variable_name] = gSetting(SANDWICH_BUN, actor_map[class_hash]());
		std::any_cast<Actor *>(current_object_settings.at(variable_name).second)->youGotACallBack(new_theatre.getActor(theatre_reference)->settings);
	}

	else if(DEVICES[0] <= class_hash && class_hash <= DEVICES[1])
	{
		current_object_settings[variable_name] = gSetting(SANDWICH_BUN, device_map[class_hash]());
		std::any_cast<Device *>(current_object_settings.at(variable_name).second)->loadSettings(new_theatre.getDevice(theatre_reference)->settings);
	}
}*/

void interpretTheatreReference(gSettings &current_object_settings, std::string variable_name, std::string theatre_reference, Theatre &new_theatre, gStringSettings &theatre_settings)
{
	std::string class_name = variable_name;
	if(variable_name.find(':') != std::string::npos)
		class_name = variable_name.substr(variable_name.find_last_of(':') + 1);
	int class_hash = getClassHash(class_name, true);

	if(class_hash == -1) // If true, this is a reference to a variable of the same name in another Actor/Device
	{
		gStringSetting referenced_setting(gKey("EMPTY"), gValue(-1, "EMPTY"));
		// BEHOLD!!!
		// the most disgusting for-if-for-if nest you have EVER SEEN!!!
		// fuck you, this shouldn't affect performance and I really can't be assed to make anything better...
		// for now. I'll probably get around to changing this later on down the line...
		// ...probably...
		// ...maybe...
		for(std::vector<gStringSetting> object_settings : theatre_settings)
		{
			if(!object_settings[0].second.second.compare(theatre_reference))
			{
				for(gStringSetting string_setting : object_settings)
				{
					if(!string_setting.first.compare(variable_name))
					{
						referenced_setting = string_setting;
						break;
					}
				}
			}
		}

		switch(referenced_setting.second.first)
		{
		case CPP_REFERENCE:
			interpretCppReference(current_object_settings, referenced_setting.first, referenced_setting.second.second);
			return;
		case RAW_DATA:
			interpretRawData(current_object_settings, referenced_setting.first, referenced_setting.second.second);
			return;
		case THEATRE_REFERENCE:
			interpretTheatreReference(current_object_settings, referenced_setting.first, referenced_setting.second.second, new_theatre, theatre_settings);
			return;
		case EXTERNAL_REFERENCE:
			interpretExternalReference(current_object_settings, referenced_setting.first, referenced_setting.second.second);
			return;
		// case SANDWICH:
			// I really don't think this is possible or will result in kind things, but better here than not I guess
			// interpretSandwich(current_object_settings, referenced_setting.first, referenced_setting.second.second, new_theatre);
			// return;
		default:
			PRINTERR("A Theatre reference variable either referenced a nonexisting Actor/Device, or referenced one that didn't define the variable it wanted! (or my code fucked up)\n\tVariable Name: " << variable_name << "\n\tReference Name: " << theatre_reference)
			return;
		}
	}

	// If the abomination above didn't fire off, this is a typical pointer-style reference
	if(ACTORS[0] <= class_hash && class_hash <= ACTORS[1])
	{
		current_object_settings[variable_name] = gSetting(THEATRE_REFERENCE, actor_map[class_hash]());
		std::any_cast<Actor *>(current_object_settings.at(variable_name).second)->youGotACallBack(new_theatre.getActor(theatre_reference)->settings);
	}

	else if(DEVICES[0] <= class_hash && class_hash <= DEVICES[1])
	{
		current_object_settings[variable_name] = gSetting(THEATRE_REFERENCE, device_map[class_hash]());
		std::any_cast<Device *>(current_object_settings.at(variable_name).second)->loadSettings(new_theatre.getDevice(theatre_reference)->settings);
	}
}

void interpretSandwich(gSettings &current_object_settings, gStringSettings &theatre_settings, int &i, int &it, Theatre &new_theatre)
{
	gSettings sandwich_settings;
	int sandwich_bun_index = it;
	int class_hash = getClassHash(theatre_settings[i][sandwich_bun_index].first);

	if(ACTORS[0] <= class_hash && class_hash <= ACTORS[1])
		sandwich_settings = new_theatre.getActor(theatre_settings[i][sandwich_bun_index].second.second)->settings;
	else
		sandwich_settings = new_theatre.getDevice(theatre_settings[i][sandwich_bun_index].second.second)->settings;

	sandwich_settings["Name"] = gSetting(RAW_DATA, gRawData{theatre_settings[i][sandwich_bun_index].second.second});

	it++;
	while(theatre_settings[i][it].first.find(':') != std::string::npos)
	{
		std::string sandwich_variable = theatre_settings[i][it].first.substr(theatre_settings[i][it].first.find_last_of(':') + 1);

		switch(theatre_settings[i][it].second.first - SANDWICH)
		{
		case CPP_REFERENCE:
			interpretCppReference(sandwich_settings, sandwich_variable, theatre_settings[i][it].second.second);
			break;
		case RAW_DATA:
			interpretRawData(sandwich_settings, sandwich_variable, theatre_settings[i][it].second.second);
			break;
		case THEATRE_REFERENCE:
			interpretTheatreReference(sandwich_settings, sandwich_variable, theatre_settings[i][it].second.second, new_theatre, theatre_settings);
			break;
		case EXTERNAL_REFERENCE:
			interpretExternalReference(sandwich_settings, sandwich_variable, theatre_settings[i][it].second.second);
			break;
		default:
			PRINTERR("A Sandwich variable... got really fucked up")
			break;
		}

		it++;
	}

	if(ACTORS[0] <= class_hash && class_hash <= ACTORS[1])
	{
		current_object_settings[theatre_settings[i][sandwich_bun_index].first] = gSetting(SANDWICH, actor_map[class_hash]());
		Actor *sandwich_bun = std::any_cast<Actor *>(current_object_settings.at(theatre_settings[i][sandwich_bun_index].first).second);
		sandwich_bun->youGotACallBack(sandwich_settings);
	}

	else if(DEVICES[0] <= class_hash && class_hash <= DEVICES[1])
	{
		current_object_settings[theatre_settings[i][sandwich_bun_index].first] = gSetting(SANDWICH, device_map[class_hash]());
		Device *sandwich_bun = std::any_cast<Device *>(current_object_settings.at(theatre_settings[i][sandwich_bun_index].first).second);
		sandwich_bun->loadSettings(sandwich_settings);
	}
}

// loadTheatre should not be called directly, which is why it's not in the header file
Theatre loadTheatre(long theatre_uid)
{
	if(!embedded_theatres.count(theatre_uid))
	{
		PRINTERR("Tried to load a Theatre with UID " << std::quoted(std::to_string(theatre_uid)) << " but no Theatre with that UID exists!")
		return Theatre("DEFAULT ERROR RETURN THEATRE RETURNED BY \"loadTheatre\"");
	}

	gStringSettings theatre_settings = theatreParser(embedded_theatres.at(theatre_uid));

	Theatre new_theatre = Theatre(theatre_settings[0][0].second.second, theatre_uid);
	new_theatre.graphx_theatre_settings = theatre_settings;
	new_theatre.theatre_file_data_printout = getTheatreStructure(new_theatre.graphx_theatre_settings);

	PRINTDEBUG("Loading Theatre \"" << new_theatre.name << "\"")

	std::vector<std::pair<int, gSettings>> all_class_settings;

	for(int i = 1 ; i < theatre_settings.size() ; i++)
	{
		gSettings current_object_settings = {{"Name", gSetting(RAW_DATA, gRawData{theatre_settings[i][0].second.second})}};

		for(int it = 1 ; it < theatre_settings[i].size() ; it++)
		{
			gStringSetting setting = theatre_settings[i][it];

			switch(setting.second.first)
			{
			case CPP_REFERENCE:
				interpretCppReference(current_object_settings, setting.first, setting.second.second);
				break;
			case RAW_DATA:
				interpretRawData(current_object_settings, setting.first, setting.second.second);
				break;
			case THEATRE_REFERENCE:
				interpretTheatreReference(current_object_settings, setting.first, setting.second.second, new_theatre, theatre_settings);
				break;
			case EXTERNAL_REFERENCE:
				interpretExternalReference(current_object_settings, setting.first, setting.second.second);
				break;
			case SANDWICH:
				interpretSandwich(current_object_settings, theatre_settings, i, it, new_theatre);
				break;
			}
		}

		int class_hash = getClassHash(theatre_settings[i][0].first);

		if(ACTORS[0] <= class_hash && class_hash <= ACTORS[1])
		{
			new_theatre.createActor(class_hash, i, current_object_settings);
			continue;
		}

		if(!theatre_settings[i][0].first.compare("Stage"))
		{
			new_theatre.stage.youGotACallBack(current_object_settings);
			new_theatre.stage_mesh->loadSettings(current_object_settings);
			new_theatre.loadStageSettings(current_object_settings);
			PRINTDEBUG("Theatre Stage \"" << new_theatre.stage.getName() << "\" given custom settings")
			continue;
		}

		new_theatre.createDevice(class_hash, i, current_object_settings);
	}

	return new_theatre;
}

void loadMainTheatre(long theatre_uid)
{
	if(current_theatre.getUID() == theatre_uid)
	{
		PRINTERR("A Theatre with UID " << std::quoted(std::to_string(theatre_uid)) << " cannot be loaded because it's already the current Theatre (or the current Theatre has the same UID)!")
		return;
	}

	loading_new_main_theatre = true;
	time_to_render = false;
	time_to_store_buffers = false;

	current_theatre.dropCurtains();
	current_theatre = loadTheatre(theatre_uid);
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

	PRINTNOTE("loadChildTheatre called but this function is currently empty")
	// NEEDS TO BE FILLED OUT
}

bool checkForExternalTheatres()
{
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

	PRINTDEBUG("Loading external GraphXTheatre file!")

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

	if(embedded_theatres.contains(theatre_uid) && embedded_theatres.at(theatre_uid).compare(theatre_file_data_stream.str()))
		PRINTNOTE("An external Theatre will override the embedded Theatre with UID #" << theatre_uid)

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