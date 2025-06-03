#ifndef GRAPHX_SETTINGS
#include "sanity_printouts.hpp"
#include "graphx_namespace.hpp"
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

struct ActorReference
{
    int uid = -1;
    std::string name = "";
    Theatre* parent_theatre = graphx::TheatreHandler.getCurrentTheatre();

    ActorReference() = default;
    ActorReference(const int UID, Theatre* ParentTheatre = graphx::TheatreHandler.getCurrentTheatre());
    ActorReference(const std::string& Name, Theatre* ParentTheatre = graphx::TheatreHandler.getCurrentTheatre());

    bool isValid() const;
    Actor* getPointer() const;
};

struct DeviceReference
{
    int uid = -1;
    std::string name = "";
    Theatre* parent_theatre = nullptr;

    DeviceReference() = default;
    DeviceReference(const int UID, Theatre* ParentTheatre = graphx::TheatreHandler.getCurrentTheatre());
    DeviceReference(const std::string& Name, Theatre* ParentTheatre = graphx::TheatreHandler.getCurrentTheatre());

    bool isValid() const;
    Device* getPointer() const;
};

struct gSettings
{
public:
    std::map<std::string, gRawData> raw_data = {}; // numeric, boolean, and string
    std::map<std::string, std::any> cpp_reference = {};
    std::map<std::string, ActorReference> actor_reference = {};
    std::map<std::string, DeviceReference> device_reference = {};
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

    template<typename P> void getActor(const std::string& setting, P*& variable) const
    {
        if(!actor_reference.contains(setting) || !actor_reference.at(setting).isValid()) return;
        if constexpr(std::is_base_of_v<Actor, P>)
            variable = dynamic_cast<P*>(actor_reference.at(setting).getPointer());
    }

    template<typename P> void getDevice(const std::string& setting, P*& variable) const
    {
        if(!device_reference.contains(setting) || !device_reference.at(setting).isValid()) return;
        if constexpr(std::is_base_of_v<Device, P>)
            variable = dynamic_cast<P*>(device_reference.at(setting).getPointer());
    }
    void getVariable(const std::string& Setting, std::string& Variable) const;

    // Skips having to manually set all the Actor variables for every derived class
    static void configureBaseVariables(Actor* actor);
    static void configureBaseVariables(Device* actor);
};
#endif