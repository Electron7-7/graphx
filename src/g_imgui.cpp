#include "g_imgui.hpp"
#include "g_common.hpp"
#include "r_common.hpp"

namespace IMGUI = ImGui;

//
// GraphXConsole
//
GraphXConsole::GraphXConsole()
: active(false), secondary_active(false), tertiary_active(false), fourth_active(false), toggle_activation_key(ImGuiKey_Tab), name("GraphX Debug Console"), was_active(false)
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

	// if(fourth_active)
		// displayMeshData();

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
	IMGUI::SameLine();
	if(IMGUI::Button("Toggle Mesh Data Printout"))
		fourth_active = !fourth_active;
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

/*void GraphXConsole::displayMeshData()
{
#ifndef GRAPHX_DEBUG
	IMGUI::SetNextWindowSize(ImVec2(300, 200), ImGuiCond_Once);
	IMGUI::SetNextWindowPos(ImVec2(300, 100), ImGuiCond_Once);
#endif
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoInputs;
	if(fourth_active)
		window_flags = ImGuiWindowFlags_None;

	for(auto mesh_data_pair : mesh_data_map)
	{
		if(!mesh_data_pair.second.needed_by_current_theatre)
			continue;
		IMGUI::Begin(mesh_data_pair.first.c_str(), &fourth_active, window_flags);
		IMGUI::Text("%s", getMeshData(mesh_data_pair).c_str());
		IMGUI::End();
	}
}*/

std::string toStringNice(float number)
{
	std::string buffer = "";
	if(number >= 0)
		buffer += " ";
	buffer += std::to_string(number).substr(0, 4);
	return buffer;
}

/*std::string GraphXConsole::getMeshData(std::pair<const std::string, gMeshData> &mesh_data_pair)
{
	std::string buffer = "";

	buffer += "Name: " + mesh_data_pair.first + "\n";
	buffer += "VBO: " + std::to_string(mesh_data_pair.second.VBO) + "\n";
	buffer += "Vertex Data (Postion | Normal | UV | Color):\n";

	/*for(int it = 0,uv_it = 0 ; it < mesh_data_pair.second.vertex_positions.size() ; it+=3,uv_it+=2)
	{
		buffer += "\t" + toStringNice(mesh_data_pair.second.vertex_positions[it]) + ", " + toStringNice(mesh_data_pair.second.vertex_positions[it+1]) + ", " + toStringNice(mesh_data_pair.second.vertex_positions[it+2]) + " | ";
		buffer += toStringNice(mesh_data_pair.second.vertex_normals[it]) + ", " + toStringNice(mesh_data_pair.second.vertex_normals[it+1]) + ", " + toStringNice(mesh_data_pair.second.vertex_normals[it+2]) + " | ";
		buffer += toStringNice(mesh_data_pair.second.vertex_uvs[uv_it]) + ", " + toStringNice(mesh_data_pair.second.vertex_uvs[uv_it+1]) + " | ";
		buffer += toStringNice(mesh_data_pair.second.vertex_colors[it]) + ", " + toStringNice(mesh_data_pair.second.vertex_colors[it+1]) + ", " + toStringNice(mesh_data_pair.second.vertex_colors[it+2]) + "\n";
	}*//*
	for(int it = 0 ; it < mesh_data_pair.second.getVertexDataSize() ; it+=11)
	{
		buffer += "\t" + toStringNice(mesh_data_pair.second.vertex_positions[it]) + ", " + toStringNice(mesh_data_pair.second.vertex_positions[it+1]) + ", " + toStringNice(mesh_data_pair.second.vertex_positions[it+2]) + " | ";
		buffer += toStringNice(mesh_data_pair.second.vertex_normals[it+3]) + ", " + toStringNice(mesh_data_pair.second.vertex_normals[it+4]) + ", " + toStringNice(mesh_data_pair.second.vertex_normals[it+5]) + " | ";
		buffer += toStringNice(mesh_data_pair.second.vertex_uvs[it+6]) + ", " + toStringNice(mesh_data_pair.second.vertex_uvs[it+7]) + " | ";
		buffer += toStringNice(mesh_data_pair.second.vertex_colors[it+8]) + ", " + toStringNice(mesh_data_pair.second.vertex_colors[it+9]) + ", " + toStringNice(mesh_data_pair.second.vertex_colors[it+10]) + "\n";
	}

	mesh_data_printouts[mesh_data_pair.first] = buffer;

	return mesh_data_printouts.at(mesh_data_pair.first);
}*/