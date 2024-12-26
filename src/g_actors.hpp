#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
#include "sanity.hpp"
#include "r_common.hpp"
#include <vector>
#include <mutex>

class Actor
{
public:
	Mesh mesh;

	RenderState current_state;
	RenderState current_state_copy = current_state;

	RenderState previous_state = current_state;
	RenderState previous_state_copy = current_state;

	std::vector<RenderState> current_state_buffer	=	{ current_state,	current_state_copy	};
	std::vector<RenderState> previous_state_buffer	=	{ previous_state,	previous_state_copy	};

	int state_index = 0;
	bool visible = true;
	std::string name;
	float movement_speed = 1.0f;

	glm::vec3 position_global;
	// glm::vec4 rotation_quaternion;
	glm::vec3 rotation_euler; // x (pitch), y (yaw), z (roll)
	glm::vec3 scale = glm::vec3(1.0f);
	glm::vec2 velocity_horizontal;

	glm::vec3 orientation_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;

	glm::vec3 world_orientation_up;

	Actor(std::string new_name, Mesh init_mesh = Mesh(), glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f), float init_yaw = INIT_YAW, float init_pitch = INIT_PITCH);

	// virtual ~Actor();

	virtual bool gatekeepRenderer();
	virtual void Tick(int current_tick);
	virtual void updateStates(std::mutex &state_mutex);

protected:
	constexpr static const float INIT_YAW = -90.0f;
	constexpr static const float INIT_PITCH = 0.0f;

	void updateVectors();
};

class GraphXPlayer: public Actor
{
public:
	float mouse_sensitivity;
	float movement_speed = 0.05f;

	GraphXPlayer(std::string new_name, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: Actor(new_name, Mesh(), init_position, init_scale), mouse_sensitivity(INIT_SENSITIVITY)
	{}

	bool gatekeepRenderer() override;

	glm::mat4 getViewMatrix();
	void doMouseMovement(std::vector<float> offset, bool constrain_pitch = true);
	void doMovement(int direction[2]);

protected:
	constexpr static const float INIT_SENSITIVITY = 0.1f;
};

class MoverTester: public Actor
{
public:
	float movement_speed = 0.025f;
	int t_direction = 0;

	MoverTester(std::string init_name, Mesh init_mesh = Mesh(), glm::vec3 init_position = glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: Actor(init_name, init_mesh, init_position, init_scale)
	{}

	void Tick(int current_tick) override;
};

class SpriteTester: public Actor
{
public:
	SpriteTester(std::string init_name, Sprite init_sprite = Sprite(), glm::vec3 init_position = glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3 init_scale = glm::vec3(1.0f))
	: Actor(init_name, init_sprite, init_position, init_scale)
	{}
};
#else
class Actor;
class GraphXPlayer;
class MoverTester;
class SpriteTester;
#endif