#include "g_imgui.hpp"
#include "g_common.hpp"
#include "r_common.hpp"

namespace IMGUI = ImGui;

//
// GraphXConsole
//
GraphXConsole::GraphXConsole()
: active(false), secondary_active(true), toggle_activation_key(ImGuiKey_Tab), name("GraphX Debug Console"), was_active(false)
{

}

bool GraphXConsole::justClosed()
{
	if(was_active && !active)
	{
		was_active = false;
		return true;
	}
	
	return false;
}

void GraphXConsole::updateFrame(GLFWwindow *window)
{
	static int last_used_vao_index = current_vao_index;
	static int last_vao_index = current_vao_index;
	if(current_vao_index != last_vao_index)
	{
		last_used_vao_index = last_vao_index;
		last_vao_index = current_vao_index;
		if(last_used_vao_index == (VAOS_AMOUNT + 1))
			last_used_vao_index = current_vao_index;
	}

	if(IMGUI::IsKeyPressed(ImGuiKey_Tab))
		active = !active;

	if(justClosed())
		toggleCursor(window, active);

	if(!active)
		return;

	std::string vao_output_strings = "Last Used VAO: ";
	switch(last_used_vao_index)
	{
	case VAO_HANDMADE:
		vao_output_strings += "VAO_HANDMADE";
		break;
	case VAO_OBJ_FULL:
		vao_output_strings += "VAO_OBJ_FULL";
		break;
	case VAO_OBJ_SEMI:
		vao_output_strings += "VAO_OBJ_SEMI";
		break;
	default:
		vao_output_strings += "NO VAO SET";
		break;
	}

	vao_output_strings += "\nCurrent VAO: ";
	switch(current_vao_index)
	{
	case VAO_HANDMADE:
		vao_output_strings += "VAO_HANDMADE";
		break;
	case VAO_OBJ_FULL:
		vao_output_strings += "VAO_OBJ_FULL";
		break;
	case VAO_OBJ_SEMI:
		vao_output_strings += "VAO_OBJ_SEMI";
		break;
	default:
		vao_output_strings += "NO VAO SET";
		break;
	}

	was_active = active;
	IMGUI::SetNextWindowPos(ImVec2(540, 15), ImGuiCond_Once);
	IMGUI::SetNextWindowSize(ImVec2(725, 695), ImGuiCond_Once);
	IMGUI::Begin(name.c_str(), &active);
	IMGUI::Text("%s", vao_output_strings.c_str());
	if(IMGUI::Button("Toggle Theatre Printout"))
	{
		secondary_active = !secondary_active;
	}
	if(secondary_active)
	{
		IMGUI::BeginChild(IMGUI::GetMainViewport()->ID);
		displayTheatre();
		IMGUI::EndChild();
	}
	IMGUI::End();
}

void GraphXConsole::displayTheatre()
{
	if(current_theatre == nullptr || current_theatre->name == std::string("Untitled Theatre"))
		return;
	IMGUI::Text("Currently Loaded Theatre: \"%s\"", current_theatre->name.c_str());
	IMGUI::Separator();
	IMGUI::Text("Actors:");
	IMGUI::Text("%s", current_theatre->giveMeAPrettyListOfAllActorsOrDevices(true).c_str());
	IMGUI::Separator();
	IMGUI::Text("Devices:");
	IMGUI::Text("%s", current_theatre->giveMeAPrettyListOfAllActorsOrDevices(false).c_str());
}