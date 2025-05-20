#include "t_interpreter.hpp"
#include "t_settings.hpp"
#include "graphx_interpreter_lookups.hpp"
#include "sanity_executable_locator.hpp"
#include "sanity_printouts.hpp"
#include "r_rendering.hpp"
#include "g_theatre.hpp"
#include <theatres.hpp>
#include <set>
#include <fstream> // Yes, the devil hath been invoked... I'm sorry

// Todo: Move this into graphx_namespace.cpp when that gets made
// GraphX
GraphXTheatreInterpreter graphx::Interpreter = GraphXTheatreInterpreter();

bool loading_new_main_theatre = true; // Definitely wanna replace this with something a little more sophisticated.
// std::string empty_settings_identifier = "FUCKYOU";
// gSettings empty_settings = {{empty_settings_identifier, graphx::gSetting(-1, {})}};

//--------------
// StringSetting
//--------------
StringSetting::StringSetting()
: name(""), value(""), category(UNDEFINED)
{}

StringSetting::StringSetting(const std::string& new_name, const std::string& new_value, const unsigned int new_category)
: name(new_name), value(new_value), category(new_category)
{}

void StringSetting::changeSetting(const std::string& new_value, const int new_category)
{
	value = new_value;
	category = (new_category != LEAVE_UNCHANGED) ? new_category : category;
}

//-------------------------
// GraphXTheatreInterpreter
//-------------------------
std::string GraphXTheatreInterpreter::validExtensions() const
{
	return three_dee_model_extensions + graphx_theatre_extensions + image_extensions;
}

std::vector<StringSettings> GraphXTheatreInterpreter::theatreParser(std::string theatre_data)
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

	StringSettings object_settings;
	std::vector<StringSettings> all_settings; // Contains all object string settings

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

				all_settings.insert(all_settings.end(), {StringSetting("TheatreName", buffer, StringSetting::RAW_DATA)});
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

				object_settings.at(0).changeSetting(buffer, StringSetting::RAW_DATA);
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
					object_settings.insert(object_settings.end(), StringSetting(buffer, "", StringSetting::RAW_DATA));
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
					setting_type_identifier = StringSetting::RAW_DATA;
					break;
				case begin_cxx_reference:
					setting_type_identifier = StringSetting::CPP_REFERENCE;
					break;
				case begin_external_reference:
					setting_type_identifier = StringSetting::EXTERNAL_REFERENCE;
					break;
				case begin_theatre_reference:
					setting_type_identifier = StringSetting::THEATRE_REFERENCE;
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

			object_settings.at(current_setting_index).category += setting_type_identifier;
			object_settings.at(current_setting_index).value = buffer;
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
					object_settings.insert(object_settings.end(), StringSetting(sandwich_bun + buffer, "", StringSetting::SANDWICH));
					setting_type_padding = StringSetting::SANDWICH;

					if(sandwich_bun.empty())
					{
						sandwich_bun = buffer + ":";
						object_settings.back().category -= StringSetting::THEATRE_REFERENCE;
					}

					buffer = "";
					character = theatre_data[++i];
				}

				buffer += character;
				character = theatre_data[++i];
			}

			if(buffer.size() > 0)
			{
				object_settings.insert(object_settings.end(), StringSetting(sandwich_bun + buffer, "", setting_type_padding));
				buffer = "";
			}

			break;
		}
	}

	return all_settings;
}

std::string GraphXTheatreInterpreter::getCategoryString(int variable_type)
{
	std::string type_return;

	switch(variable_type)
	{
	case StringSetting::CPP_REFERENCE:
		type_return = "StringSetting::CPP_REFERENCE";
		break;
	case StringSetting::THEATRE_REFERENCE:
		type_return = "StringSetting::THEATRE_REFERENCE";
		break;
	case StringSetting::RAW_DATA:
		type_return = "StringSetting::RAW_DATA";
		break;
	case StringSetting::EXTERNAL_REFERENCE:
		type_return = "StringSetting::EXTERNAL_REFERENCE";
		break;
	case StringSetting::SANDWICH:
		type_return = "StringSetting::SANDWICH";
		break;
	default:
		if(variable_type > StringSetting::SANDWICH)
			type_return = getCategoryString(variable_type - StringSetting::SANDWICH);
		else
			type_return = "UNKNOWN";
		break;
	}

	return type_return + " (" + std::to_string(variable_type) + ")";
}

