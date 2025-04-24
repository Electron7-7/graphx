#ifndef GRAPHX_THEATRE
#include "g_common_fwd.hpp"
#include "r_common_fwd.hpp"
#include "graphx_namespace.hpp"
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
    Theatre(const graphx::gUID);
    Theatre(const int, const std::string = "Untitled Theatre");
    ~Theatre();

    const graphx::gUID getUID() const;
    void probeRenderCommands() const;
    void delegateKeyInput(GLFWwindow*, const int, const int) const;
    void delegateMouseInput(GLFWwindow*, const double, const double) const;
    const LightsCount getLightsCount() const;

    //---------------------------------------
    // Start of Actor/Device Helper Functions
    //---------------------------------------
    void createActorOrDevice(const graphx::gClass*, const graphx::gUID&, const graphx::gSettings&);

    void addActor(Actor*);
    void addDevice(Device*);

    // Safe and reliable, since every Actor must have a unique UID
    Actor* getActor(const graphx::gUID) const;
    // Safe and reliable, since every Device must have a unique UID
    Device* getDevice(const graphx::gUID) const;

    // Safe, but unreliable; if multiple Actors share the same name, this returns the first Actor it encounters
    Actor* getActor(const std::string) const;
    // Safe, but unreliable; if multiple Devices share the same name, this returns the first Device it encounters
    Device* getDevice(const std::string) const;

    void getActors(std::vector<Actor*>&) const;
    void getDevices(std::vector<Device*>&) const;

    void getActorsOfType(const graphx::gClass*, std::vector<Actor*>&) const;
    void getDevicesOfType(const graphx::gClass*, std::vector<Device*>&) const;

    void removeActor(const graphx::gUID);
    void removeDevice(const graphx::gUID);
    //-------------------------------------
    // End of Actor/Device Helper Functions
    //-------------------------------------

private:
    graphx::gUID uid = graphx::gUID(-1, "Untitled Theatre");
    std::map<graphx::gUID, Actor*> actors;
    std::map<graphx::gUID, Device*> devices;
};

#define THEATRE_ERR_DUPLICATE_UID(function, type, uid) std::string(function) + " - " + std::string(type) + " with the UID " + uid +  " already exists!"
#define THEATRE_ERR_WRONG_BASE_TYPE(function, expected, received) std::string(function) + " - expected a class of type \"" + std::string(expected) + "\" but received a class of type \"" + std::string(received) + "\" instead!"
#define THEATRE_ERR_INVALID_UID(function, type, uid) std::string(function) + " - no " + std::string(type) + " with the UID " + uid + " was found! Returning \"new " + std::string(type) + "()\" in order to avoid undefined behaviour; this is a memory leak, and should be fucking avoided like the plague!"
#endif