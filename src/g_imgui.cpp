#include "g_imgui.hpp"
#include "g_common.hpp"
#include "imgui_internal.h"
#include "imgui_stdlib.h"
#include "models.hpp"

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

	if(tertiary_active)
		displayTheatreData();

	if(!active)
	{
		graphx::debug::actor_debug_menu_open = keep_debug_labels_on;
		return;
	}

	was_active = active;
#ifndef GRAPHX_DEBUG
	IMGUI::SetNextWindowPos(ImVec2(777, 60), ImGuiCond_Once);
	IMGUI::SetNextWindowSize(ImVec2(484, 434), ImGuiCond_Once);
#endif
	IMGUI::Begin(name.c_str(), &active, ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus);
	IMGUI::Text("%s", std::string("Currently Loaded Theatre: " + current_theatre->name).c_str());
	IMGUI::Separator();
	if(IMGUI::Button("Toggle GraphXTheatre Printout"))
		tertiary_active = !tertiary_active;
	IMGUI::Checkbox("Keep Debug Labels On", &keep_debug_labels_on);
	IMGUI::NewLine();
	IMGUI::NewLine();
	graphx::debug::actor_debug_menu_open = true;
	IMGUI::Text("Actor Debugger");
	IMGUI::Separator();
	IMGUI::SliderFloat("Debug Label Size", &graphx::debug::actor_debug_menu_text_scale, 0.0f, 10.0f, "%.2f");
	IMGUI::Text("Select Actor by Name:");
	IMGUI::PushItemWidth(100.0f);
	IMGUI::InputText("##1", &actor_name_selection);
	IMGUI::PopItemWidth();
	IMGUI::SameLine();
	if(IMGUI::Button("Get Actor!##1"))
	{
		actor_selection_made = true;
		actor_selection_valid = !(actor_name_selection.empty() || current_theatre->getActor(actor_name_selection) == nullptr);
		if(!actor_selection_valid)
			error_string = "Invalid Actor Name!";
		else
			actor = current_theatre->getActor(actor_name_selection);
	}
	IMGUI::Text("Select Actor by UID:");
	IMGUI::PushItemWidth(100.0f);
	IMGUI::InputText("##2", &actor_uid_selection);
	IMGUI::PopItemWidth();
	IMGUI::SameLine();
	if(IMGUI::Button("Get Actor!##2"))
	{
		actor_selection_made = true;
		actor_selection_valid = true;
		try
		{
			std::stod(actor_uid_selection);
		}
		catch (std::invalid_argument const &exception)
		{
			PRINTERR("GraphXConsole::displayActorDebugger - std::stod(actor_uid_selection) threw an exception!");
			actor_selection_valid = false;
		}

		if(actor_selection_valid && current_theatre->getActor(std::stod(actor_uid_selection)) != nullptr)
			actor = current_theatre->getActor(std::stod(actor_uid_selection));
		else
		{
			actor_selection_valid = false;
			error_string = "Invalid Actor UID!";
		}
	}

	if(actor_selection_made && actor_selection_valid)
	{
		IMGUI::Text("Actor Info");
		IMGUI::Separator();
		IMGUI::Text(std::string("Name: " + actor->getName()).c_str(), "%s");
		IMGUI::Text(std::string("Type: " + std::string(actor->getType().name)).c_str(), "%s");
		IMGUI::Text(std::string("UID: "  + std::to_string(actor->getUID())).c_str(), "%s");
		IMGUI::Separator();
		if(IMGUI::Button("Toggle Visibility"))
			actor->visible = !actor->visible;
		std::vector<float> position_scalars = actor->getPosition<std::vector<float>>();
		std::vector<float> rotation_scalars = actor->getRotationDegrees<std::vector<float>>();
		std::vector<float> scale_scalars = {actor->scale.x, actor->scale.y, actor->scale.z};
		if(IMGUI::DragFloat3("Position", position_scalars.data(), -0.1f, -100.0f, 100.0f))
			actor->setGlobalPosition(glm::vec3(position_scalars[0], position_scalars[1], position_scalars[2]));
		if(IMGUI::DragFloat3("Rotation", rotation_scalars.data(), -0.1f, -100.0f, 100.0f))
			actor->setGlobalRotation(glm::radians(glm::vec3(rotation_scalars[0], rotation_scalars[1], rotation_scalars[2])));
		if(IMGUI::DragFloat3("Scale", scale_scalars.data(), -0.1f, -100.0f, 100.0f))
			actor->scale = glm::vec3(scale_scalars[0], scale_scalars[1], scale_scalars[2]);
		// Todo: finish this shit, lol
		error_string = "";
	}
	if(!actor_selection_valid)
	{
		IMGUI::Text(error_string.c_str(), "%s");
	}
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

std::string toStringNice(float number)
{
	std::string buffer = "";
	if(number >= 0)
		buffer += " ";
	buffer += std::to_string(number).substr(0, 4);
	return buffer;
}