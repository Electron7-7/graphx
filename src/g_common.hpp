#ifndef GRAPHX_ENGINE_COMMON
#define GRAPHX_ENGINE_COMMON
#include "graphx_namespace.hpp"
#include "r_common.hpp"

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
	unsigned int actor_type; // Slowly phase this out via visitor system

	std::string name = "Untitled Actor";
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

	graphx::gSettings settings;

	Actor(std::string new_name = "Untitled Actor", Mesh *init_mesh = new Mesh(), glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_euler_degrees = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f));
	virtual ~Actor();

	template<typename T> T getPosition();
	template<typename T> T getRotation();

	template<typename T> void setGlobalPosition(T new_value);
	template<typename T> void setGlobalRotation(T new_value);
	template<typename T> void setLocalPosition(T new_value);
	template<typename T> void setLocalRotation(T new_value);

	long getUID();
	void setUID(long manual_uid);
	bool isType(int class_type);

	virtual void youGotACallBack(graphx::gSettings new_settings = {{"FUCKYOU", {}}}); // Loads settings
	virtual void callToStage(Theatre *parent_theatre);
	virtual void takeABow();
	virtual void tick(int current_tick);
	virtual void updateStates(std::mutex &state_mutex);
	virtual bool wantsToBeRendered();
	virtual bool wantsToBeBuffered();

protected:
	long UID = -1; // A UID of -1 means it's not been set yet
	int my_type = graphx::classes::ACTOR;
	bool debug_visible;
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
	Mesh *stage = new Mesh(new Material(false, glm::vec3(0.5, 0.1, 0.4)));
	std::string name = "Untitled Theatre";
	std::vector<Actor *> troupe = {};
	int point_lights_count = 0;
	int spot_lights_count = 0;

	Theatre(std::string init_name = "Untitled Theatre", long new_uid = -1);
	~Theatre();

	void startPreshow();
	void dropCurtains();
	long getUID();
	void setUID(long new_uid);

	void troupeEnter(std::vector<std::pair<Actor *, long>> new_troupe);
	void actorEnter(Actor *new_actor, long uid, graphx::gSettings new_settings = {{"IDONTUNDERSTANDTHEQUESTIONANDIWONTRESPONDTOIT", {}}});
	void actorLeave(Actor *old_actor);
	void actorLeave(long uid);
	void placeDevice(Device *new_device, long uid, graphx::gSettings new_settings = {{"IDONTUNDERSTANDTHEQUESTIONANDIWONTRESPONDTOIT", {}}});
	void removeDevice(Device *old_device);
	void removeDevice(long uid);

	void createActor(int actor_type, long uid, graphx::gSettings new_settings);
	void createDevice(int device_type, long uid, graphx::gSettings new_settings);
	
	Actor *getActor(long uid);
	Actor *getActor(std::string actor_name);
	Device *getDevice(long uid);
	Device *getDevice(std::string device_name);
	GraphXPlayer *getPlayer();
	Environment *getEnvironment();

	// Use with CAUTION!!
	// Returns static_cast<T>(getActor(identifier)).
	// Useful for getting a down-casted pointer to a known object. Performs NO safety checks, so only use this if you know both the UID/Name AND the specific sub-class of the object you're getting.
	template<typename T> T iKnowWhatActorIWant(auto identifier)
	{
		if(getActor(identifier) == nullptr)
			return new std::remove_pointer_t<T>;
		return static_cast<T>(getActor(identifier));
	}

	// Use with CAUTION!!
	// Returns static_cast<T>(getDevice(identifier)).
	// Useful for getting a down-casted pointer to a known object. Performs NO safety checks, so only use this if you know both the UID/Name AND the specific sub-class of the object you're getting.
	template<typename T> T iKnowWhatDeviceIWant(auto identifier)
	{
		if(getDevice(identifier) == nullptr)
			return new std::remove_pointer_t<T>;
		return static_cast<T>(getDevice(identifier));
	}

private:
	std::unordered_map<long, Actor *> objects = {};
	std::unordered_map<long, Device *> devices = {};
	long UID = -1;
	long environment_uid = -1;
	long player_uid = -1;

	void sortTroupe();
	void countLights();
};

// extern std::map<long, Theatre> all_theatres;
// extern long current_theatre_uid;
extern Theatre current_theatre;
extern bool current_troupe_changed; // Convert this into a function/variable inside Theatre

Theatre *getCurrentTheatre(); // Abstracts Theatre acquisition to avoid bad shit like "&all_theatres[int]"
Environment *getCurrentEnvironment();
GraphXPlayer *getCurrentPlayer();
#endif