std::string GraphXTheatreInterpreter::getTheatreStructure(const std::vector<StringSettings>& all_settings)
{
	std::string structure_out = "Theatre Structure\n\nTheatre \"" + all_settings.at(0).at(0).value + "\"\n";

	for(int i = 1 ; i < all_settings.size() ; i++)
	{
		structure_out += "\n\t" + all_settings.at(i).at(0).name + " (Assumed UID: " + std::to_string(i) + ") \"" + " \"" + all_settings.at(i).at(0).value + "\"\n";
		for(int it = 1 ; it < all_settings.at(i).size() ; it++)
			structure_out += "\t\t" + all_settings.at(i).at(it).name + " = " + all_settings.at(i).at(it).value + " (type: " + getCategoryString(all_settings.at(i).at(it).category) + ")\n";
	}

	return structure_out;
}

void GraphXTheatreInterpreter::interpretCppReference(gSettings &current_object_settings, const std::string& variable_name, const std::string& cpp_reference)
{
	if(!cpp_definitions.contains(cpp_reference))
	{
		PRINTERR("Tried to load a non-existing C++ Reference Variable \"" << cpp_reference << "\"!")
		return;
	}

	current_object_settings.cpp_reference[variable_name] = cpp_definitions.at(cpp_reference);
}

void GraphXTheatreInterpreter::interpretRawData(gSettings &current_object_settings, const std::string& variable_name, const std::string& raw_data)
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

	for(const char& character : raw_data)
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
		current_object_settings.raw_data[variable_name] = vector_buffer;
		return;
	}

	current_object_settings.raw_data[variable_name] = gRawData{raw_data};
}

