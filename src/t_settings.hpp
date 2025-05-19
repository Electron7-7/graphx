#ifndef GRAPHX_SETTINGS
#include "sanity_printouts.hpp"
#include <string>
#include <map>
#include <any>
#include <vector>
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
    std::map<std::string, gRawData> raw_data = {}; // numeric, boolean, and string
    std::map<std::string, std::any> cpp_reference = {};
    std::map<std::string, Actor*> actor_reference = {};
    std::map<std::string, Device*> device_reference = {};
    std::map<std::string, std::string> external_reference = {};

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

    // void getActor(const std::string& Setting, Actor*& Variable) const;
    // void getDevice(const std::string& Setting, Device*& Variable) const;
    template<typename P> void getActor(const std::string& setting, P*& variable) const
    {
        if(!actor_reference.contains(setting)) return;
        if constexpr(std::is_base_of_v<Actor, P>)
            variable = dynamic_cast<P*>(actor_reference.at(setting));
    }

    template<typename P> void getDevice(const std::string& setting, P*& variable) const
    {
        if(!device_reference.contains(setting)) return;
        if constexpr(std::is_base_of_v<Device, P>)
            variable = dynamic_cast<P*>(actor_reference.at(setting));
    }
    void getVariable(const std::string& Setting, std::string& Variable) const;

    // Skips having to manually set all the Actor variables for every derived class
    static void configureBaseVariables(Actor* actor);
    static void configureBaseVariables(Device* actor);
};
#endif