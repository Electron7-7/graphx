#ifndef GRAPHX_ENGINE_COMMON
#define GRAPHX_ENGINE_COMMON
#include "r_common.hpp"
// #include <unordered_map>
// #include <any>
// #include "g_math.hpp"

extern Theatre *current_theatre_deprecated;
extern bool current_troupe_changed;

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
	long UID;
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

	Actor(std::string new_name = "Untitled Actor", Mesh *init_mesh = NULL, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_euler_degrees = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f));

	virtual void youGotACallBack(std::string new_name, bool is_visible, Mesh *new_mesh, glm::vec3 new_position, glm::quat new_quaternion, glm::vec3 new_scale); // Theatre file calls this!
	virtual void youGotACallBack(std::string new_name, bool is_visible, Mesh *new_mesh, glm::vec3 new_position, glm::vec3 new_rotation_degrees, glm::vec3 new_scale); // Theatre file calls this!

	virtual void tick(int current_tick);
	virtual void callToStage(Theatre *parent_theatre);
	virtual void takeABow();
	virtual void updateStates(std::mutex &state_mutex);
	virtual bool wantsToBeRendered();
	virtual bool wantsToBeBuffered();

protected:
	bool debug_visible;
	virtual void updateVectors();
};

struct Theatre
{
	Mesh stage = Mesh();
	std::string name = "Untitled Theatre";
	std::unordered_map<int, std::vector<Actor *>> troupe = {};
	std::unordered_map<int, std::vector<Material *>> materials = {};
	std::unordered_map<int, std::vector<Mesh *>> meshes = {};

	int point_lights_count = 0;
	int spot_lights_count = 0;

	Theatre(std::string init_name = "Untitled Theatre");

	void startPreshow();
	void dropCurtains();
	void actorEnter(Actor *new_actor);
	void troupeEnter(std::vector<Actor *> new_troupe);
	void actorLeave(Actor *old_actor);

private:
	void sortTroupe();
	void countLights();
};
#endif