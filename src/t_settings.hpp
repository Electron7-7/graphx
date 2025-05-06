#ifndef GRAPHX_SETTINGS
#include "sanity_printouts.hpp"
#include <string>
#include <map>
#include <any>
#include <vector>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#define GRAPHX_SETTINGS

#ifdef COMPILER_FORWARD_DECLARATIONS // Forward declarations
class Actor;
struct Device;
class GraphXTheatreInterpreter;
#endif

typedef std::vector<std::string> gRawData;

struct gSettings
{
public:
	gSettings() = default;

	template<typename V> void getVariable(const std::string& setting, V& variable) const
	{
	    if(cpp_reference.contains(setting))
	    {
	        if(typeid(V) == cpp_reference.at(setting).type())
	        {
	            variable = std::any_cast<V>(cpp_reference.at(setting));
	            return;
	        }

	        PRINTERR("in gSettings::getVariable - setting and variable are not the same type! Setting: " << setting)
	        return;
	    }

	    PRINTERR("in gSettings::getVariable - cpp_reference does not contain setting: " << setting)
	}

	template<typename V> void getNumber(const std::string& Setting, V& Variable) const;

	void getBoolean(const std::string& Setting, bool& Variable) const;
	void getString(const std::string& Setting, std::string& Variable) const;
	void getResource(const std::string& setting, std::string& variable) const;

	void getActorUID(const std::string& Setting, int& Variable) const;
	void getDeviceUID(const std::string& Setting, int& Variable) const;
	void getVariable(const std::string& Setting, std::string& Variable) const;

private:
	friend GraphXTheatreInterpreter;

	std::map<std::string, gRawData> raw_data = {}; // numeric, boolean, and string
	std::map<std::string, std::any> cpp_reference = {};
	std::map<std::string, int> actor_reference = {};
	std::map<std::string, int> device_reference = {};
	std::map<std::string, std::string> external_reference = {};
};

// Skips having to manually set all the Actor variables for every derived class
void configureBaseVariables(Actor* actor);
void configureBaseVariables(Device* actor);
#endif