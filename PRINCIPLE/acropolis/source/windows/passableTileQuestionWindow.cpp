// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <algorithm>
#include "acropolis/application.hpp"
#include "principle/deps/imgui/imgui.h"

acropolis::Application::PassableTileQuestionWindow::PassableTileQuestionWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
}

void acropolis::Application::PassableTileQuestionWindow::open()
{
	auto& candidate_manager = this->application->candidate_manager;
	for (auto& i: this->application->visible_tile_state)
	{
		if (candidate_manager.space_begin(i.first) == candidate_manager.space_end(i.first))
		{
			continue;
		}

		this->tiles.emplace_back(i.first.layer, i.first.x, i.first.y);
		std::sort(this->tiles.begin(), this->tiles.end());
	}
	this->current_index = 0;

	if (!this->tiles.empty())
	{
		this->is_dirty = true;
		principle::Window::open();
	}
}

std::string acropolis::Application::PassableTileQuestionWindow::get_title() const
{
	return "Passable Tile Questionnaire";
}

void acropolis::Application::PassableTileQuestionWindow::update_gui()
{
	if (this->is_dirty)
	{
		update_views();
		this->is_dirty = false;
	}

	if (this->current_index >= tiles.size())
	{
		ImGui::Text("Done!");

		if (ImGui::Button("Save Results"))
		{
			auto filename = this->application->get_output_filename("tile_results.csv");
			if (!filename.empty())
			{
				this->application->save_tiles(filename);
			}
		}

		ImGui::Spacing();
		ImGui::Separator();

		if (ImGui::Button("Close"))
		{
			close();
		}
	}
	else
	{
		auto tile_coordinate = thoreau::TileCoordinate(
			std::get<1>(this->tiles[this->current_index]),
			std::get<2>(this->tiles[this->current_index]),
			std::get<0>(this->tiles[this->current_index]));

		ImGui::Text("Tile (%d, %d; %d)", tile_coordinate.x, tile_coordinate.y, tile_coordinate.layer);
		ImGui::Text("Is this tile passable?");

		bool made_choice = false;
		if (ImGui::Button("Yes"))
		{
			this->application->impassable_tiles.erase(tile_coordinate);
			this->application->passable_tiles.insert(tile_coordinate);
			made_choice = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			this->application->passable_tiles.erase(tile_coordinate);
			this->application->impassable_tiles.insert(tile_coordinate);
			made_choice = true;
		}

		ImGui::SameLine();
		if (ImGui::Button("Skip"))
		{
			this->application->passable_tiles.erase(tile_coordinate);
			this->application->impassable_tiles.erase(tile_coordinate);
			made_choice = true;
		}

		if (made_choice)
		{
			++this->current_index;
			update_views();
		}

		ImGui::Spacing();
		ImGui::Separator();

		ImGui::Text(
			"%5lu of %5lu (%0.2f%% complete)",
			this->current_index,
			this->tiles.size(),
			this->current_index * 100.0f / this->tiles.size());

		if (this->current_index > 0)
		{
			if (ImGui::Button("Previous"))
			{
				--this->current_index;
				update_views();
			}
		}
	}
}

void acropolis::Application::PassableTileQuestionWindow::update_views()
{
	if (this->current_index < this->tiles.size())
	{
		auto tile_coordinate = thoreau::TileCoordinate(
			std::get<1>(this->tiles[this->current_index]),
			std::get<2>(this->tiles[this->current_index]),
			std::get<0>(this->tiles[this->current_index]));

		this->application->scene_view->focus(tile_coordinate);
		this->application->tile_view->show(tile_coordinate);
	}
}
