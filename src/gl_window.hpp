#include "sanity.hpp"
#include <sys/types.h>

#ifndef OPENGL_WINDOW_HEADER
#define OPENGL_WINDOW_HEADER

class Window
{
public:
	Window(u_int16_t width = 1280, u_int16_t height = 720);

	u_int16_t w_width, w_height;
	GLFWwindow* w_window;

	void SwapAndClear();
};

#endif