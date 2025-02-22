#ifndef GRAPHX_IMGUI
#define GRAPHX_IMGUI
#include "imgui.h"
#include "g_common.hpp"
#include <string>

struct GraphXConsole
{
	bool active;
	ImGuiKey toggle_activation_key;
	std::string name;
	Theatre *current_theatre = nullptr;

	GraphXConsole();

	void updateFrame(GLFWwindow *window);
	void displayTheatre();
	bool justClosed();

private:
	bool was_active;
};

extern bool show_imgui_window;

void toggleCursor(GLFWwindow *window, bool show_cursor);
#endif