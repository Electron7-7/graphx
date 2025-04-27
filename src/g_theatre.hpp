#ifndef GRAPHX_THEATRE
#include "t_interpreter.hpp"
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

    void addActor(Actor* Actor);
    void addDevice(Device* Device);

    std::vector<Actor*> getAllActors() const;
    std::vector<Device*> getAllDevices() const;

    // Safe and reliable, since every Actor must have a unique UID
    Actor* getActor(const int UniqueID) const;
    // Safe and reliable, since every Device must have a unique UID
    Device* getDevice(const int UniqueID) const;

    // Safe, but unreliable; if multiple Actors share the same name, this returns the first Actor it encounters
    Actor* getActor(const std::string& Name) const;
    // Safe, but unreliable; if multiple Devices share the same name, this returns the first Device it encounters
    Device* getDevice(const std::string& Name) const;

    void removeActor(const int UniqueID);
    void removeDevice(const int UniqueID);
    //-------------------------------------
    // End of Actor/Device Helper Functions
    //-------------------------------------

private:
    int UID = -1;
    std::map<int, ActorPointerWrapper> wrapped_actors;
    std::map<int, DevicePointerWrapper> wrapped_devices;
    std::vector<Actor*> unwrapped_actors;
    std::vector<Device*> unwrapped_devices;

    int setActorUID(Actor*, const int);
    int setDeviceUID(Device*, const int);

    void addInterpretedActor(Actor*);
    void addInterpretedDevice(Device*);

    void parallelAddActor(Actor*, const int, const bool);
    void parallelAddDevice(Device*, const int, const bool);

    void parallelRemoveActor(const int);
    void parallelRemoveDevice(const int);

    void checkAndManageParallelActorDesync();
    void checkAndManageParallelDeviceDesync();

    friend void Actor::setUID(const int);
    friend void Device::setUID(const int);
    friend void GraphXTheatreInterpreter::loadTheatre(const long, Theatre&);
};

#define THEATRE_ERR_DUPLICATE_UID(function, type, uid) std::string(function) + " - " + std::string(type) + " with the UID " + uid +  " already exists!"
#define THEATRE_ERR_INVALID_UID(function, type, uid) std::string(function) + " - no " + std::string(type) + " with the UID " + uid + " was found! Returning \"&graphx::safety::" + std::string(static_cast<char>(std::tolower(std::string(type).at(0))) + std::string(type).substr(1)) + "\"!"
#define THEATRE_ERR_PARALLEL_DESYNC(function, type) std::string(function) + " - a desync between the " << std::string(type) << " map and vector has been detected! To maintain synchronization, both the map and the vector will be compared to locate and remove the extra " << std::string(type)
#define THEATRE_ERR_DESYNC_DETECTION(type, location, uid) "Extraneous " << std::string(type) << " detected in " << std::string(location) << " with UID: " << std::to_string(uid) << " will be deleted"
#endif