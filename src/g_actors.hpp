#include "sanity.hpp"
#include "r_renderer.hpp"
#include "default_cube.graphxmodel"

#ifndef GRAPHX_ACTORS_OBJECT_ORIENTATED
#define GRAPHX_ACTORS_OBJECT_ORIENTATED
//
// Actor Struct
//
struct Actor
{
	constexpr static const float INIT_YAW = -90.0f;
	constexpr static const float INIT_PITCH = 0.0f;
	constexpr static const float INIT_SPEED = 2.5f;

	bool renderable = true;
	const char* name;
	const char* texture;
	int vertex_attributes;

	glm::vec3 position_global;
	// glm::vec4 rotation_quaternion;
	glm::vec3 rotation_euler;		// x (pitch), y (yaw), z (roll)
	glm::vec2 velocity_horizontal;

	glm::vec3 orientation_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;

	glm::vec3 world_orientation_up;

	float movement_speed;

	Actor(const char *new_name, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_up = glm::vec3(0.0f, 1.0f, 0.0f), float init_yaw = INIT_YAW, float init_pitch = INIT_PITCH);

	void doMovement(int direction[2], float delta_time = 0.016f);

	void updateActorVectors();
};

//
// PlayerController Struct
//
struct GraphXPlayer: Actor
{
	constexpr static const float INIT_SENSITIVITY = 0.1f;

	float mouse_sensitivity;

	using Actor::Actor;
	GraphXPlayer(const char *new_name, glm::vec3 init_position = glm::vec3(0.0f));

	glm::mat4 getViewMatrix();
	void doMouseMovement(float offset[2], GLboolean constrain_pitch = true);
};

//
// Tester Struct
//
struct Tester: Actor
{
	bool renderable = true;
	int position_flip = 0;

	// RenderInitializeCmd render_init_cmd;
	glm::vec3 testing_position[2] =
	{
		glm::vec3(-3.0f, 0.0f, 0.0f),
		glm::vec3(3.0f, 0.0f, 0.0f)
	};

	using Actor::Actor;
	Tester();

	void flipPosition();
	void makeCube();	// This should be handeled by the renderer
	void drawCube();	// This too
};
#endif


#ifndef GRAPHX_ACTORS_FUNCTIONAL
#define GRAPHX_ACTORS_FUNCTIONAL
#endif