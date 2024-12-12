#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
#include "sanity.hpp"
#include "r_common.hpp"
#include <vector>

class Actor
{
public:
	int state_index = 0;

	RenderCmd render_command;

	RenderState current_state;
	RenderState current_state_copy = current_state;

	RenderState previous_state = current_state;
	RenderState previous_state_copy = current_state;

	std::vector<RenderState> current_state_buffer	=	{ current_state,	current_state_copy	};
	std::vector<RenderState> previous_state_buffer	=	{ previous_state,	previous_state_copy	};

	bool visible = true;
	const char *name;
	float movement_speed;

	/*
		EMERGENCY BRAKE
		Actors should always have meshes (Thinkers will be for non-rendered game objects). If the render storage vector is full of Mesh pointers
		then it's fine for them to have a default constructor that's overwritten later by the code or  immediately by the Actor constructor,
		AS LONG AS the render storage is generated AFTER giving each Actor a Mesh (or leaving it as the default ERROR mesh).
	*/
	Mesh *mesh = new Mesh;

	glm::vec3 position_global;
	// glm::vec4 rotation_quaternion;
	glm::vec3 rotation_euler;		// x (pitch), y (yaw), z (roll)
	glm::vec2 velocity_horizontal;

	glm::vec3 orientation_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;

	glm::vec3 world_orientation_up;

	Actor(const char *new_name, Mesh init_mesh = Mesh(), glm::vec3 init_position = glm::vec3(0.0f), float init_yaw = INIT_YAW, float init_pitch = INIT_PITCH);

	bool gatekeepRenderer() { return ((mesh->vao_id != VAO_ERR) && visible); }

	virtual void Tick();

protected:
	constexpr static const float INIT_YAW = -90.0f;
	constexpr static const float INIT_PITCH = 0.0f;
	constexpr static const float INIT_SPEED = 2.5f;

	void updateVectors();

	virtual void updateStates();
};

class GraphXPlayer: public Actor
{
public:
	constexpr static const float INIT_SENSITIVITY = 0.1f;

	float mouse_sensitivity;

	using Actor::Actor;
	GraphXPlayer(const char *new_name, glm::vec3 init_position = glm::vec3(0.0f))
	: Actor(new_name, Mesh(), init_position), mouse_sensitivity(INIT_SENSITIVITY)
	{}

	glm::mat4 getViewMatrix();
	void doMouseMovement(float offset[2], GLboolean constrain_pitch = true);
	void doMovement(int direction[2], float delta_time = 0.016f);
};

class Tester: public Actor
{
public:
	int position_flip = 0;

	glm::vec3 testing_position[2] =
	{
		glm::vec3(-3.0f, 2.0f, -6.0f),
		glm::vec3(3.0f, 2.0f, -6.0f)
	};

	using Actor::Actor;
	Tester() : Actor("Tester", Mesh(), glm::vec3(0.0f, 0.0f, -3.0f))
	{}

	void Tick() override;
};
#endif