#ifndef GRAPHX_ENGINE_COMMON
#define GRAPHX_ENGINE_COMMON
#include "r_common.hpp"
#include <unordered_map>
#include <any>

typedef std::unordered_map<std::string, std::any> gSettings;
extern gSettings null_settings;

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

	Mesh *mesh = NULL; // replace with std::vector<Mesh *> meshes later(?)

	glm::vec3 position_global = glm::vec3(0.0f);
	glm::quat quaternion = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
	glm::vec3 scale = glm::vec3(1.0f);

	glm::vec3 orientation_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;
	glm::vec3 world_orientation_up;

	RenderState current_state;
	RenderState current_state_copy;

	RenderState previous_state;
	RenderState previous_state_copy;

	std::vector<RenderState> current_state_buffer;
	std::vector<RenderState> previous_state_buffer;

	int state_index = 0;

	gSettings settings;

	Actor(std::string new_name = "Untitled Actor", Mesh *init_mesh = NULL, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_euler_degrees = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f));

	virtual void youGotACallBack(gSettings new_settings = null_settings); // Loads settings
	virtual void tick(int current_tick);
	virtual void callToStage(Theatre *parent_theatre);
	virtual void takeABow();
	virtual void updateStates(std::mutex &state_mutex);
	virtual bool wantsToBeRendered();
	virtual bool wantsToBeBuffered();
	virtual long getUID();
	virtual void setUID(long manual_uid = -1); // If manual_uid is -1, a new UID is generated instead

protected:
	long UID = -1; // A UID of -1 means it's not been set yet
	bool debug_visible;
	virtual void updateVectors();
};

struct Theatre
{
	Mesh stage = Mesh();
	long environment_uid = -1;
	std::string name = "Untitled Theatre";
	std::unordered_map<long, Actor *> objects = {};
	std::unordered_map<long, Device *> devices = {};
	std::vector<Actor *> troupe = {};
	int point_lights_count = 0;
	int spot_lights_count = 0;

	Theatre(std::string init_name = "Untitled Theatre");

	void startPreshow();
	void dropCurtains();
	void actorEnter(Actor *new_actor);
	void troupeEnter(std::vector<Actor *> new_troupe);
	void actorLeave(Actor *old_actor);

private:
	void addActor(Actor *new_actor);
	void removeActor(Actor *old_actor);
	void sortTroupe();
	void countLights();
};

Theatre *getCurrentTheatre(); // Abstracts "&all_theatres[current_theatre_uid]"

extern std::unordered_map<int, Theatre *> all_theatres;
extern int current_theatre_uid;
extern bool current_troupe_changed;

// extern Theatre *current_theatre_deprecated;
#endif