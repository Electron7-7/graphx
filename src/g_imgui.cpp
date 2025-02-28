#include "g_imgui.hpp"
#include "g_common.hpp"

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
	if(IMGUI::IsKeyPressed(ImGuiKey_Tab))
		active = !active;

	if(justClosed())
		toggleCursor(window, active);

	if(!active)
		return;

	was_active = active;
	IMGUI::SetNextWindowPos(ImVec2(540, 15), ImGuiCond_Once);
	IMGUI::SetNextWindowSize(ImVec2(725, 695), ImGuiCond_Once);
	IMGUI::Begin(name.c_str(), &active);
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