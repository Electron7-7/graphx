// r_common.hpp - Common Rendering Declarations
#include "sanity.hpp"
#include <string>

#define GLSHADER_TYPE_VERTEX 0
#define GLSHADER_TYPE_FRAGMENT 1
#define GLSHADER_TYPE_PROGRAM 2


#ifndef GRAPHX_RENDERING
#define GRAPHX_RENDERING

//
// Object-Oriented
//
class GLShader
{
public:
	unsigned int ID;

	GLShader(const char* vertex_path, const char* fragment_path);

	void use();

	void setBool(const std::string& name, bool value) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float value) const;
	void setMatrix(const std::string& name, glm::mat4 value) const;

private:
	void shaderErrorHandler(int thing, int type);
};

//
// Functional
//
GLFWwindow *W_CreateWindow(u_int16_t width, u_int16_t height, const char* title = "Fucking GraphX", bool make_context_current = true);
void W_SwapAndClear(GLFWwindow* w_window, float clear_color_r = 0.3f, float clear_color_g = 0.4f, float clear_color_b = 0.7f, float clear_color_a = 1.0f);
GLuint T_GenerateTexture(const char* filepath);

#endif