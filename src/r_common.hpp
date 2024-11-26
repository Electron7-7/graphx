// r_common.hpp - Generic objects, primitives, cameras, etc
#include "sanity.hpp"

#ifndef GRAPHX_PRIMITIVES
#define GRAPHX_PRIMITIVES

//
// Cube
//
class Cube
{
public:
	float vertices[24] = {
		-1.0f, -1.0f, -1.0f,	// Left		Down	Back		0
		-1.0f, -1.0f, 1.0f,		// Left		Down	Forward		1
		-1.0f, 1.0f, -1.0f,		// Left		Up		Back		2
		-1.0f, 1.0f, 1.0f,		// Left		Up		Forward		3
		 1.0f, -1.0f, -1.0f,	// Right	Down	Back		4
		 1.0f, -1.0f, 1.0f,		// Right	Down	Forward		5
		 1.0f, 1.0f, -1.0f,		// Right	Up		Back		6
		 1.0f, 1.0f, 1.0f,		// Right	Up		Forward		7
		};

	unsigned int indices[36] = {
		// -X Square
		0, 1, 2,	// Bottom Triangle
		3, 1, 2,	// Top Triangle

		// +X Square
		4, 5, 6,
		7, 5, 6,

		// -Y Square
		0, 1, 4,
		5, 1, 4,

		// +Y Square
		2, 3, 6,
		7, 3, 6,

		// -Z Square
		0, 2, 4,
		6, 2, 4,

		// +Z Square
		1, 3, 5,
		7, 3, 5
	};

	void makeCube();
	void drawCube();
};

#endif

#ifndef GRAPHX_DEBUG_CAMERA
#define GRAPHX_DEBUG_CAMERA

//
// Debug Camera (not for in-game use)
//
class DebugCamera
{
public:
	constexpr static const float YAW = -90.0f;
	constexpr static const float PITCH = 0.0f;
	constexpr static const float SPEED = 2.5f;
	constexpr static const float SENSITIVITY = 0.1f;

	glm::vec3 position;
	glm::vec2 horizontal_velocity;
	glm::vec3 front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 world_up;

	float yaw;
	float pitch;
	float movement_speed;
	float mouse_sensitivity;

	DebugCamera(glm::vec3 init_position = glm::vec3(0.0f), glm::vec3 init_up = glm::vec3(0.0f, 1.0f, 0.0f), float init_yaw = YAW, float init_pitch = PITCH);

	glm::mat4 getViewMatrix();
	void doMovement(int direction[2], float delta_time = 0.016f);
	void doMouseMovement(float offset[2], GLboolean constrain_pitch = true);

private:
	void updateCameraVectors();
};

#endif