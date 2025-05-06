#ifndef GRAPHX_THEATRE
#include "sanity_printouts.hpp"
#include "g_device.hpp"
#include "g_actor.hpp"
#include <glfw_fwd.hpp>
#include <vector>
#include <memory>
#include <random>
#include <map>
#include <set>
#define GRAPHX_THEATRE

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
class Actor;
struct Device;
class GraphXTheatreInterpreter;
#endif

// Note about Theatres:
// I abstracted getting Actor and Device pointers to functions, because directly grabbing them from their maps
// might return null (if using []) or crash the engine (if using .at()). This crash will appear to happen for no
// reason, so abstracting to these functions lets me put a PRINTERR in them that will print a detailed warning
// message if the function can't find the Actor/Device (before returning a nullptr). This means that getting
// Actor or Device pointers won't (directly) crash the engine, but will print a warning if it returns a nullptr.

// Error message macros
#define THEATRE_ERR_DUPLICATE_UID(function, type, uid) std::string(function) + " - " + std::string(type) + " with the UID " + std::to_string(uid) +  " already exists!"
#define THEATRE_ERR_INVALID_UID(function, type, uid) std::string(function) + " - no " + std::string(type) + " with the UID " + std::to_string(uid) + " was found! Returning empty " + std::string(type)
#define THEATRE_ERR_INVALID_NAME(function, type, name) std::string(function) + " - no " + std::string(type) + " with the Name " + std::string(name) + " was found! Returning empty " + std::string(type)
#define THEATRE_ERR_PARALLEL_DESYNC(function, type) std::string(function) + " - a desync between the " << std::string(type) << " map and vector has been detected! To maintain synchronization, both the map and the vector will be compared to locate and remove the extra " << std::string(type)
#define THEATRE_ERR_DESYNC_DETECTION(type, location, uid) "Extraneous " << std::string(type) << " detected in " << std::string(location) << " with UID: " << std::to_string(uid) << " will be deleted"

struct LightsCount
{
    const unsigned int point_lights;
    const unsigned int spot_lights;
    const unsigned int directional_lights;

    LightsCount(const unsigned int, const unsigned int, const unsigned int);
};

struct Theatre
{
    // Shitty functions that have GOT to GO
    std::set<std::string> getTextureNames();
    std::set<std::string> getMeshDataNames();


    std::string name = "Untitled Theatre";

    // The constructor that should be used 99% of the time
    Theatre(const std::string& Name = "Untitled Theatre");

    // The constructor that the interpreter uses when creating Theatres
    Theatre(const int, const std::string&);

    int getUID() const;
    void setUID(int NewUID);
    void probeRenderCommands();
    void delegateKeyInput(GLFWwindow*, const int, const int, const int, const int) const;
    void delegateMouseInput(GLFWwindow*, const double, const double) const;
    const LightsCount getLightsCount() const;

    int addActor(std::shared_ptr<Actor> Actor);
    int addDevice(std::shared_ptr<Device> Device);

    std::vector<std::shared_ptr<Actor>> getAllActors() const;
    std::vector<std::shared_ptr<Device>> getAllDevices() const;

    // Safe and reliable, since every Actor/Device must have a unique UID
    template<typename T> std::shared_ptr<T> getActor(const int UniqueID) const
    {
        if(actor_map.contains(UniqueID))
            return std::dynamic_pointer_cast<T>(actor_map.at(UniqueID));

        PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getActor", "Actor", UniqueID))
        return std::make_shared<T>("Safety Actor");
    }

    template<typename T> std::shared_ptr<T> getDevice(const int UniqueID) const
    {
        if(device_map.contains(UniqueID))
            return std::dynamic_pointer_cast<T>(device_map.at(UniqueID));

        PRINTERR(THEATRE_ERR_INVALID_UID("Theatre::getDevice", "Device", UniqueID))
        return std::make_shared<T>("Safety Device");
    }

    // Safe, but unreliable; if multiple Actors/Devices share the same name, this returns the first one it sees
    template<typename T> std::shared_ptr<T> getActor(const std::string& Name) const
    {
        for(int i = 0; i < actor_vector.size(); i++)
            if(!actor_vector.at(i)->name.compare(Name))
                return std::dynamic_pointer_cast<T>(actor_vector.at(i));

        PRINTERR(THEATRE_ERR_INVALID_NAME("Theatre::getActor", "Actor", Name))
        return std::make_shared<T>("Safety Actor");
    }

    template<typename T> std::shared_ptr<T> getDevice(const std::string& Name) const
    {
        for(int i = 0; i < device_vector.size(); i++)
            if(!device_vector.at(i)->name.compare(Name))
                return std::dynamic_pointer_cast<T>(device_vector.at(i));

        PRINTERR(THEATRE_ERR_INVALID_NAME("Theatre::getDevice", "Device", Name))
        return std::make_shared<T>("Safety Device");
    }

    int getActorUID(const std::string& Name) const;
    int getDeviceUID(const std::string& Name) const;

    void removeActor(const int UniqueID);
    void removeDevice(const int UniqueID);

    void checkAndSetCurrentVariables(std::shared_ptr<Actor> = nullptr, std::shared_ptr<Device> = nullptr);
    //-------------------------------------
    // End of Actor/Device Helper Functions
    //-------------------------------------

private:
    int theatre_uid = -1;
    std::map<unsigned int, std::shared_ptr<Actor>> actor_map;
    std::map<unsigned int, std::shared_ptr<Device>> device_map;
    std::vector<std::shared_ptr<Actor>> actor_vector;
    std::vector<std::shared_ptr<Device>> device_vector;

    static std::random_device uid_random_device;
    static std::mt19937 uid_random_generator;

    int point_lights_count = 0;
    int spot_lights_count = 0;
    int directional_lights_count = 0;

    int generateUID(const bool);

    int changeActorUID(const int, const int);
    int changeDeviceUID(const int, const int);

    void addInterpretedActor(std::shared_ptr<Actor>);
    void addInterpretedDevice(std::shared_ptr<Device>);

    void parallelAddActor(std::shared_ptr<Actor>, const int, const bool);
    void parallelAddDevice(std::shared_ptr<Device>, const int, const bool);

    void parallelRemoveActor(const int);
    void parallelRemoveDevice(const int);

    void checkAndManageParallelActorDesync();
    void checkAndManageParallelDeviceDesync();

    friend GraphXTheatreInterpreter;
    friend Actor;
    friend Device;
};

namespace graphx
{
    namespace current
    {
        extern Theatre theatre;
    }
}
#endif