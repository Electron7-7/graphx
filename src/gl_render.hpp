#include <string>
#include "sanity.hpp"

#ifndef OPENGL_WINDOW_HEADER
#define OPENGL_WINDOW_HEADER

class Window
{
public:
	Window(u_int16_t width = 1280, u_int16_t height = 720);

	u_int16_t w_width, w_height;
	GLFWwindow* w_window;

	void SwapAndClear(float clear_color[4] = default_clear_color);

private:
	inline static float default_clear_color[4] = { 0.3f, 0.4f, 0.7f, 1.0f };
};

#endif

#ifndef GRAPHX_SHADER_LIB
#define GRAPHX_SHADER_LIB
#define GLSHADER_TYPE_VERTEX 0
#define GLSHADER_TYPE_FRAGMENT 1
#define GLSHADER_TYPE_PROGRAM 2

class GLShader
{
public:
	unsigned int ID;

	GLShader(const char* vertex_path, const char* fragment_path);

	void use();

	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
	void setMatrix(const std::string &name, glm::mat4 value) const;

private:
	void shaderErrorHandler(int thing, int type);
};

#endif