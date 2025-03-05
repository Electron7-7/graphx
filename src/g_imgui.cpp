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

	if(secondary_active)
	{
		displayTheatre();
	}

	if(tertiary_active)
	{
		displayTheatreData();
	}

	if(!active)
		return;

	std::string vao_output_strings = "Last Used VAO: ";
	switch(last_used_vao_index)
	{
	case VAO_HANDMADE:
		vao_output_strings += "VAO_HANDMADE";
		break;
	case VAO_OBJ:
		vao_output_strings += "VAO_OBJ";
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
	case VAO_OBJ:
		vao_output_strings += "VAO_OBJ";
		break;
	default:
		vao_output_strings += "NO VAO SET";
		break;
	}

	was_active = active;
	IMGUI::SetNextWindowSize(ImVec2(465, 71), ImGuiCond_Once);
	IMGUI::SetNextWindowPos(ImVec2(760, 55), ImGuiCond_Once);
	IMGUI::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus);
	// IMGUI::Text("%s", vao_output_strings.c_str());
	// IMGUI::Text("%s", std::string("Size: " + std::to_string(IMGUI::GetWindowSize()[0]) + ", " + std::to_string(IMGUI::GetWindowSize()[1]) + " Position: " + std::to_string(IMGUI::GetWindowPos()[0]) + ", " + std::to_string(IMGUI::GetWindowPos()[1])).c_str());
	IMGUI::Text("%s", std::string("Currently Loaded Theatre: " + current_theatre->name).c_str());
	if(IMGUI::Button("Toggle Theatre Printout"))
	{
		secondary_active = !secondary_active;
	}
	IMGUI::SameLine();
	if(IMGUI::Button("Toggle GraphXTheatre Printout"))
	{
		tertiary_active = !tertiary_active;
	}
	IMGUI::End();
}

void GraphXConsole::displayTheatre()
{
	if(current_theatre == nullptr || current_theatre->name == std::string("Untitled Theatre"))
		return;
	std::string this_name = current_theatre->name + " - Actors & Devices";
	IMGUI::SetNextWindowSize(ImVec2(510, 485), ImGuiCond_Once);
	IMGUI::SetNextWindowPos(ImVec2(35, 220), ImGuiCond_Once);
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoInputs;
	if(active)
		window_flags = ImGuiWindowFlags_None;
	IMGUI::Begin(this_name.c_str(), &secondary_active, window_flags);
	// IMGUI::Text("%s", std::string("Size: " + std::to_string(IMGUI::GetWindowSize()[0]) + ", " + std::to_string(IMGUI::GetWindowSize()[1]) + " Position: " + std::to_string(IMGUI::GetWindowPos()[0]) + ", " + std::to_string(IMGUI::GetWindowPos()[1])).c_str());
	IMGUI::Text("Actors:");
	IMGUI::Text("%s", current_theatre->giveMeAPrettyListOfAllActorsOrDevices(true).c_str());
	IMGUI::Separator();
	IMGUI::Text("Devices:");
	IMGUI::Text("%s", current_theatre->giveMeAPrettyListOfAllActorsOrDevices(false).c_str());
	IMGUI::End();
}

void GraphXConsole::displayTheatreData()
{
	if(current_theatre == nullptr || current_theatre->name == std::string("Untitled Theatre"))
		return;
	std::string this_name = current_theatre->name + " - GraphXTheatre Data";
	IMGUI::SetNextWindowSize(ImVec2(635, 520), ImGuiCond_Once);
	IMGUI::SetNextWindowPos(ImVec2(600, 175), ImGuiCond_Once);
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoInputs;
	if(active)
		window_flags = ImGuiWindowFlags_None;
	IMGUI::Begin(this_name.c_str(), &tertiary_active, window_flags);
	// IMGUI::Text("%s", std::string("Size: " + std::to_string(IMGUI::GetWindowSize()[0]) + ", " + std::to_string(IMGUI::GetWindowSize()[1]) + " Position: " + std::to_string(IMGUI::GetWindowPos()[0]) + ", " + std::to_string(IMGUI::GetWindowPos()[1])).c_str());
	IMGUI::Text("%s", current_theatre->theatre_file_data_printout.c_str());
	IMGUI::End();
}