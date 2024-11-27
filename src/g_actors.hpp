#include "sanity.hpp"

#ifndef GRAPHX_ACTORS_FUNCTIONAL
#define GRAPHX_ACTORS_FUNCTIONAL
#endif


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
	// constexpr static const float INIT_SENSITIVITY = 0.1f;

	const char *name;

	glm::vec3 position_global;
	glm::vec2 velocity_horizontal;
	glm::vec3 orientation_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;
	glm::vec3 world_orientation_up;

	float yaw;
	float pitch;
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

	GraphXPlayer(const char *new_name, glm::vec3 init_position = glm::vec3(0.0f));

	glm::mat4 getViewMatrix();
	void doMouseMovement(float offset[2], GLboolean constrain_pitch = true);
};
#endif