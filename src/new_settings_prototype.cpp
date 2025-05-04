#include <iostream>
#include <map>
#include <memory>

template<typename T> void PRINT(T x) { if constexpr(std::is_arithmetic_v<T>) { std::cout << std::to_string(x) << std::endl; } else { std::cout << std::boolalpha << std::string(x) << std::endl; } }

struct cpp_type
{
	std::string identifier;

	cpp_type(const std::string& new_ident = "UNDEFINED"): identifier(new_ident) {}
};

cpp_type reference_me = cpp_type("Reference Me");

std::map<std::string, cpp_type> cpp_map =
{ { "ReferenceMe", reference_me } };

class Actor
{
public:
	std::string name;
	std::string this_has_no_setting = "Hopefully you see this!";
	int number = -1;
	cpp_type cpp_reference = cpp_type();
	cpp_type high_level_code_setup = cpp_type();
	std::shared_ptr<Actor> theatre_reference = nullptr;
	std::string fake_external_reference = "unloaded";

	Actor(const std::string& new_name = "Untitled Actor"): name(new_name) {}
	virtual ~Actor() = default;

	void Print()
	{
		PRINT("\nActor::Print()");
		PRINT(name);
		PRINT(this_has_no_setting);
		PRINT(number);
		PRINT(cpp_reference.identifier);
		PRINT(high_level_code_setup.identifier);
		if(theatre_reference == nullptr)
			PRINT("nullptr");
		else
			PRINT(theatre_reference->name);
		PRINT(fake_external_reference);
	}

	virtual void PrintOut()
	{
		PRINT("\nActor::PrintOut()");
		PRINT(name);
		PRINT(this_has_no_setting);
		PRINT(number);
		PRINT(cpp_reference.identifier);
		PRINT(high_level_code_setup.identifier);
		if(theatre_reference == nullptr)
			PRINT("nullptr");
		else
			PRINT(theatre_reference->name);
		PRINT(fake_external_reference);
	}
};

class DerivedActor: public Actor
{
public:
	DerivedActor(const std::string& new_name = "Untitled DerivedActor"): Actor(new_name) {}

	virtual void PrintOut() override
	{
		PRINT("\nDerivedActor::PrintOut()");
		PRINT(name);
		PRINT(this_has_no_setting);
		PRINT(number);
		PRINT(cpp_reference.identifier);
		PRINT(high_level_code_setup.identifier);
		if(theatre_reference == nullptr)
			PRINT("nullptr");
		else
			PRINT(theatre_reference->name);
		PRINT(fake_external_reference);
	}
};

struct Theatre
{
public:
	std::map<int, std::shared_ptr<Actor>> actors = {{}};

	Theatre() = default;

	std::shared_ptr<Actor> getActor(int id)
	{
		if(actors.contains(id))
			return actors.at(id);
		return nullptr;
	}

	std::shared_ptr<Actor> getActor(const std::string& name)
	{
		for(auto& actor_pair : actors)
			if(!actor_pair.second->name.compare(name))
				return actor_pair.second;
		return nullptr;
	}
};

Theatre current_theatre;

std::string test_settings_file_representation =
R"~(
Actor (ActorThatExistsAlready)
{}

DerivedActor (DerivedActor With Loaded Settings)
{
	Number (481516)
	C++ Reference [ReferenceMe]
	High Level Test [SayHello]
	Theatre Reference <ActorThatExistsAlready>
	External Reference "true"
}
)~";

// Actual implementation would be a struct with separate maps for each defined type of setting
std::map<std::string, std::string> test_numeric_settings =
{
	{ "Number", "481516" },
};

std::map<std::string, std::string> test_string_settings =
{
	{ "Name", "DerivedActor With Loaded Settings" },
};

std::map<std::string, std::string> test_cpp_reference_settings =
{
	{ "C++ Reference", "ReferenceMe" },
};

std::map<std::string, std::string> test_high_level_settings =
{
	{ "High Level Test", "SayHello" },
};

std::map<std::string, std::string> test_theatre_reference_settings =
{
	{ "Theatre Reference", "ActorThatExistsAlready" },
};

