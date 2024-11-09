#ifndef GL_INCLUDES
#define GL_INCLUDES
	#include <glad/glad.h>
	#include <GLFW/glfw3.h>
	#include <glm/glm.hpp>
	#include <glm/gtc/matrix_transform.hpp>
	#include <glm/gtc/type_ptr.hpp>
#endif

#ifndef COMMON_INCLUDES
#define COMMON_INCLUDES
	#include <string>
#endif

#ifndef COMMON_FUNCTIONS
#define COMMON_FUNCTIONS

void shaderErrorHandler(unsigned int shader, bool is_program = false);

#endif

#ifndef SHADER_LIB
#define SHADER_LIB

class Shader
{
public:
	unsigned int ID;

	Shader(const char* vertex_path, const char* fragment_path);

	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
};
#endif

#ifndef CAMERA_CLASS
#define CAMERA_CLASS

enum Camera_Movement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;

class Camera
{
public:
	glm::vec3 Position;
	glm::vec3 Front;
	glm::vec3 Up;
	glm::vec3 Right;
	glm::vec3 WorldUp;

	float Yaw;
	float Pitch;

	float MovementSpeed;
	float MouseSensitivity;

	Camera(glm::vec3 position = glm::vec3(0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);

	glm::mat4 GetViewMatrix();
	void ProcessKeyboard(Camera_Movement direction, float deltaTime);
	void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch);

private:
	void updateCameraVectors();
};

#endif