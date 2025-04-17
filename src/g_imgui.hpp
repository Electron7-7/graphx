#ifndef GRAPHX_IMGUI
#define GRAPHX_IMGUI
#include "g_common_fwd.hpp"
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
public:
	bool active;
	bool secondary_active;
	bool tertiary_active;

	ImGuiKey toggle_activation_key;
	std::string name;
	Theatre *current_theatre = nullptr;

	GraphXConsole();

	void updateFrame(GLFWwindow *window);
	void displayTheatreData();
	bool justClosed();

private:
	bool was_active;
	bool keep_debug_labels_on = false;
	bool actor_selection_made = false;
	bool actor_selection_valid = false;
	Actor *actor = nullptr;
	std::string actor_name_selection = "";
	std::string actor_uid_selection = "";
	std::string error_string = "";
};

extern bool show_imgui_window;

void toggleCursor(GLFWwindow *window, bool show_cursor);
#endif