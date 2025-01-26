#ifndef GRAPHX_ENGINE_COMMON
#define GRAPHX_ENGINE_COMMON
#include "r_common.hpp"
#include "g_math.hpp"

#define EULER_CHANGE_QUATERNION 0
#define QUATERNION_CHANGE_EULER 1

#define ACTOR_ACTOR 			0
#define ACTOR_TOOL  			1
#define ACTOR_LIGHT 			1
#define ACTOR_PHYSICS			2
#define ACTOR_PLAYER			3

#define DEVICE_DEVICE			0
#define DEVICE_COLLIDER			1

struct Device
{
	unsigned int type;

	Device()
	{ type = DEVICE_DEVICE; }

	Device(Device &sample)
	{ type = sample.type; }
};

struct RenderState
{
	glm::vec3 render_position;
	JPH::Quat render_quaternion;
	// glm::vec3 render_euler;
	glm::vec3 render_scale;

	RenderState(glm::vec3 init_position = glm::vec3(0.0f), JPH::Quat init_quaternion = JPH::Quat::sIdentity(), glm::vec3 init_scale = glm::vec3(1.0f))
	: render_position(init_position), render_quaternion(init_quaternion), render_scale(init_scale)
	{}
};

class Actor
{
public:
	Mesh mesh;
	std::unordered_map<unsigned int, Device *> devices;

	unsigned int actor_type;
	bool visible = true;
	std::string name;
	float movement_speed = 1.0f;

	glm::vec3 position_global;
	glm::vec3 rotation_euler;
	JPH::Quat rotation_quaternion;
	glm::vec3 scale = glm::vec3(1.0f);
	glm::vec2 velocity_horizontal;
	glm::vec3 velocity;

	glm::vec3 orientation_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;

	glm::vec3 world_orientation_up;

	RenderState current_state;
	RenderState current_state_copy = current_state;

	RenderState previous_state = current_state;
	RenderState previous_state_copy = current_state;

	std::vector<RenderState> current_state_buffer = { current_state, current_state_copy };
	std::vector<RenderState> previous_state_buffer = { previous_state, previous_state_copy };

	int state_index = 0;

	Actor(std::string new_name, Mesh init_mesh = Mesh(), glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_rotation_euler = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: mesh(init_mesh), position_global(init_position), rotation_euler(init_rotation_euler), scale(init_scale), orientation_front(glm::vec3(0.0f, 0.0f, -1.0f))
	{
		actor_type = ACTOR_ACTOR;
		name = new_name;
		world_orientation_up = glm::vec3(0.0f, 1.0f, 0.0f);
		rotation_euler = init_rotation_euler;
		rotation_quaternion = JPH::Quat::sEulerAngles(convertMath<JPH::Vec3>(init_rotation_euler));
		current_state = RenderState(init_position, rotation_quaternion);
		updateVectors();
	}

	int giveDevice(Device *new_device);
	Device *getDevice(unsigned int device_type);

	virtual void Tick(int current_tick);
	virtual void init(Theatre *parent_theatre);
	virtual void updateStates(std::mutex &state_mutex);
	virtual bool wantsToBeRendered();
	virtual bool wantsToBeBuffered();

protected:
	bool debug_visible;

	void updateRotation(bool override_which);
	void updateVectors();
};

void P_CheckCollisions(std::vector<Actor *> troupe);
glm::vec3 P_CalculateInelasticCollision(Actor *stooge_left, Actor *stooge_right); // Naming colliding Actors "stooges"

extern std::vector<std::vector<Actor *>> to_be_collided;
#endif