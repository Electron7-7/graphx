#include "t_common.hpp"
#include "sanity.hpp"
#include "graphx_interpreter_lookups.hpp"
#include "g_jolt.hpp"
#include "g_common.hpp"
#include "r_common.hpp"
#include <images.h>
#include <models.hpp>
#include <theatres.hpp>
#include <set>
#include <filesystem> // Yes, the devil hath been invoked... I'm sorry
#include <fstream>
#include <sstream>

bool loading_new_main_theatre = true; // Definitely wanna replace this with something a little more sophisticated.
std::string empty_settings_identifier = "FUCKYOU";
graphx::gSettings empty_settings = {{empty_settings_identifier, graphx::gSetting(-1, {})}};

graphx::interpreter::gStringSettings theatreParser(std::string theatre_data)
{
	std::set<char> whitespace =
	{
		' ',
		'	',
		'\r',
		'\n',
		'\t'
	};

	const char begin_theatre_name = '@';
	const char begin_settings = '{';
	const char begin_cxx_reference = '[';
	const char begin_theatre_reference = '<';
	const char begin_raw_data = '(';
	const char begin_external_reference = '"';

	const char end_settings = '}';
	const char end_cxx_reference = ']';
	const char end_theatre_reference = '>';
	const char end_raw_data = ')';
	const char end_external_reference = '"';

	const char sandwich_delimiter = ':';

	bool reading_settings = false;

	std::vector<graphx::interpreter::gStringSetting> object_settings;

	graphx::interpreter::gStringSettings all_settings; // Contains all object string settings

	std::string buffer = "";
	int current_setting_index = 1;

	for(int i = 0 ; i < theatre_data.size() ; i++)
	{
		char character = theatre_data[i];

		if(!theatre_data.substr(i, 2).compare("//"))
		{ // This is a comment (in the .gt file, not... not this comment...)
			while(character != '\n')
				character = theatre_data[++i];
			buffer = "";
		}

		if(!reading_settings)
		{
			switch(character)
			{
			case begin_settings:
				reading_settings = true;
				current_setting_index = 1;
				break;
			case begin_theatre_name:
				buffer = "";
				character = theatre_data[++i];

				while(!whitespace.contains(character))
				{
					buffer += character;
					character = theatre_data[++i];
				}

				all_settings.insert(all_settings.end(), {graphx::interpreter::gStringSetting(graphx::interpreter::gKey("TheatreName"), graphx::interpreter::gValue(RAW_DATA, buffer))});
				buffer = "";
				break;
			case begin_raw_data:
				buffer = "";
				character = theatre_data[++i];

				while(character != end_raw_data)
				{
					buffer += character;
					character = theatre_data[++i];
				}

				object_settings.at(0).second = graphx::interpreter::gValue(RAW_DATA, buffer);
				buffer = "";
				break;
			default:
				if(!theatre_data.substr(i, 2).compare("//"))
				{ // This is a comment (in the .gt file, not... not this comment...)
					while(character != '\n')
						character = theatre_data[++i];
					buffer = "";
				}

				while(!whitespace.contains(character))
				{
					buffer += character;
					character = theatre_data[++i];
				}

				if(buffer.size() > 0)
				{
					object_settings.insert(object_settings.end(), graphx::interpreter::gStringSetting(buffer, graphx::interpreter::gValue(RAW_DATA, "")));
					buffer = "";
					break;
				}
			}
			continue;
		}

		int setting_type_identifier = 0;
		int setting_type_padding = 0;

		switch(character)
		{
		case end_settings:
			reading_settings = false;
			all_settings.insert(all_settings.end(), object_settings);
			object_settings.clear();
			buffer = "";
			break;
		case begin_raw_data:
		case begin_cxx_reference:
		case begin_external_reference:
		case begin_theatre_reference:
			switch(character)
			{
				case begin_raw_data:
					setting_type_identifier = RAW_DATA;
					break;
				case begin_cxx_reference:
					setting_type_identifier = CPP_REFERENCE;
					break;
				case begin_external_reference:
					setting_type_identifier = EXTERNAL_REFERENCE;
					break;
				case begin_theatre_reference:
					setting_type_identifier = THEATRE_REFERENCE;
					break;
			}

			buffer = "";
			character = theatre_data[++i];

			while(character != end_raw_data && character != end_cxx_reference && character != end_external_reference && character != end_theatre_reference)
			{
				buffer += character;
				character = theatre_data[++i];
			}

			if(current_setting_index >= object_settings.size())
			{
				PRINTERR("Tried to parse too many values for number of setting variables! (current_setting_index >= object_settings.size())")
				break;
			}

			object_settings.at(current_setting_index).second.first += setting_type_identifier;
			object_settings.at(current_setting_index).second.second = buffer;
			setting_type_padding = 0;
			current_setting_index++;
			buffer = "";
			break;
		case sandwich_delimiter:
			break;
		default: // Parsing variable name(s)
			std::string sandwich_bun = "";

			if(!theatre_data.substr(i, 2).compare("//"))
			{ // This is a comment (in the .gt file, not... not this comment...)
				while(character != '\n')
					character = theatre_data[++i];
				buffer = "";
			}

			while(!whitespace.contains(character))
			{

				if(character == sandwich_delimiter)
				{
					object_settings.insert(object_settings.end(), graphx::interpreter::gStringSetting(sandwich_bun + buffer, graphx::interpreter::gValue(SANDWICH, "")));
					setting_type_padding = SANDWICH;

					if(sandwich_bun.empty())
					{
						sandwich_bun = buffer + ":";
						object_settings.back().second.first -= THEATRE_REFERENCE;
					}

					buffer = "";
					character = theatre_data[++i];
				}

				buffer += character;
				character = theatre_data[++i];
			}

			if(buffer.size() > 0)
			{
				object_settings.insert(object_settings.end(), graphx::interpreter::gStringSetting(sandwich_bun + buffer, graphx::interpreter::gValue(setting_type_padding, "")));
				buffer = "";
			}

			break;
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
		if(variable_type > SANDWICH)
			type_return = getVariableTypeName(variable_type - SANDWICH);
		else
			type_return = "UNKNOWN";
		break;
	}

	return type_return + " (" + std::to_string(variable_type) + ")";
}

std::string getTheatreStructure(graphx::interpreter::gStringSettings theatre_storage)
{
	std::string structure_out = "Theatre Structure\n\nTheatre \"" + theatre_storage[0][0].second.second + "\"\n";

	for(int i = 1 ; i < theatre_storage.size() ; i++)
	{
		structure_out += "\n\t" + theatre_storage[i][0].first + " \"" + theatre_storage[i][0].second.second + "\"\n";
		for(int it = 1 ; it < theatre_storage[i].size() ; it++)
		{
			graphx::interpreter::gStringSetting setting = theatre_storage[i][it];
			structure_out += "\t\t" + setting.first + " = " + setting.second.second + " (type: " + getVariableTypeName(setting.second.first) + ")\n";
		}
	}

	return structure_out;
}

/*int getClassHash(std::string class_name, bool dont_print_error = true)
{
	std::string class_name_checked = class_name;

	if(class_name.back() == ':')
		class_name_checked = class_name.substr(0, class_name.size() - 1);

	if(graphx::classes::classnames.contains(class_name_checked)) // true if equal
		return graphx::classes::classnames.at(class_name_checked);

	if(!dont_print_error)
		PRINTERR("Class name \"" << class_name_checked << "\" not found in \"graphx::classes::classnames\"!\n\tSolution 1: Add it!\n\tSolution 2: Fix typo!\n\tSolution 3: Uhoh...")
	return graphx::classes::INVALID_TYPE;
}*/

void interpretCppReference(graphx::gSettings &current_object_settings, std::string variable_name, std::string cpp_reference)
{
	if(!cpp_definitions.contains(cpp_reference))
	{
		PRINTERR("Tried to load a non-existing C++ Reference Variable \"" << cpp_reference << "\"!")
		return;
	}

	current_object_settings[variable_name] = graphx::gSetting(CPP_REFERENCE, cpp_definitions.at(cpp_reference));
}

void interpretRawData(graphx::gSettings &current_object_settings, std::string variable_name, std::string raw_data)
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
	graphx::interpreter::gRawData vector_buffer;
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
		current_object_settings[variable_name] = graphx::gSetting(RAW_DATA, vector_buffer);
		return;
	}

	current_object_settings[variable_name] = graphx::gSetting(RAW_DATA, graphx::interpreter::gRawData{raw_data});
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

