#include "sanity.hpp"
#include "r_common.hpp"
#include <vector>

#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
struct Actor
{
	constexpr static const float INIT_YAW = -90.0f;
	constexpr static const float INIT_PITCH = 0.0f;
	constexpr static const float INIT_SPEED = 2.5f;

	int state_index = 0;

	RenderState current_state_original;
	RenderState current_state_copy = current_state_original;

	RenderState previous_state_original;
	RenderState previous_state_copy = previous_state_original;

	std::vector<RenderState> current_state	=	{ current_state_original,	current_state_copy	};
	std::vector<RenderState> previous_state	=	{ previous_state_original,	previous_state_copy	};

	bool visible = true;
	const char* name;
	float movement_speed;

	Mesh* mesh = NULL;

	glm::vec3 position_global;
	// glm::vec4 rotation_quaternion;
	glm::vec3 rotation_euler;		// x (pitch), y (yaw), z (roll)
	glm::vec2 velocity_horizontal;

	glm::vec3 orientation_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;

	glm::vec3 world_orientation_up;

	Actor(const char* new_name, glm::vec3 init_position = glm::vec3(0.0f), float init_yaw = INIT_YAW, float init_pitch = INIT_PITCH);

	bool isRenderable() { return ((mesh != NULL) && visible); }
	void updateVectors();

	virtual void Tick();
};

struct GraphXPlayer: Actor
{
	constexpr static const float INIT_SENSITIVITY = 0.1f;

	float mouse_sensitivity;

	using Actor::Actor;
	GraphXPlayer(const char* new_name, glm::vec3 init_position = glm::vec3(0.0f));

	glm::mat4 getViewMatrix();
	void doMouseMovement(float offset[2], GLboolean constrain_pitch = true);
	void doMovement(int direction[2], float delta_time = 0.016f);
};

struct Tester: Actor
{
	int position_flip = 0;

	glm::vec3 testing_position[2] =
	{
		glm::vec3(-3.0f, 2.0f, -6.0f),
		glm::vec3(3.0f, 2.0f, -6.0f)
	};

	using Actor::Actor;
	Tester() : Actor("Tester", glm::vec3(0.0f, 0.0f, -3.0f))
	{}

	void flipPosition();
};

extern std::vector<Actor> actors_in_current_space;
#endif