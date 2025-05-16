#ifndef GRAPHX_IMGUI
#define GRAPHX_IMGUI
#include <glm/glm.hpp>
#define IM_VEC4_CLASS_EXTRA \
	constexpr ImVec4(const glm::vec4& f) : x(f.x), y(f.y), z(f.z), w(f.w) {} \
	operator glm::vec4() const { return glm::vec4(x,y,z,w); }
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include <string>

#ifdef COMPILER_FORWARD_DECLARATIONS // This is to keep forward declarations from causing issues when including header files
class Actor; // Forward Declaration
#endif

struct GraphXConsole
{
public:
	bool active;
	bool secondary_active;
	bool tertiary_active;
	bool quaternary_active;

	ImGuiKey toggle_activation_key;
	std::string name;

	GraphXConsole();

	void updateFrame(GLFWwindow* window);
	void showActorEditor(Actor*, int = -1);
	void displayTheatrePrintout();
	void exportTheatreFile();
	void liveTheatreEditor();
	bool justClosed();

private:
	bool was_active;
	bool keep_debug_labels_on = false;
	bool actor_selection_made = false;
	bool actor_selection_valid = false;
	Actor* single_actor = nullptr;
	std::string actor_name_selection = "";
	std::string actor_uid_selection = "";
	std::string error_string = "";
};

extern bool show_imgui_window;

void toggleCursor(GLFWwindow *window, bool show_cursor);
#endif