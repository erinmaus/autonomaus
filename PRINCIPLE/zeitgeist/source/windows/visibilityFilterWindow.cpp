// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/deps/imgui/imgui.h"
#include "zeitgeist/application.hpp"

zeitgeist::Application::VisibilityFilterWindow::VisibilityFilterWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;

	set_can_close(false);
}

std::string zeitgeist::Application::VisibilityFilterWindow::get_title() const
{
	return "Filter Scene";
}

void zeitgeist::Application::VisibilityFilterWindow::update_gui()
{
	ImGui::BeginGroup();
	ImGui::Checkbox("Selection", &this->application->draw_selection);
	ImGui::Checkbox("Models", &this->application->draw_models);
	ImGui::Indent();
	ImGui::Checkbox("Unclassified", &this->application->draw_unclassified);
	ImGui::Checkbox("Scenery", &this->application->draw_scenery);
	ImGui::Checkbox("Props", &this->application->draw_props);
	ImGui::Checkbox("Actors", &this->application->draw_actors);
	ImGui::Unindent();
	ImGui::Checkbox("Terrain", &this->application->draw_terrain);
	ImGui::EndGroup();

	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Checkbox("Export Tile Snapshots", &this->application->export_tile_snapshots);
	if (this->application->export_tile_snapshots)
	{
		ImGui::InputInt("Layer", &this->application->export_tile_layer);
	}
}
