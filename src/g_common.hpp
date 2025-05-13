#ifndef GRAPHX_ENGINE_COMMON
#define GRAPHX_ENGINE_COMMON
#include "t_settings.hpp"
#include "graphx_namespace.hpp"
#include "r_common_fwd.hpp"
#include "g_common_fwd.hpp"
#include <glm/fwd.hpp>
#include <memory>
#include <mutex>
#include <set>

extern bool loading_new_main_theatre;

struct RenderState
{   // Used by Actors to store/send position, rotation, and scale data
	glm::vec3 render_position = glm::vec3(0.0f);
	glm::quat render_quaternion = glm::quat();
	glm::vec3 render_scale = glm::vec3(0.0f);
};

class Actor
{
public:
	bool visible = true;

	Mesh* mesh = nullptr; // replace with std::vector<Mesh *> meshes later(?)

	glm::vec3 scale = glm::vec3(1.0f);

	glm::vec3 orientation_front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 orientation_grounded_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;
	glm::vec3 world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);

	std::vector<RenderState> current_state_buffer;
	std::vector<RenderState> previous_state_buffer;

	int state_index = 0;

	graphx::gSettings settings = empty_settings;

	Actor(std::string = "Untitled Actor", Mesh* = nullptr, glm::vec3 = glm::vec3(0.0f), glm::vec3 = glm::vec3(0.0f), glm::vec3 = glm::vec3(1.0f));
	virtual ~Actor() = default;

	template<typename T> T getPosition();
	template<typename T> T getRotation();
	template<typename T> T getRotationDegrees();

	// Todo: convert these to function templates
	virtual void setGlobalPosition(glm::vec3);
	virtual void setGlobalPosition(JPH::Vec3);
	virtual void setGlobalRotation(glm::vec3);
	virtual void setGlobalRotation(glm::quat);
	virtual void setGlobalRotation(JPH::Vec3);
	virtual void setGlobalRotation(JPH::Quat);
	virtual void setLocalPosition(glm::vec3);
	virtual void setLocalPosition(JPH::Vec3);
	virtual void setLocalRotation(glm::vec3);
	virtual void setLocalRotation(glm::quat);
	virtual void setLocalRotation(JPH::Vec3);
	virtual void setLocalRotation(JPH::Quat);

	long getUID() const;
	void setUID(const long);
	std::string getName() const;
	void setName(const std::string);
	void setName(const char*);
	const graphx::gClass* getType() const;

	void highlightMe();
	void unHighlightMe();

	virtual RenderCommands getRenderCommands();
	virtual bool isPhysicsActor();
	virtual void youGotACallBack(graphx::gSettings = empty_settings); // Loads settings
	virtual void callToStage(Theatre*);
	virtual void takeABow();
	virtual void processMouse(GLFWwindow*, double, double);
	virtual void processInput(GLFWwindow*);
	virtual void processKey(GLFWwindow*, int, int, int, int);
	virtual void tick(int);
	virtual void updateStates(std::mutex&);

protected:
	glm::vec3 position_global = glm::vec3(0.0f);
	glm::vec3 position_local = glm::vec3(0.0f);
	glm::quat quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::quat local_quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	long UID = -1;
	std::string name = "Untitled Actor";
	const graphx::gClass* my_type = nullptr;
	glm::vec4 debug_highlight_color = glm::vec4(0.0f);

	virtual void updateVectors();
};

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
	Actor stage;
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
	// void actorLeave(Actor* old_actor);
	// void actorLeave(long uid);
	// void placeDevice(Device* new_device, long uid, graphx::gSettings new_settings = empty_settings);
	// void removeDevice(Device* old_device);
	// void removeDevice(long uid);

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

// extern Theatre graphx::current::theatre;
// extern std::map<int, Actor*(*)()> actor_map;

// Use with CAUTION!!
// Wants to return static_cast<T>(graphx::current::theatre.getActor(identifier)) but if that fails, returns new std::remove_pointer_t<T>.
// Useful for getting a down-casted pointer to a known object. Performs NO safety checks, so only use this if you know both the UID/Name AND the specific sub-class of the object you're getting.
/*template<typename T> T iKnowWhatActorIWant(auto identifier)
{
	if(graphx::current::theatre.getUID() == -1 || graphx::current::theatre.getActor(identifier) == nullptr)
		return new std::remove_pointer_t<T>;

	return static_cast<T>(graphx::current::theatre.getActor(identifier));
}*/

// Use with CAUTION!!
// Wants to return static_cast<T>(graphx::current::theatre.getDevice(identifier)) but if that fails, returns new std::remove_pointer_t<T>.
// Useful for getting a down-casted pointer to a known object. Performs NO safety checks, so only use this if you know both the UID/Name AND the specific sub-class of the object you're getting.
/*template<typename T> T iKnowWhatDeviceIWant(auto identifier)
{
	if(graphx::current::theatre.getUID() == -1 || graphx::current::theatre.getDevice(identifier) == nullptr)
		return new std::remove_pointer_t<T>;
	return static_cast<T>(graphx::current::theatre.getDevice(identifier));
}*/

// template<typename T> Actor *createNewActor() { return new T; }

Theatre *getCurrentTheatre(bool print_note = true);
// Environment *getCurrentEnvironment();
// GraphXPlayer *getCurrentPlayer();
#endif
