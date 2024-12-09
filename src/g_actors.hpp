#include "sanity.hpp"
#include "r_common.hpp"
// #include "default_cube.graphxmodel"

#ifndef GRAPHX_ACTORS
#define GRAPHX_ACTORS
//
// Object-Oriented
//
struct Actor
{
	constexpr static const float INIT_YAW = -90.0f;
	constexpr static const float INIT_PITCH = 0.0f;
	constexpr static const float INIT_SPEED = 2.5f;

	const char* name;
	Mesh* mesh;
	bool visible = true;

	glm::vec3 position_global;
	// glm::vec4 rotation_quaternion;
	glm::vec3 rotation_euler;		// x (pitch), y (yaw), z (roll)
	glm::vec2 velocity_horizontal;

	glm::vec3 orientation_front;
	glm::vec3 orientation_up;
	glm::vec3 orientation_right;

	glm::vec3 world_orientation_up;

	float movement_speed;

	Actor(const char* new_name, glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_up = glm::vec3(0.0f, 1.0f, 0.0f), float init_yaw = INIT_YAW, float init_pitch = INIT_PITCH);

	bool isVisible() { return (!mesh->isEmpty() && visible); }
	void updateVectors();
	void doMovement(int direction[2], float delta_time = 0.016f);
};

struct GraphXPlayer: Actor
{
	constexpr static const float INIT_SENSITIVITY = 0.1f;

	float mouse_sensitivity;

	using Actor::Actor;
	GraphXPlayer(const char* new_name, glm::vec3 init_position = glm::vec3(0.0f));

	glm::mat4 getViewMatrix();
	void doMouseMovement(float offset[2], GLboolean constrain_pitch = true);
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
	Tester();

	void flipPosition();
};


extern std::vector<Actor*> renderables;
#endif