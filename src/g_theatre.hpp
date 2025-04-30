#include <memory>
#ifndef GRAPHX_THEATRE
#include <glfw_fwd.hpp>
#include <vector>
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
#define THEATRE_ERR_INVALID_UID(function, type, uid) std::string(function) + " - no " + std::string(type) + " with the UID " + std::to_string(uid) + " was found! Returning \"&graphx::safety::" + std::string(static_cast<char>(std::tolower(std::string(type).at(0))) + std::string(type).substr(1)) + "\"!"
#define THEATRE_ERR_INVALID_NAME(function, type, name) std::string(function) + " - no " + std::string(type) + " with the Name " + std::string(name) + " was found! Returning \"&graphx::safety::" + std::string(static_cast<char>(std::tolower(std::string(type).at(0))) + std::string(type).substr(1)) + "\"!"
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
    void probeRenderCommands();
    void delegateKeyInput(GLFWwindow*, const int, const int, const int, const int) const;
    void delegateMouseInput(GLFWwindow*, const double, const double) const;
    const LightsCount getLightsCount() const;

    void addActor(std::shared_ptr<Actor> Actor);
    void addDevice(std::shared_ptr<Device> Device);

    std::vector<std::shared_ptr<Actor>> getAllActors() const;
    std::vector<std::shared_ptr<Device>> getAllDevices() const;

    // Safe and reliable, since every Actor must have a unique UID
    std::shared_ptr<Actor> getActor(const int UniqueID) const;
    // Safe and reliable, since every Device must have a unique UID
    std::shared_ptr<Device> getDevice(const int UniqueID) const;

    // Safe, but unreliable; if multiple Actors share the same name, this returns the first Actor it encounters
    std::shared_ptr<Actor> getActor(const std::string& Name) const;
    // Safe, but unreliable; if multiple Devices share the same name, this returns the first Device it encounters
    std::shared_ptr<Device> getDevice(const std::string& Name) const;

    void removeActor(const int UniqueID);
    void removeDevice(const int UniqueID);

    void checkAndSetCurrentVariables(std::shared_ptr<Actor> = nullptr, std::shared_ptr<Device> = nullptr);
    //-------------------------------------
    // End of Actor/Device Helper Functions
    //-------------------------------------

private:
    int UID = -1;
    std::map<int, std::shared_ptr<Actor>> actor_map;
    std::map<int, std::shared_ptr<Device>> device_map;
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

    std::shared_ptr<Actor> addInterpretedActor(std::shared_ptr<Actor>);
    std::shared_ptr<Device> addInterpretedDevice(std::shared_ptr<Device>);

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