std::map<std::string, std::string> test_external_reference_settings =
{
	{ "External Reference", "true" },
};



void getNumeric(auto& variable, const std::string& setting)
{
	if(!test_numeric_settings.contains(setting))
	{
		PRINT("No setting: " + setting);
		return;
	}

	// full version: checks for real numbers vs whole numbers and vectors/glm vs scalars and all that shit
	variable = std::stol(test_numeric_settings.at(setting));
}

void getString(auto& variable, const std::string& setting)
{
	if(!test_string_settings.contains(setting))
	{
		PRINT("No setting: " + setting);
		return;
	}

	variable = test_string_settings.at(setting);
}

void getCPP(auto& variable, const std::string& setting)
{
	if(!test_cpp_reference_settings.contains(setting))
	{
		PRINT("No setting: " + setting);
		return;
	}

	if(!cpp_map.contains(test_cpp_reference_settings.at(setting)))
	{
		PRINT("No cpp reference: " + test_cpp_reference_settings.at(setting));
		return;
	}

	variable = cpp_map.at(test_cpp_reference_settings.at(setting));
}

void getHighLevel(auto& variable, const std::string& setting)
{
	if(!test_high_level_settings.contains(setting))
	{
		PRINT("No setting: " + setting);
		return;
	}

	if(!test_high_level_settings.at(setting).compare("SayHello"))
		variable = cpp_type("Hello!");
}

void getTheatreReference(auto& variable, const std::string& setting)
{
	if(!test_theatre_reference_settings.contains(setting))
	{
		PRINT("No setting: " + setting);
		return;
	}

	variable = current_theatre.getActor(test_theatre_reference_settings.at(setting));
}

void getExternalReference(auto& variable, const std::string& setting)
{
	if(!test_external_reference_settings.contains(setting))
	{
		PRINT("No setting: " + setting);
		return;
	}

	// would be replaced with calls to functions that process external files
	variable = "loaded";
}

void processSettings(std::shared_ptr<Actor> actor)
{

	getString(actor->name, "Name");
	getString(actor->this_has_no_setting, "BlahBlahBlah");
	getNumeric(actor->number, "Number");
	getCPP(actor->cpp_reference, "C++ Reference");
	getHighLevel(actor->high_level_code_setup, "High Level Test");
	getTheatreReference(actor->theatre_reference, "Theatre Reference");
	getExternalReference(actor->fake_external_reference, "External Reference");

}


int main()
{
	current_theatre.actors[0] = std::shared_ptr<Actor>(new DerivedActor("ActorThatExistsAlready"));
	current_theatre.actors[1] = std::shared_ptr<Actor>(new DerivedActor());

	PRINT("\nUnprocessed Settings:");
	PRINT(current_theatre.actors.at(1)->name);
	PRINT(current_theatre.actors.at(1)->this_has_no_setting);
	PRINT(current_theatre.actors.at(1)->number);
	PRINT(current_theatre.actors.at(1)->cpp_reference.identifier);
	PRINT(current_theatre.actors.at(1)->high_level_code_setup.identifier);
	PRINT("should be nullptr");
	PRINT(current_theatre.actors.at(1)->fake_external_reference);
	current_theatre.actors.at(1)->Print();
	current_theatre.actors.at(1)->PrintOut();

	processSettings(current_theatre.actors.at(1));

	PRINT("\nProcessed Settings:");
	PRINT(current_theatre.actors.at(1)->name);
	PRINT(current_theatre.actors.at(1)->this_has_no_setting);
	PRINT(current_theatre.actors.at(1)->number);
	PRINT(current_theatre.actors.at(1)->cpp_reference.identifier);
	PRINT(current_theatre.actors.at(1)->high_level_code_setup.identifier);
	if(current_theatre.actors.at(1)->theatre_reference == nullptr)
		PRINT("nullptr");
	else
		PRINT(current_theatre.actors.at(1)->theatre_reference->name);
	PRINT(current_theatre.actors.at(1)->fake_external_reference);
	current_theatre.actors.at(1)->Print();
	current_theatre.actors.at(1)->PrintOut();

	return 0;
}