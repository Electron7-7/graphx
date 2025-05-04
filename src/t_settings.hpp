#ifndef GRAPHX_SETTINGS
#include "sanity_printouts.hpp"
#include <string>
#include <map>
#include <any>
#include <vector>
#include <type_traits>
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#define GRAPHX_SETTINGS

#ifdef COMPILER_FORWARD_DECLARATIONS // Forward declarations
class Actor;
struct Device;
#endif

typedef std::vector<std::string> gRawData;

struct gSettings
{
public:
	gSettings() = default;

	template<typename V> void getRawData(const std::string& Setting, V& Variable) const
	{
	    if(!raw_data.contains(Setting) || raw_data.at(Setting).size() == 0) return;

	    // Sorry, John Carmack, but I felt the need to write some overly descriptive comments
	    if constexpr(std::is_same_v<V, glm::vec2> || std::is_same_v<V, glm::vec3> || std::is_same_v<V, glm::vec4> || std::is_same_v<V, glm::quat>)
	    {
	        // Since we know the glm variable has to be at least 2 indices large,
	        // we can use a single constexpr if statement with a ternary operator
	        // to determine the variable's index count.
	        unsigned int size = 1;
	        if constexpr(!std::is_same_v<V, glm::vec2>)
	            { size = (std::is_same_v<V, glm::vec3>) ? 2 : 3; }

	        // We already know the gRawData vector exists and isn't empty,
	        // so we can take advantage of glm swizzling if the vector only
	        // has one number, and do an early return
	        if(raw_data.at(Setting).size() == 1)
	        {
	        	float swizzle = std::stof(raw_data.at(Setting).at(0));
	        	// Unfortunately, quaternions can't be swizzled exactly the same, so I assume this is user error and use the default constructor instead
				if constexpr(std::is_same_v<V, glm::quat>)
	            {
	            	Variable = glm::quat();
	            	return;
	            }
				else
	            {
	            	Variable = V(swizzle);
	            	return;
	            }
	        }

	        // Make the rest of the code easier to read
	        const gRawData& raw_data_copy = raw_data.at(Setting);

	        // If the gRawData vector is smaller than the glm variable, fill in the
	        // indices outside of its range with zeros
	        if(raw_data_copy.size() < (size + 1))
	            for(int i = (raw_data_copy.size() - 1); i < size; i++)
	                Variable[i] = 0.0f;

	        // Fill the rest of the glm variable
	        for(int i = 0; i < raw_data_copy.size(); i++)
	            Variable[i] = std::stof(raw_data_copy.at(i));

	        return; // Just because I'm paranoid :P
	    }

	    // Get the gRawData vector if it's not empty and create a new one with a single "0" if it is
	    const gRawData& raw_data_copy = (raw_data.at(Setting).size() > 0) ? raw_data.at(Setting) : gRawData{"0"};

	    if constexpr(std::is_same_v<V, int>)    { Variable = std::stoi(raw_data_copy.at(0)); return; }
	    if constexpr(std::is_same_v<V, long>)   { Variable = std::stol(raw_data_copy.at(0)); return; }
	    if constexpr(std::is_same_v<V, float>)  { Variable = std::stof(raw_data_copy.at(0)); return; }
	    if constexpr(std::is_same_v<V, double>) { Variable = std::stod(raw_data_copy.at(0)); return; }

	    // Defaults for anything else:
	    if constexpr(std::is_integral_v<V>)       { Variable = std::stol(raw_data_copy.at(0)); return; }
	    if constexpr(std::is_floating_point_v<V>) { Variable = std::stod(raw_data_copy.at(0)); return; }

	    PRINTERR("in gSettings::getNumeric - unexpected input type!")
	}

	template<> void getRawData<bool>(const std::string& Setting, bool& Variable) const;
	template<> void getRawData<std::string>(const std::string& Setting, std::string& Variable) const;

	template<typename V> void getVariable(const std::string& Setting, V& Variable) const
	{
	    if(!cpp_reference.contains(Setting)) return;

	    if(typeid(V) == cpp_reference.at(Setting).type())
	    {
	        Variable = std::any_cast<V>(cpp_reference.at(Setting));
	        return;
	    }

	    PRINTERR("in gSettings::getVariable - variable type does not match setting variable type!")
	}

	void getActor(const std::string& Setting, int& Variable) const;
	void getDevice(const std::string& Setting, int& Variable) const;
	void getExternal(const std::string& Setting, std::string& Variable) const;

	void addRawData(const std::string& SettingName, const gRawData& Setting);
	void addVariable(const std::string& SettingName, std::any Setting);
	void addActor(const std::string& SettingName, int Setting);
	void addDevice(const std::string& SettingName, int Setting);
	void addExternal(const std::string& SettingName, const std::string& Setting);

private:
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