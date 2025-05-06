#include "g_imgui.hpp"
// #include "g_devices.hpp"
#include "g_actor.hpp"
#include "g_theatre.hpp"
#include "graphx_namespace.hpp"
#include "imgui_stdlib.h"
#include "sanity_printouts.hpp"
#include <models.hpp>

namespace IMGUI = ImGui;

//
// GraphXConsole
//
GraphXConsole::GraphXConsole()
: active(false), secondary_active(false), tertiary_active(false), quaternary_active(false), toggle_activation_key(ImGuiKey_Tab), name("GraphX Debug Console"), was_active(false)
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
		displayTheatrePrintout();

	if(tertiary_active)
		liveTheatreEditor();

	if(quaternary_active)
		exportTheatreFile();

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
	IMGUI::Text("%s", std::string("Currently Loaded Theatre: " + graphx::current::theatre.name).c_str());
	IMGUI::Separator();
	if(IMGUI::Button("Toggle GraphXTheatre Printout"))
		secondary_active = !secondary_active;
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
		actor_selection_valid = !(actor_name_selection.empty() || graphx::current::theatre.getActor<Actor>(actor_name_selection) == nullptr);
		if(!actor_selection_valid)
			error_string = "Invalid Actor Name!";
		else
			single_actor = graphx::current::theatre.getActor<Actor>(actor_name_selection);
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

		if(actor_selection_valid && graphx::current::theatre.getActor<Actor>(std::stod(actor_uid_selection)) != nullptr)
			single_actor = graphx::current::theatre.getActor<Actor>(std::stod(actor_uid_selection));
		else
		{
			actor_selection_valid = false;
			error_string = "Invalid Actor UID!";
		}
	}

	if(actor_selection_made && actor_selection_valid)
	{
		error_string = "";
		IMGUI::NewLine();
		showActorEditor(single_actor);
	}
	if(!actor_selection_valid)
	{
		IMGUI::Text(error_string.c_str(), "%s");
	}
	IMGUI::Separator();
	if(IMGUI::Button("Open live Theatre editor (Warning: performance will go down!)"))
		tertiary_active = true;
	if(IMGUI::Button("Export current Theatre state as a GraphXTheatre (.gt) file"))
		quaternary_active = true;
	IMGUI::End();
}

std::string indexMe(const std::string string, const int index)
{
	return (string + "##" + std::to_string(index));
}

void GraphXConsole::showActorEditor(std::shared_ptr<Actor> actor, int index)
{
	IMGUI::BeginGroup();
	//
	//  Actor info
	//
	IMGUI::TextColored(glm::vec4(0.7f, 0.8f, 1.0f, 0.8f), "Actor");
	IMGUI::SameLine();
	IMGUI::Text(std::string(actor->getTypeName() + " \"" + actor->name + "\" (UID: " + std::to_string(actor->getUID()) + ")").c_str(), "%s");
	IMGUI::Separator();
	//
	//  Actor visibility
	//
	if(IMGUI::Button(indexMe("Toggle Visibility", index).c_str()))
		actor->visible = !actor->visible;
	IMGUI::SameLine();
	//
	//  Reset Actor
	//
	if(IMGUI::Button(indexMe("Reset Actor", index).c_str()))
		actor->loadSettings();
	//
	//  Actor rotation, position, and scale manipulation
	//
	std::vector<float> position_vectors = gmath::glmVectorToFloats(actor->getGlobalPosition());
	std::vector<float> rotation_vectors = gmath::glmVectorToFloats(actor->getGlobalEulerAngles(true));
	std::vector<float> scale_vectors = gmath::glmVectorToFloats(actor->getGlobalScale());
	if(IMGUI::DragFloat3(indexMe("Position", index).c_str(), position_vectors.data(), -0.1f, -100.0f, 100.0f))
		actor->setGlobalPosition(glm::vec3(position_vectors[0], position_vectors[1], position_vectors[2]));
	if(IMGUI::DragFloat3(indexMe("Rotation", index).c_str(), rotation_vectors.data(), -0.1f, -100.0f, 100.0f))
		actor->setGlobalEulerAngles(glm::vec3(rotation_vectors[0], rotation_vectors[1], rotation_vectors[2]), true);
	if(IMGUI::DragFloat3(indexMe("Scale", index).c_str(), scale_vectors.data(), -0.1f, -100.0f, 100.0f))
		actor->setGlobalScale({scale_vectors[0], scale_vectors[1], scale_vectors[2]});
	IMGUI::EndGroup();
	if(IMGUI::IsItemHovered())
		actor->debug_highlight_enabled = true;
	else
		actor->debug_highlight_enabled = false;
}

void GraphXConsole::liveTheatreEditor()
{
	std::vector<std::shared_ptr<Actor>> troupe = graphx::current::theatre.getAllActors();
	IMGUI::Begin("Live Theatre Editor", &tertiary_active);
	for(int i = 0 ; i < troupe.size() ; i++) // AYO I THINK THAT THE TROUPE IS GETTING BLOATED AS FUCK MY GUY
	{
		if(i != 0) IMGUI::NewLine();
		showActorEditor(troupe.at(i), i);
	}
	IMGUI::End();
}

void GraphXConsole::exportTheatreFile()
{
	// GraphXTheatreInterpreter::gStringSettings init_settings = graphx::current::theatre.graphx_theatre_settings;
	IMGUI::Begin("Export Theatre", &quaternary_active);

	IMGUI::End();
}

void GraphXConsole::displayTheatrePrintout()
{
	if(!graphx::current::theatre.name.compare("Untitled Theatre"))
		return;
	std::string this_name = graphx::current::theatre.name + " - Parsed GraphXTheatre Settings";
#ifndef GRAPHX_DEBUG
	IMGUI::SetNextWindowSize(ImVec2(635, 520), ImGuiCond_Once);
	IMGUI::SetNextWindowPos(ImVec2(600, 175), ImGuiCond_Once);
#endif
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoInputs;
	if(active)
		window_flags = ImGuiWindowFlags_None;
	IMGUI::Begin(this_name.c_str(), &secondary_active, window_flags);
	// IMGUI::Text("%s", graphx::current::theatre.theatre_file_data_printout.c_str());
	IMGUI::End();
}