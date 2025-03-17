#ifndef GRAPHX_IMGUI
#define GRAPHX_IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <glm/fwd.hpp>
#include <string>
#include <map>

// Forward Declarations
struct Theatre;
struct gMeshData;

struct GraphXConsole
{
	bool active;
	bool secondary_active;
	bool tertiary_active;
	bool fourth_active;

	ImGuiKey toggle_activation_key;
	std::string name;
	Theatre *current_theatre = nullptr;

	GraphXConsole();

	void updateFrame(GLFWwindow *window);
	void displayTheatre();
	void displayTheatreData();
	void displayMeshData();
	bool justClosed();
	std::string getMeshData(std::pair<const std::string, gMeshData> &mesh_data);

private:
	std::map<std::string, gMeshData> mesh_data_store;
	std::map<std::string, std::string> mesh_data_printouts;
	bool was_active;
};

extern bool show_imgui_window;

void toggleCursor(GLFWwindow *window, bool show_cursor);
#endif