#ifndef GRAPHX_IMGUI
#define GRAPHX_IMGUI
#include "sanity.hpp" // Included for the GLM header files
#include "imgui.h"
#include <string>

// Forward Declarations
struct Theatre;

struct GraphXConsole
{
	bool active;
	bool secondary_active;
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