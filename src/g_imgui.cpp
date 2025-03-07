#include "g_imgui.hpp"
#include "g_common.hpp"
#include "r_common.hpp"

namespace IMGUI = ImGui;

//
// GraphXConsole
//
GraphXConsole::GraphXConsole()
: active(false), secondary_active(false), tertiary_active(false), toggle_activation_key(ImGuiKey_Tab), name("GraphX Debug Console"), was_active(false)
{}

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

	if(secondary_active)
		displayTheatre();

	if(tertiary_active)
		displayTheatreData();

	if(!active)
		return;

	was_active = active;
#ifndef GRAPHX_DEBUG
	IMGUI::SetNextWindowSize(ImVec2(465, 71), ImGuiCond_Once);
	IMGUI::SetNextWindowPos(ImVec2(760, 55), ImGuiCond_Once);
#endif
	IMGUI::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus);
	IMGUI::Text("%s", std::string("Currently Loaded Theatre: " + current_theatre->name).c_str());
	if(IMGUI::Button("Toggle Theatre Printout"))
		secondary_active = !secondary_active;
	IMGUI::SameLine();
	if(IMGUI::Button("Toggle GraphXTheatre Printout"))
		tertiary_active = !tertiary_active;
	IMGUI::End();
}

void GraphXConsole::displayTheatre()
{
	if(current_theatre == nullptr || !current_theatre->name.compare("Untitled Theatre"))
		return;
	std::string this_name = current_theatre->name + " - Actors & Devices";
#ifndef GRAPHX_DEBUG
	IMGUI::SetNextWindowSize(ImVec2(510, 485), ImGuiCond_Once);
	IMGUI::SetNextWindowPos(ImVec2(35, 220), ImGuiCond_Once);
#endif
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoInputs;
	if(active)
		window_flags = ImGuiWindowFlags_None;
	IMGUI::Begin(this_name.c_str(), &secondary_active, window_flags);
	IMGUI::Text("Actors:");
	IMGUI::Text("%s", current_theatre->giveMeAPrettyListOfAllActorsOrDevices(true).c_str());
	IMGUI::Separator();
	IMGUI::Text("Devices:");
	IMGUI::Text("%s", current_theatre->giveMeAPrettyListOfAllActorsOrDevices(false).c_str());
	IMGUI::End();
}

void GraphXConsole::displayTheatreData()
{
	if(current_theatre == nullptr || !current_theatre->name.compare("Untitled Theatre"))
		return;
	std::string this_name = current_theatre->name + " - Parsed GraphXTheatre Settings";
#ifndef GRAPHX_DEBUG
	IMGUI::SetNextWindowSize(ImVec2(635, 520), ImGuiCond_Once);
	IMGUI::SetNextWindowPos(ImVec2(600, 175), ImGuiCond_Once);
#endif
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoInputs;
	if(active)
		window_flags = ImGuiWindowFlags_None;
	IMGUI::Begin(this_name.c_str(), &tertiary_active, window_flags);
	IMGUI::Text("%s", current_theatre->theatre_file_data_printout.c_str());
	IMGUI::End();
}