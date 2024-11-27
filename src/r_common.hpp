#include "sanity.hpp"
#include <string>

#ifndef GRAPHX_RENDERING_FLAGS_AND_TYPES
#define GRAPHX_RENDERING_FLAGS_AND_TYPES
	#define GLSHADER_TYPE_VERTEX 0
	#define GLSHADER_TYPE_FRAGMENT 1
	#define GLSHADER_TYPE_PROGRAM 2
#endif

#ifndef GRAPHX_RENDERING_FUNCTIONAL
#define GRAPHX_RENDERING_FUNCTIONAL
	GLFWwindow *W_CreateWindow(u_int16_t width, u_int16_t height, const char* title = "Fucking GraphX", bool make_context_current = true);
	void W_SwapAndClear(GLFWwindow *w_window, float clear_color_r = 0.3f, float clear_color_g = 0.4f, float clear_color_b = 0.7f, float clear_color_a = 1.0f);
	GLuint T_GenerateTexture(const char *filepath);
#endif

#ifndef GRAPHX_RENDERING_OBJECT_ORIENTATED
#define GRAPHX_RENDERING_OBJECT_ORIENTATED
//
// Shader
//
class GLShader
{
public:
	unsigned int ID;

	GLShader(const char *vertex_path, const char *fragment_path);

	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMatrix(const std::string &name, glm::mat4 value) const;

private:
	void shaderErrorHandler(int thing, int type);
};

//
// Cube
//
class Cube
{
public:
	float vertices[40] = {
		// positions			// texture coords
		-1.0f, -1.0f, -1.0f,	0.0f, 0.0f,		// Left		Down	Back		0
		-1.0f, -1.0f,  1.0f,	1.0f, 0.0f,		// Left		Down	Forward		1
		-1.0f,  1.0f, -1.0f,	0.0f, 1.0f,		// Left		Up		Back		2
		-1.0f,  1.0f,  1.0f,	1.0f, 1.0f,		// Left		Up		Forward		3
		 1.0f, -1.0f, -1.0f,	1.0f, 0.0f,		// Right	Down	Back		4
		 1.0f, -1.0f,  1.0f,	0.0f, 0.0f,		// Right	Down	Forward		5
		 1.0f,  1.0f, -1.0f,	1.0f, 1.0f,		// Right	Up		Back		6
		 1.0f,  1.0f,  1.0f,	0.0f, 1.0f		// Right	Up		Forward		7
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