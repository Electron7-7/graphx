#include <algorithm>
#ifndef GRAPHX_SETTINGS
#include "sanity_printouts.hpp"
#include <any>
#include <map>
#include <vector>
#include <filesystem> // Yes, the devil hath been invoked... I'm sorry
#include <glm/glm.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#define GRAPHX_SETTINGS

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
class Actor;
struct Device;
#endif

#define DEFAULT_VALUE_SETTING 99

#define GRAB_SETTING_ERR_DEVICE_POINTER     -4
#define GRAB_SETTING_ERR_ACTOR_POINTER      -8
#define GRAB_SETTING_ERR_RAW_DATA           -15
#define GRAB_SETTING_ERR_CPP_REFERENCE      -16
#define GRAB_SETTING_ERR_SANDWICH_BUN       -23
#define GRAB_SETTING_ERR_EXTERNAL_REFERENCE -42

#define GRAB_SETTING_ERR_MSG_TEMPLATE std::string("getSetting called with non-matching variable and setting!")

typedef std::vector<std::string> gRawData;

template<typename T> struct is_raw_data:
std::disjunction
<
	std::is_same<T, std::string>,
	std::is_same<T, bool>,
	std::is_arithmetic<T>,
	std::is_same<T, glm::vec2>,
	std::is_same<T, glm::vec3>,
	std::is_same<T, glm::vec4>,
	std::is_same<T, glm::quat>
> {};

template<typename T> struct is_theatre_reference:
std::disjunction
<
	std::is_same<T, std::shared_ptr<Actor>>,
	std::is_same<T, std::shared_ptr<Device>>
> {};

template<typename T> struct is_theatre_reference<std::shared_ptr<T>>:
std::disjunction
<
	std::is_base_of<Actor, T>,
	std::is_base_of<Device, T>
> {};

struct gSettings; // Forward Declaration

struct gSetting
{
public:
	// Defining this inline bc why not? It's never going to change, lmfao
	const bool notEmpty() const
	{ return setting.has_value(); }

protected:
	explicit gSetting(std::any = std::any());

	std::any setting;

	friend gSettings;
};

struct gSettingRawData: public gSetting
{
	gSettingRawData();
	gSettingRawData(const gRawData&);

	template<typename T> int getSetting(T& variable) const
	{
	    if(!setting.has_value()) return 0;

	    static_assert(
	        std::is_arithmetic_v<T> ||
	        std::is_same_v<T, glm::vec2> ||
	        std::is_same_v<T, glm::vec3> ||
	        std::is_same_v<T, glm::vec4> ||
	        std::is_same_v<T, glm::quat>
	    );

	    if constexpr(!std::is_arithmetic_v<T>) // Assumes the vector is the same size (or lower) as the variable!
	    {
	        std::vector<std::string> raw_data_copy = std::any_cast<gRawData>(setting);
	        for(int i = 0; i < raw_data_copy.size(); i++)
	            variable[i] = stringToNumber<float>(raw_data_copy[i]);
	        return 0;
	    }

	    else if constexpr(std::is_arithmetic_v<T>)
	    {
	        variable = stringToNumber<T>(std::any_cast<gRawData>(setting)[0]);
	        return 0;
	    }

	    PRINTERR("in gSettingRawData::getSetting: type is expected to be arithmetic or GLM vector/quaternion!")
	    return GRAB_SETTING_ERR_RAW_DATA;
	}

	template<> int getSetting(std::string& variable) const
	{
	    if(!setting.has_value()) return 0;

	    std::string string_out = "";

	    for(std::string setting_string : std::any_cast<gRawData>(setting))
	        string_out.append(setting_string);

	    variable = string_out;
	    return 0;
	}

	template<> int getSetting(bool& variable) const
	{
	    if(!setting.has_value()) return 0;

	    std::string raw_data_lower = std::any_cast<gRawData>(setting)[0];
	    std::transform(raw_data_lower.begin(), raw_data_lower.end(), raw_data_lower.begin(), [](unsigned char c)
	    {
	        return std::tolower(c);
	    });

	    variable = raw_data_lower.compare("false");
	    return 0;
	}

private:
	template<typename T> T stringToNumber(const std::string& str) const
	{   // There's probably a better way to do this, but fuck it idc
        T valid_number = 0;

	    if constexpr(std::is_integral_v<T>)
	    {
	        try
	        {
	            valid_number = std::stol(str);
	        }

	        catch(std::invalid_argument const& exception)
	        {
	            PRINTERR("in gSetting::stringToNumber at std::stol: std::invalid_argument " << exception.what())
	            valid_number = 0; // Just to be safe...
	        }

	        return valid_number;
	    }

	    else
	    {
	        try
	        {
	            valid_number = std::stod(str);
	        }

	        catch(std::invalid_argument const& exception)
	        {
	            PRINTERR("in gSetting::stringToNumber at std::stod: std::invalid_argument " << exception.what())
	            valid_number = 0; // Just to be safe...
	        }

	        return valid_number;
	    }
	}
};

