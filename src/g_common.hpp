#ifndef GRAPHX_ENGINE_COMMON
#define GRAPHX_ENGINE_COMMON
#include "sanity.hpp" // Included for the GLM headers
#include "t_settings.hpp"
#include "graphx_namespace.hpp"
#include <mutex>

// Forward Declarations
struct Theatre; // For Actor
class GraphXPlayer;
struct Device;
struct Environment;
struct Material;
struct Mesh;

extern bool loading_new_main_theatre;

struct RenderState
{
	glm::vec3 render_position;
	glm::quat render_quaternion;
	glm::vec3 render_scale;

	RenderState(glm::vec3 init_position = glm::vec3(0.0f), glm::quat init_quaternion = glm::quat(), glm::vec3 init_scale = glm::vec3(1.0f));
};

class Actor
{
public:
	bool visible = true;

	Mesh *mesh = nullptr; // replace with std::vector<Mesh *> meshes later(?)

	glm::vec3 scale = glm::vec3(1.0f);

	glm::vec3 orientation_front = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 orientation_grounded_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;
	glm::vec3 world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);

	RenderState current_state;
	RenderState current_state_copy;

	RenderState previous_state;
	RenderState previous_state_copy;

	std::vector<RenderState> current_state_buffer;
	std::vector<RenderState> previous_state_buffer;

	int state_index = 0;

	graphx::gSettings settings = empty_settings;

	Actor(std::string new_name = "Untitled Actor", Mesh *init_mesh = nullptr, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_euler_degrees = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f));
	virtual ~Actor() = default;

	template<typename T> T getPosition();
	template<typename T> T getRotation();

	template<typename T> void setGlobalPosition(T new_value);
	template<typename T> void setGlobalRotation(T new_value);
	template<typename T> void setLocalPosition(T new_value);
	template<typename T> void setLocalRotation(T new_value);

	long getUID();
	void setUID(long manual_uid);
	bool isType(int class_type);
	bool isType(std::initializer_list<int> const &class_types);
	// I have to define this in the header file, unfortunately
	template<std::size_t array_size> bool isType(std::array<int, array_size> class_types)
	{
		for(int type : class_types)
			if(my_type == type)
				return true;
		return false;
	}
	std::string getTypeName();
	long getType();
	void setName(std::string new_name);
	void setName(char *new_name);
	std::string getName();

	virtual bool isPhysicsActor();
	virtual void youGotACallBack(graphx::gSettings new_settings = empty_settings); // Loads settings
	virtual void callToStage(Theatre *parent_theatre);
	virtual void takeABow();
	virtual void processMouse(GLFWwindow *window, double x_position_in, double y_position_in);
	virtual void processInput(GLFWwindow *window);
	virtual void processKey(GLFWwindow *window, int key, int scancode, int action, int mods);
	virtual void tick(int current_tick);
	virtual void updateStates(std::mutex &state_mutex);
	virtual bool wantsToBeRendered();

protected:
	int my_type;
	long UID = -1; // A UID of -1 means it's not been set yet
	std::string name = "Untitled Actor";
	glm::vec3 position_global = glm::vec3(0.0f);
	glm::vec3 position_local = glm::vec3(0.0f);
	glm::quat quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::quat local_quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

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
	Mesh *stage_mesh = nullptr;
	Material *stage_material = nullptr;
	Actor stage;
	glm::vec3 stage_scale = glm::vec3(0.0f);
	glm::vec3 stage_position = glm::vec3(0.0f);
	glm::quat stage_quaternion = glm::quat();

	std::string name = "Untitled Theatre";
	std::vector<Actor *> troupe = {};
	int point_lights_count = 0;
	int spot_lights_count = 0;

	Theatre(std::string init_name = "Untitled Theatre", long new_uid = -1);

	void loadStageSettings(graphx::gSettings stage_settings);
	void raiseCurtains();
	void dropCurtains();
	long getUID();
	void setUID(long new_uid);
	void delegateKeyInput(GLFWwindow *window, int key, int scancode, int action, int mods);
	void delegateMouseInput(GLFWwindow *window, double x_position_in, double y_position_in);

	std::string giveMeAPrettyListOfAllActorsOrDevices(bool show_actors);

	void refreshTroupe();
	void troupeEnter(std::vector<std::pair<Actor *, long>> new_troupe);
	void actorEnter(Actor *new_actor, long uid, graphx::gSettings new_settings = empty_settings);
	void actorLeave(Actor *old_actor);
	void actorLeave(long uid);
	void placeDevice(Device *new_device, long uid, graphx::gSettings new_settings = empty_settings);
	void removeDevice(Device *old_device);
	void removeDevice(long uid);

	void createActor(int actor_type, long uid, graphx::gSettings new_settings = empty_settings);
	void createDevice(int device_type, long uid, graphx::gSettings new_settings  = empty_settings);

	Actor *getFirstActorOfType(int type_name);
	Device *getFirstDeviceOfType(int type_name);

	// WARNING!! THIS FUNCTION WILL RETURN A nullptr IF NO ACTOR MATCHING type_name IS FOUND!!
	Actor *unsafeGetFirstActorOfType(int type_name);
	// WARNING!! THIS FUNCTION WILL RETURN A nullptr IF NO DEVICE MATCHING type_name IS FOUND!!
	Device *unsafeGetFirstDeviceOfType(int type_name);

	Actor *getActor(long uid);
	Actor *getActor(std::string actor_name);
	Device *getDevice(long uid);
	Device *getDevice(std::string device_name);
	GraphXPlayer *getPlayer();
	Environment *getEnvironment();

private:
	std::unordered_map<long, Actor *> objects = {};
	std::unordered_map<long, Device *> devices = {};
	long UID = -1;
	long environment_uid = -1;
	long player_uid = -1;

	void sortTroupe();
	void countLights();
};

extern Theatre current_theatre;
extern std::map<int, Actor*(*)()> actor_map;

// Use with CAUTION!!
// Wants to return static_cast<T>(current_theatre.getActor(identifier)) but if that fails, returns new std::remove_pointer_t<T>.
// Useful for getting a down-casted pointer to a known object. Performs NO safety checks, so only use this if you know both the UID/Name AND the specific sub-class of the object you're getting.
template<typename T> T iKnowWhatActorIWant(auto identifier)
{
	if(current_theatre.getUID() == -1 || current_theatre.getActor(identifier) == nullptr)
		return new std::remove_pointer_t<T>;

	return static_cast<T>(current_theatre.getActor(identifier));
}

// Use with CAUTION!!
// Wants to return static_cast<T>(current_theatre.getDevice(identifier)) but if that fails, returns new std::remove_pointer_t<T>.
// Useful for getting a down-casted pointer to a known object. Performs NO safety checks, so only use this if you know both the UID/Name AND the specific sub-class of the object you're getting.
template<typename T> T iKnowWhatDeviceIWant(auto identifier)
{
	if(current_theatre.getUID() == -1 || current_theatre.getDevice(identifier) == nullptr)
		return new std::remove_pointer_t<T>;
	return static_cast<T>(current_theatre.getDevice(identifier));
}

template<typename T> Actor *createNewActor()
{
	return new T;
}

Theatre *getCurrentTheatre();
Environment *getCurrentEnvironment();
GraphXPlayer *getCurrentPlayer();
#endif