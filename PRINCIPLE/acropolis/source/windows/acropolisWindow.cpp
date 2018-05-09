// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "acropolis/application.hpp"
#include "principle/deps/imgui/imgui.h"
#include "thoreau/thoreau.hpp"

acropolis::Application::AcropolisWindow::AcropolisWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
	set_can_close(false);
}

std::string acropolis::Application::AcropolisWindow::get_title() const
{
	return "ACROPOLIS";
}

void acropolis::Application::AcropolisWindow::update_gui()
{
	int x = this->tile_x;
	if (ImGui::InputInt("Focus X", &x) && x >= 0)
	{
		this->tile_x = x;
	}

	int y = this->tile_y;
	if (ImGui::InputInt("Focus Y", &y) && y >= 0)
	{
		this->tile_y = y;
	}

	int layer = this->tile_layer;
	if (ImGui::InputInt("Focus Layer", &layer) && layer >= 0)
	{
		this->tile_layer = layer;
	}

	if (ImGui::Button("Focus"))
	{
		auto tile_coordinate = thoreau::TileCoordinate(x, y, layer);
		this->application->scene_view->focus(tile_coordinate);
		this->application->tile_view->show(tile_coordinate);
	}

	ImGui::Spacing();
	ImGui::Separator();

	if (ImGui::Button("Apply Heuristic"))
	{
		this->application->apply_tile_heuristic();
		this->application->show_tiles = show_impassable_tiles;
	}

	ImGui::SameLine();
	if (ImGui::Button("Passable Questionnaire"))
	{
		this->application->show_tiles = show_focus;
		this->application->passable_tile_question_window.open();
	}
}