void interpretExternalReference(graphx::gSettings &current_object_settings, std::string variable_name, std::string external_reference)
{
	std::string file_extension = external_reference.substr(external_reference.find_last_of(".") + 1);

	if(valid_extensions.find(file_extension) == std::string::npos)
	{
		PRINTERR("Tried to interpret an External Reference setting for a file type that is not supported! Supported files are:\n" << what_are_the_valid_extensions)
		return;
	}

	if(three_dee_model_extensions.find(file_extension) != std::string::npos)
	{
		current_object_settings[variable_name] = graphx::gSetting(EXTERNAL_REFERENCE, M_LoadModelFile(external_reference, file_extension));
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

void interpretTheatreReference(graphx::gSettings &current_object_settings, std::string variable_name, std::string theatre_reference, Theatre &new_theatre, graphx::interpreter::gStringSettings &theatre_settings)
{
	std::string class_name = variable_name;
	if(variable_name.find(':') != std::string::npos)
		class_name = variable_name.substr(variable_name.find_last_of(':') + 1);

	if(!gClasses::isDefined(class_name)) // If true, this is a reference to a variable of the same name in another Actor/Device
	{
		graphx::interpreter::gStringSetting referenced_setting(variable_name, graphx::interpreter::gValue(-1, "EMPTY"));

		// BEHOLD!!!
		// the most disgusting for-if-for-if nest you have EVER SEEN!!!
		// fuck you, this shouldn't affect performance and I really can't be assed to make anything better...
		// for now. I'll probably get around to changing this later on down the line...
		// ...probably...
		// ...maybe...
		for(std::vector<graphx::interpreter::gStringSetting> object_settings : theatre_settings)
		{
			if(!object_settings[0].second.second.compare(theatre_reference))
			{
				for(graphx::interpreter::gStringSetting string_setting : object_settings)
				{
					if(!string_setting.first.compare(variable_name))
					{
						referenced_setting.second = string_setting.second;
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
		default:
			PRINTERR("A Theatre reference variable either referenced a nonexisting Actor/Device, or referenced one that didn't define the variable it wanted! (or my code fucked up)\n\tVariable Name: " << variable_name << "\n\tReference Name: " << theatre_reference)
			return;
		}
	}

	// If the abomination above didn't fire off, this is a typical pointer-style reference
	if(gClasses::isActor(class_name))
		current_object_settings[variable_name] = graphx::gSetting(THEATRE_REFERENCE, new_theatre.getActor(theatre_reference));

	else if(gClasses::isDevice(class_name))
		current_object_settings[variable_name] = graphx::gSetting(THEATRE_REFERENCE, new_theatre.getDevice(theatre_reference));
}

void interpretSandwich(graphx::gSettings &current_object_settings, graphx::interpreter::gStringSettings &theatre_settings, std::string current_object_name, int &i, int &it, unsigned long settings_size, Theatre &new_theatre)
{
	graphx::gSettings sandwich_settings;
	graphx::interpreter::gStringSetting sandwich_bun_setting = theatre_settings[i][it];

	if(gClasses::isActor(sandwich_bun_setting.first))
		sandwich_settings = new_theatre.getActor(sandwich_bun_setting.second.second)->settings;
	else
		sandwich_settings = new_theatre.getDevice(sandwich_bun_setting.second.second)->settings;

	sandwich_settings["Name"] = graphx::gSetting(RAW_DATA, graphx::interpreter::gRawData{sandwich_bun_setting.second.second + "_" + current_object_name});

	it++;
	while(theatre_settings[i][it].second.first > SANDWICH && it < settings_size)
	{
		std::string sandwich_variable = theatre_settings[i][it].first.substr(theatre_settings[i][it].first.find_last_of(':') + 1);
		std::string sandwich_value = theatre_settings[i][it].second.second;
		int sandwich_value_type = theatre_settings[i][it].second.first - SANDWICH;

		switch(sandwich_value_type)
		{
		case CPP_REFERENCE:
			interpretCppReference(sandwich_settings, sandwich_variable, sandwich_value);
			break;
		case RAW_DATA:
			interpretRawData(sandwich_settings, sandwich_variable, sandwich_value);
			break;
		case THEATRE_REFERENCE:
			interpretTheatreReference(sandwich_settings, sandwich_variable, sandwich_value, new_theatre, theatre_settings);
			break;
		case EXTERNAL_REFERENCE:
			interpretExternalReference(sandwich_settings, sandwich_variable, sandwich_value);
			break;
		default:
			PRINTERR("A Sandwich variable... got really fucked up")
			break;
		}

		it++;
	}

	if(gClasses::isActor(sandwich_bun_setting.first))
	{
		Actor* sandwich_bun = valid_actors.at(sandwich_bun_setting.first)(&new_theatre, -1, sandwich_settings);
		sandwich_bun->youGotACallBack(sandwich_settings);
		current_object_settings[sandwich_bun_setting.first] = graphx::gSetting(SANDWICH, sandwich_bun);
	}

	else if(gClasses::isDevice(sandwich_bun_setting.first))
	{
		Device* sandwich_bun = valid_devices.at(sandwich_bun_setting.first)(&new_theatre, -1, sandwich_settings);
		sandwich_bun->loadSettings(sandwich_settings);
		current_object_settings[sandwich_bun_setting.first] = graphx::gSetting(SANDWICH, sandwich_bun);
	}

	it--;
}

// loadTheatre should not be called directly, which is why it's not in the header file
Theatre loadTheatre(long theatre_uid)
{
	if(!embedded_theatres.count(theatre_uid))
	{
		PRINTERR("Tried to load a Theatre with UID " << std::quoted(std::to_string(theatre_uid)) << " but no Theatre with that UID exists!")
		return Theatre("DEFAULT ERROR RETURN THEATRE RETURNED BY \"loadTheatre\"");
	}

	graphx::interpreter::gStringSettings theatre_settings = theatreParser(embedded_theatres.at(theatre_uid));

	Theatre new_theatre = Theatre(theatre_settings[0][0].second.second, theatre_uid);
	new_theatre.graphx_theatre_settings = theatre_settings;
	new_theatre.theatre_file_data_printout = getTheatreStructure(new_theatre.graphx_theatre_settings);

	PRINTDEBUG("Loading Theatre \"" << new_theatre.name << "\"")

	for(int i = 1 ; i < theatre_settings.size() ; i++)
	{
		graphx::gSettings current_object_settings = {{"Name", graphx::gSetting(RAW_DATA, graphx::interpreter::gRawData{theatre_settings[i][0].second.second})}};

		for(int it = 1 ; it < theatre_settings[i].size() ; it++)
		{
			graphx::interpreter::gStringSetting setting = theatre_settings[i][it];

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
				interpretSandwich(current_object_settings, theatre_settings, theatre_settings[i][0].second.second, i, it, theatre_settings[i].size(), new_theatre);
				break;
			}
		}

		if(gClasses::isActor(theatre_settings.at(i).at(0).first))
		{
			new_theatre.createActor(theatre_settings[i][0].first, i, current_object_settings);
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

		new_theatre.createDevice(theatre_settings[i][0].first, i, current_object_settings);
	}

	return new_theatre;
}

void loadMainTheatre(long theatre_uid)
{
	if(graphx::current::theatre.getUID() == theatre_uid)
	{
		PRINTERR("A Theatre with UID " << std::quoted(std::to_string(theatre_uid)) << " cannot be loaded because it's already the current Theatre (or the current Theatre has the same UID)!")
		return;
	}

	loading_new_main_theatre = true;
	time_to_render = false;
	time_to_store_buffers = false;

	graphx::current::theatre.dropCurtains();
	graphx::current::theatre = loadTheatre(theatre_uid);
	graphx::current::theatre.raiseCurtains();
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
	// Last minute realization that I had to move these out of the header file "sanity.hpp"
	std::string binary_path = BINARY_PATH;
	std::string theatres_directory = std::string(BINARY_PATH) + EXTERNAL_THEATRES_DIRECTORY;

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

	// Last minute realization that I had to move these out of the header file "sanity.hpp"
	std::string binary_path = BINARY_PATH;
	std::string theatres_directory = std::string(BINARY_PATH) + EXTERNAL_THEATRES_DIRECTORY;

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