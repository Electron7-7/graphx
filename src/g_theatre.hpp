#ifndef GRAPHX_THEATRE
#include "graphx_namespace.hpp"
#include "g_actor.hpp"
#include "g_device.hpp"
#include <glfw_fwd.hpp>
#include <vector>
#include <map>
#define GRAPHX_THEATRE

// Note about Theatres:
// I abstracted getting Actor and Device pointers to functions, because directly grabbing them from their maps
// might return null (if using []) or crash the engine (if using .at()). This crash will appear to happen for no
// reason, so abstracting to these functions lets me put a PRINTERR in them that will print a detailed warning
// message if the function can't find the Actor/Device (before returning a nullptr). This means that getting
// Actor or Device pointers won't (directly) crash the engine, but will print a warning if it returns a nullptr.

struct LightsCount
{
    const unsigned int point_lights;
    const unsigned int spot_lights;
    const unsigned int directional_lights;

    LightsCount(const unsigned int, const unsigned int, const unsigned int);
};

struct Theatre
{
    std::string name = "Untitled Theatre";

    // The constructor that should be used 99% of the time
    Theatre(const std::string& Name = "Untitled Theatre");

    // The constructor that the interpreter uses when creating Theatres
    Theatre(const int, const std::string&);
    ~Theatre();

    int getUID() const;
    void probeRenderCommands() const;
    void delegateKeyInput(GLFWwindow*, const int, const int, const int, const int) const;
    void delegateMouseInput(GLFWwindow*, const double, const double) const;
    const LightsCount getLightsCount() const;

    //---------------------------------------
    // Start of Actor/Device Helper Functions
    //---------------------------------------
    void addActor(Actor*);
    void addDevice(Device*);

    // Safe and reliable, since every Actor must have a unique UID
    Actor* getActor(const int UID) const;
    // Safe and reliable, since every Device must have a unique UID
    Device* getDevice(const int UID) const;

    // Safe, but unreliable; if multiple Actors share the same name, this returns the first Actor it encounters
    Actor* getActor(const std::string&) const;
    // Safe, but unreliable; if multiple Devices share the same name, this returns the first Device it encounters
    Device* getDevice(const std::string&) const;

    void getActors(std::vector<ActorPointerWrapper>&) const;
    void getDevices(std::vector<DevicePointerWrapper>&) const;

    void removeActor(const int);
    void removeDevice(const int);
    //-------------------------------------
    // End of Actor/Device Helper Functions
    //-------------------------------------

private:
    int UID = -1;
    std::map<int, ActorPointerWrapper> actors;
    std::map<int, DevicePointerWrapper> devices;

    int setActorUID(const int);
    int setDeviceUID(const int);

    friend void Actor::setUID(const int);
    friend void Device::setUID(const int);

    void addInterpretedActor(Actor*);
    void addInterpretedDevice(Device*);
};

#define THEATRE_ERR_DUPLICATE_UID(function, type, uid) std::string(function) + " - " + std::string(type) + " with the UID " + uid +  " already exists!"
#define THEATRE_ERR_WRONG_BASE_TYPE(function, expected, received) std::string(function) + " - expected a class of type \"" + std::string(expected) + "\" but received a class of type \"" + std::string(received) + "\" instead!"
#define THEATRE_ERR_INVALID_UID(function, type, uid) std::string(function) + " - no " + std::string(type) + " with the UID " + uid + " was found! Returning \"&graphx::safety::" + std::string(static_cast<char>(std::tolower(std::string(type).at(0))) + std::string(type).substr(1)) + "\"!"
#endif