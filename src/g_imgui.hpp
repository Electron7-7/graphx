#ifndef GRAPHX_IMGUI
#define GRAPHX_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <glm/fwd.hpp>
#include <string>
#include <vector>

// Forward Declarations
struct Theatre;
struct gMeshData;


struct GraphXConsole
{
	bool active;
	bool secondary_active;
	bool tertiary_active;

	ImGuiKey toggle_activation_key;
	std::string name;
	Theatre *current_theatre = nullptr;

	GraphXConsole();

	void updateFrame(GLFWwindow *window);
	void displayActorDebugger();
	void displayTheatreData();
	bool justClosed();

private:
	bool was_active;
};

extern bool show_imgui_window;

void toggleCursor(GLFWwindow *window, bool show_cursor);
#endif