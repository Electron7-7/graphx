#include <g_common.hpp>

#ifndef CAMERA_CLASS
#define CAMERA_CLASS

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;

class Camera
{
public:
	glm::vec3 Position;
	glm::vec2 horizontal_velocity;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	float movement_speed;
	float mouse_sensitivity;

	Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	glm::mat4 GetViewMatrix();
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);
	void DoMovement(int direction[2], float delta_time = 0.016);

private:
	void updateCameraVectors();
};

#endif