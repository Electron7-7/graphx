#ifndef GRAPHX_ENGINE_COMMON
#define GRAPHX_ENGINE_COMMON
#include "r_common.hpp"
// #include "g_math.hpp"

#define ACTOR_ACTOR 		0
#define ACTOR_TOOL  		1
#define ACTOR_LIGHT 		2
#define ACTOR_PHYSICS		3
#define ACTOR_PLAYER		4

extern std::unordered_map<double, Actor *> actor_uid_lookup;
extern glm::vec3 vector3_up;
extern glm::vec3 vector3_front;
extern glm::vec3 vector3_right;

struct RenderState
{
	glm::vec3 render_position;
	glm::quat render_quaternion;
	glm::vec3 render_scale;

	RenderState(glm::vec3 init_position = glm::vec3(0.0f), glm::quat init_quaternion = glm::quat(), glm::vec3 init_scale = glm::vec3(1.0f))
	: render_position(init_position), render_quaternion(init_quaternion), render_scale(init_scale)
	{}
};

class Actor
{
public:
	long UID;

	unsigned int actor_type;
	std::string name;
	bool visible = true;

	Mesh *mesh; // std::vector<Mesh *> meshes;

	glm::vec3 position_global;
	glm::quat quaternion;
	glm::vec3 scale;
	glm::vec3 velocity;

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

	Actor(std::string new_name = "Untitled Actor", Mesh *init_mesh = NULL, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: UID(actor_uid_lookup.size()), mesh(init_mesh), position_global(init_position), scale(init_scale), orientation_front(glm::vec3(0.0f, 0.0f, -1.0f))
	{
		actor_type = ACTOR_ACTOR;
		name = new_name;
		// actor_uid_lookup.insert(actor_uid_lookup.end(), std::pair<double, Actor *>{UID, this});
		world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);
		quaternion = glm::quat(init_rotation_euler);
		current_state = RenderState(init_position, quaternion, init_scale);
		current_state_copy = current_state;
		previous_state = current_state;
		previous_state_copy = current_state;
		current_state_buffer = { current_state, current_state_copy };
		previous_state_buffer = { previous_state, previous_state_copy };
		updateVectors();
	}

	virtual void tick(int current_tick);
	virtual void init(Theatre *parent_theatre);
	virtual void updateStates(std::mutex &state_mutex);
	virtual bool wantsToBeRendered();
	virtual bool wantsToBeBuffered();

protected:
	bool debug_visible;
	virtual void updateVectors();
};
#endif