struct gSettingCppReference: public gSetting
{
	gSettingCppReference();
	gSettingCppReference(const std::string&);

	template<typename T>
	int getSetting(T& variable) const
	{
	    if(!setting.has_value()) return 0;

        if(setting.type() == typeid(T))
        { variable = std::any_cast<T>(setting); return 0; }

        PRINTERR("in gSettingCppReference::getSetting: type does not match setting type!")
        return GRAB_SETTING_ERR_CPP_REFERENCE;
	}
};

struct gSettingTheatreReference: public gSetting
{
	gSettingTheatreReference();
	gSettingTheatreReference(std::shared_ptr<Actor>, const bool = false);
	gSettingTheatreReference(std::shared_ptr<Device>, const bool = false);

	// Again, this'll never change so I'm just gonna define it in the header
	const bool isSandwich() const
	{ return is_sandwich; }

	template<typename T> int getSetting(std::shared_ptr<T>& variable) const
	{
	    if(!setting.has_value()) return 0;

		if constexpr(std::is_same_v<T, Actor>)
	    { variable = std::static_pointer_cast<Actor>(std::any_cast<std::shared_ptr<Actor>>(setting)); return 0; }

		else if constexpr(std::derived_from<T, Actor>)
		{ variable = std::static_pointer_cast<T>(std::any_cast<std::shared_ptr<Actor>>(setting)); return 0; }

		if constexpr(std::is_same_v<T, Device>)
		{ variable = std::static_pointer_cast<Device>(std::any_cast<std::shared_ptr<Device>>(setting)); return 0; }

		else if constexpr(std::derived_from<T, Device>)
		{ variable = std::static_pointer_cast<T>(std::any_cast<std::shared_ptr<Device>>(setting)); return 0; }

	    PRINTERR("in gSettingTheatreReference::getSetting: type is not derived from Actor or Device!")
	    return GRAB_SETTING_ERR_ACTOR_POINTER;
	}

private:
	bool is_sandwich = false; // Probably not needed
};

struct gSettingExternalReference: public gSetting
{
	gSettingExternalReference();
	gSettingExternalReference(const std::filesystem::path&);
	gSettingExternalReference(const std::string&);

	template<typename T> int getSetting(T& variable) const
	{
	    if(!setting.has_value()) return 0;

	    if(setting.type() == typeid(std::string))
	    {
	        variable = std::any_cast<std::string>(setting);
	        return 0;
	    }

	    else if(setting.type() == typeid(std::filesystem::path))
	    {
	        variable = std::any_cast<std::filesystem::path>(setting);
	        return 0;
	    }

	    PRINTERR("in gSettingExternalReference::getSetting: setting was not of type std::string or std::filesystem::path")
	    return GRAB_SETTING_ERR_EXTERNAL_REFERENCE;
	}
};

struct gSettings
{
	void addSetting(const std::string& Name, const gSettingRawData& 	 	  Setting);
	void addSetting(const std::string& Name, const gSettingCppReference& 	  Setting);
	void addSetting(const std::string& Name, const gSettingTheatreReference&  Setting);
	void addSetting(const std::string& Name, const gSettingExternalReference& Setting);

	template<typename V> void getSetting(const std::string& SettingName, V& Variable)
	{
		if constexpr(is_raw_data<V>::value)
		{ raw_data[SettingName].getSetting(Variable); return; }

		else
		{
			if(external_references.contains(SettingName))
			{
				if constexpr(std::is_same_v<V, std::string>)
				{ external_references[SettingName].getSetting(Variable); return; }
			}

			else
			{ cpp_references[SettingName].getSetting(Variable); return; }
		}
	}

	template<typename V> void getSetting(const std::string& SettingName, std::shared_ptr<V>& Variable)
	{
		if(theatre_references.contains(SettingName))
		{ theatre_references[SettingName].getSetting(Variable); return; }
	}


private:
	std::map<std::string, gSettingRawData>           raw_data;
	std::map<std::string, gSettingCppReference>      cpp_references;
	std::map<std::string, gSettingTheatreReference>  theatre_references;
	std::map<std::string, gSettingExternalReference> external_references;
};
#endif