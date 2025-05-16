#ifndef GRAPHX_THEATRE
// #include "sanity_printouts.hpp"
#include "g_device.hpp"
#include "g_actor.hpp"
#include <glfw_fwd.hpp>
#include <vector>
#include <memory>
// #include <random>
#include <map>
#include <set>
#define GRAPHX_THEATRE

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
class Actor;
struct Device;
class GraphXTheatreInterpreter;
#endif

extern bool loading_new_main_theatre;

// Note about Theatres:
// I abstracted getting Actor and Device pointers to functions, because directly grabbing them from their maps
// might return null (if using []) or crash the engine (if using .at()). This crash will appear to happen for no
// reason, so abstracting to these functions lets me put a PRINTERR in them that will print a detailed warning
// message if the function can't find the Actor/Device (before returning a nullptr). This means that getting
// Actor or Device pointers won't (directly) crash the engine, but will print a warning if it returns a nullptr.

struct Theatre
{
    // TEMPORARY
    Actor* getFlashlight() const;
    // TEMPORARY

    Mesh* stage_mesh = nullptr;
    Material* stage_material = nullptr;
    Actor stage = Actor("stage");
    glm::vec3 stage_scale = glm::vec3(0.0f);
    glm::vec3 stage_position = glm::vec3(0.0f);
    glm::quat stage_quaternion = glm::quat();

    std::string name = "Untitled Theatre";
    int point_lights_count = 0;
    int spot_lights_count = 0;
    int directional_lights_count = 0;
    bool dropping_curtains = false;

    // This abomination is what a "gStringSettings" typedef actually is...
    std::vector<std::vector<std::pair<std::string, std::pair<int, std::string>>>> graphx_theatre_settings;
    std::string theatre_file_data_printout = "";

    Theatre(std::string init_name = "Untitled Theatre", long new_uid = -1);

    std::vector<long> dumpActorIDs();
    std::vector<Actor*> getTroupe();

    std::set<std::string> getMeshDataNames();
    std::set<std::string> getTextureNames();
    void probeActorsForRenderCommands();
    void loadStageSettings(graphx::gSettings stage_settings);
    void raiseCurtains();
    void dropCurtains();
    long getUID();
    void setUID(long new_uid);
    void delegateKeyInput(GLFWwindow* window, int key, int scancode, int action, int mods);
    void delegateMouseInput(GLFWwindow* window, double x_position_in, double y_position_in);
    void actorEnter(Actor* new_actor, long uid, graphx::gSettings new_settings = empty_settings);

    void createActor(const std::string& actor_type, long uid, graphx::gSettings new_settings = empty_settings);
    void createDevice(const std::string& device_type, long uid, graphx::gSettings new_settings  = empty_settings);

    Actor* getActor(long uid);
    Actor* getActor(std::string actor_name);
    Device* getDevice(long uid);
    Device* getDevice(std::string device_name);

    GraphXPlayer* getPlayer();
    Environment* getEnvironment();

private:
    std::map<long, Actor*> objects = {};
    std::map<long, Device*> devices = {};
    std::map<int, std::shared_ptr<Actor>> actor_map = {};   // NEW CODE
    std::map<int, std::shared_ptr<Device>> device_map = {}; // NEW CODE
    long UID = -1;
    long environment_uid = -1;
    long player_uid = -1;
};

Theatre *getCurrentTheatre(bool print_note = true);
#endif