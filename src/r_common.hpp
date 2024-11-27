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
#endif