void GraphXTheatreInterpreter::interpretExternalReference(gSettings& current_object_settings, const std::string& variable_name, const std::string& external_reference)
{
	std::string file_extension = external_reference.substr(external_reference.find_last_of(".") + 1);

	if(graphx::Interpreter.validExtensions().find(file_extension) == std::string::npos)
	{
		PRINTERR("Tried to interpret an External Reference setting for a file type that is not supported! Supported files are:\n" << what_are_the_valid_extensions)
		return;
	}

	if(three_dee_model_extensions.find(file_extension) != std::string::npos)
	{
		current_object_settings.external_reference[variable_name] = M_LoadModelFile(external_reference, file_extension);
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

void GraphXTheatreInterpreter::interpretTheatreReference(gSettings& current_object_settings, const std::string& variable_name, const std::string& theatre_reference, Theatre& new_theatre, std::vector<StringSettings>& theatre_settings)
{
	std::string class_name = variable_name;
	if(variable_name.find(':') != std::string::npos)
		class_name = variable_name.substr(variable_name.find_last_of(':') + 1);

	if(!gClasses::isDefined(class_name)) // If true, this is a reference to a variable of the same name in another Actor/Device
	{
		StringSetting referenced_setting(variable_name, "EMPTY", StringSetting::UNDEFINED);

		// BEHOLD!!!
		// the most disgusting for-if-for-if nest you have EVER SEEN!!!
		// fuck you, this shouldn't affect performance and I really can't be assed to make anything better...
		// for now. I'll probably get around to changing this later on down the line...
		// ...probably...
		// ...maybe...
		for(const StringSettings& object_settings : theatre_settings) // Potential fuck ups here, lmfao
		{
			if(object_settings.at(0).value.compare(theatre_reference))
				continue;

			for(const StringSetting& setting : object_settings)
			{
				if(setting.name.compare(variable_name))
					continue;
				referenced_setting.value = setting.value;
				break;
			}
		}

		switch(referenced_setting.category)
		{
		case StringSetting::CPP_REFERENCE:
			interpretCppReference(current_object_settings, referenced_setting.name, referenced_setting.value);
			return;
		case StringSetting::RAW_DATA:
			interpretRawData(current_object_settings, referenced_setting.name, referenced_setting.value);
			return;
		case StringSetting::THEATRE_REFERENCE:
			interpretTheatreReference(current_object_settings, referenced_setting.name, referenced_setting.value, new_theatre, theatre_settings);
			return;
		case StringSetting::EXTERNAL_REFERENCE:
			interpretExternalReference(current_object_settings, referenced_setting.name, referenced_setting.value);
			return;
		default:
			PRINTERR("A Theatre reference variable either referenced a nonexisting Actor/Device, or referenced one that didn't define the variable it wanted! (or my code fucked up)\n\tVariable Name: " << variable_name << "\n\tReference Name: " << theatre_reference)
			return;
		}
	}

	// If the abomination above didn't fire off, this is a typical pointer-style reference
	if(gClasses::isActor(class_name))
		current_object_settings.actor_reference[variable_name] = new_theatre.getActor(theatre_reference);

	else if(gClasses::isDevice(class_name))
		current_object_settings.device_reference[variable_name] = new_theatre.getDevice(theatre_reference);
}

void GraphXTheatreInterpreter::interpretSandwich(gSettings& current_object_settings, std::vector<StringSettings>& theatre_settings, const std::string& current_object_name, int& i, int& it, const unsigned long& settings_size, Theatre& new_theatre)
{
	gSettings sandwich_settings;
	StringSetting sandwich_bun_setting = theatre_settings[i][it];

	if(gClasses::isActor(sandwich_bun_setting.name))
		sandwich_settings = new_theatre.getActor(sandwich_bun_setting.value)->getSettings();
	else
		sandwich_settings = new_theatre.getDevice(sandwich_bun_setting.value)->getSettings();

	sandwich_settings.raw_data["Name"] = gRawData{sandwich_bun_setting.value + "_" + current_object_name};

	it++;
	while(theatre_settings.at(i).at(it).category > StringSetting::SANDWICH && it < settings_size)
	{
		std::string sandwich_variable = theatre_settings.at(i).at(it).name.substr(theatre_settings.at(i).at(it).name.find_last_of(':') + 1);
		std::string sandwich_value = theatre_settings.at(i).at(it).value;

		switch(theatre_settings.at(i).at(it).category - StringSetting::SANDWICH)
		{
		case StringSetting::CPP_REFERENCE:
			interpretCppReference(sandwich_settings, sandwich_variable, sandwich_value);
			break;
		case StringSetting::RAW_DATA:
			interpretRawData(sandwich_settings, sandwich_variable, sandwich_value);
			break;
		case StringSetting::THEATRE_REFERENCE:
			interpretTheatreReference(sandwich_settings, sandwich_variable, sandwich_value, new_theatre, theatre_settings);
			break;
		case StringSetting::EXTERNAL_REFERENCE:
			interpretExternalReference(sandwich_settings, sandwich_variable, sandwich_value);
			break;
		default:
			PRINTERR("A Sandwich variable... got really fucked up")
			break;
		}

		it++;
	}

	int temp_uid = i+1080; // TODO: MAKE A RANDOM UID GENERATOR!!

	if(gClasses::isActor(sandwich_bun_setting.name))
	{
		// Actor* sandwich_bun = valid_actors.at(sandwich_bun_setting.name)(&new_theatre, -1, sandwich_settings);
		new_theatre.createActor(sandwich_bun_setting.name, temp_uid, sandwich_settings);
		current_object_settings.actor_reference[sandwich_bun_setting.name] = new_theatre.getActor(temp_uid);
	}

	else if(gClasses::isDevice(sandwich_bun_setting.name))
	{
		// Device* sandwich_bun = valid_devices.at(sandwich_bun_setting.name)(&new_theatre, -1, sandwich_settings);
		new_theatre.createDevice(sandwich_bun_setting.name, temp_uid, sandwich_settings);
		current_object_settings.device_reference[sandwich_bun_setting.name] = new_theatre.getDevice(temp_uid);
	}

	it--;
}

// loadTheatre should not be called directly, which is why it's not in the header file
void GraphXTheatreInterpreter::loadTheatre(const long theatre_uid, Theatre& new_theatre)
{
	if(!embedded_theatres.count(theatre_uid))
	{
		PRINTERR("Tried to load a Theatre with UID " << std::quoted(std::to_string(theatre_uid)) << " but no Theatre with that UID exists!")
		return;
	}

	std::vector<StringSettings> theatre_settings = theatreParser(embedded_theatres.at(theatre_uid));

	new_theatre = Theatre(theatre_settings.at(0).at(0).value, theatre_uid);
	new_theatre.graphx_theatre_settings = theatre_settings;
	new_theatre.theatre_file_data_printout = getTheatreStructure(new_theatre.graphx_theatre_settings);

	PRINTDEBUG("Loading Theatre \"" << new_theatre.name << "\"")

	for(int i = 1 ; i < theatre_settings.size() ; i++)
	{
		gSettings current_object_settings;
		current_object_settings.raw_data["Name"] = gRawData{theatre_settings.at(i).at(0).value};

		for(int it = 1 ; it < theatre_settings[i].size() ; it++)
		{
			StringSetting setting = theatre_settings[i][it];

			switch(setting.category)
			{
			case StringSetting::CPP_REFERENCE:
				interpretCppReference(current_object_settings, setting.name, setting.value);
				break;
			case StringSetting::RAW_DATA:
				interpretRawData(current_object_settings, setting.name, setting.value);
				break;
			case StringSetting::THEATRE_REFERENCE:
				interpretTheatreReference(current_object_settings, setting.name, setting.value, new_theatre, theatre_settings);
				break;
			case StringSetting::EXTERNAL_REFERENCE:
				interpretExternalReference(current_object_settings, setting.name, setting.value);
				break;
			case StringSetting::SANDWICH:
				interpretSandwich(current_object_settings, theatre_settings, theatre_settings[i][0].value, i, it, theatre_settings[i].size(), new_theatre);
				break;
			}
		}

		if(gClasses::isActor(theatre_settings.at(i).at(0).name))
		{
			new_theatre.createActor(theatre_settings[i][0].name, i, current_object_settings);
			continue;
		}

		if(!theatre_settings[i][0].name.compare("Stage"))
		{
			new_theatre.stage.youGotACallBack();
			new_theatre.stage_mesh->loadSettings();
			new_theatre.loadStageSettings(current_object_settings);
			PRINTDEBUG("Theatre Stage \"" << new_theatre.stage.name << "\" given custom settings")
			continue;
		}

		new_theatre.createDevice(theatre_settings[i][0].name, i, current_object_settings);
	}
}

void I_LoadNewMainTheatre(long theatre_uid)
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
	graphx::Interpreter.loadTheatre(theatre_uid, graphx::current::theatre);
	graphx::current::theatre.raiseCurtains();
	jolt_physics_system.OptimizeBroadPhase();

	time_to_store_buffers = true;
	loading_new_main_theatre = false;
}

void I_LoadChildTheatre(long theatre_uid, Theatre *parent_theatre)
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

void I_EmbedExternalTheatre(std::filesystem::path theatre_file_path)
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

bool I_CheckForAndLoadExternalTheatres()
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
			I_EmbedExternalTheatre(entry.path());
		}
	}

	if(!has_theatre_file)
		PRINTNOTE("External GraphXTheatre directory exists, but no GraphXTheatre files were found.")

	return has_theatre_file;
}