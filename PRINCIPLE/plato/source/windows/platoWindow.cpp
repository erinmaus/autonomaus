// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include <cstdio>
#include "principle/deps/imgui/imgui.h"
#include "plato/application.hpp"
#include "thoreau/thoreau.hpp"

plato::Application::PlatoWindow::PlatoWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;

	set_can_close(false);
}

std::string plato::Application::PlatoWindow::get_title() const
{
	return "PLATO";
}

void plato::Application::PlatoWindow::update_gui()
{
	ImGui::Text("Operations");
	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Text("Map:");
	ImGui::SameLine();
	if (ImGui::Button("Import"))
	{
		import_map();
	}

	ImGui::SameLine();
	if (ImGui::Button("Export"))
	{
		this->application->export_window.open();
	}

	ImGui::SameLine();
	ImGui::Text("Database:");

	ImGui::SameLine();
	if (ImGui::Button("Open"))
	{
		load_database(true);
	}

	ImGui::SameLine();
	if (ImGui::Button("Create"))
	{
		load_database(false);
	}

	ImGui::SameLine();
	ImGui::Text("DAT:");
	ImGui::SameLine();
	if (ImGui::Button("Import###dat"))
	{
		this->application->import_dat_window.open();
	}

	int layer = this->application->current_layer;
	if (ImGui::InputInt("Layer", &layer))
	{
		if (layer >= thoreau::TileCoordinate::MIN_LAYER &&
			layer < thoreau::TileCoordinate::MAX_LAYER)
		{
			this->application->change_layer(layer);
		}
	}

	if (ImGui::Button("Load Layer"))
	{
		auto& map = this->application->get_game_manager().get_map();
		thoreau::TileCoordinate current_tile_coordinate;
		if (map.get_first_tile(current_tile_coordinate))
		{
			do
			{
				if (current_tile_coordinate.layer == this->application->current_layer)
				{
					this->application->visible_tile_state.add(current_tile_coordinate) = true;
				}
			} while (map.get_next_tile(current_tile_coordinate, current_tile_coordinate));
		}

		this->application->load_tiles();

		if (this->application->current_layer == 0)
		{
			this->application->camera_offset.x = -(50 * thoreau::SECTOR_SIZE);
			this->application->camera_offset.x += this->application->get_display()->get_width() / 2.0f;
			this->application->camera_offset.y = -(50 * thoreau::SECTOR_SIZE);
			this->application->camera_offset.y += this->application->get_display()->get_height() / 2.0f;
		}
	}

	ImGui::Spacing();
	ImGui::Text("View");
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Checkbox("Draw Walls", &this->application->draw_walls);
	ImGui::Checkbox("Draw Elevations", &this->application->draw_elevations);
	ImGui::Checkbox("Draw Atlas", &this->application->draw_atlas);
}

void plato::Application::PlatoWindow::import_map()
{
	std::string map_path;
	{
		auto data_path = std::getenv("NECRONOMICON_DATA_PATH");
		if (data_path != nullptr)
		{
			map_path = this->application->get_input_filename(std::string(data_path) + "/map");
		}
		else
		{
			map_path = this->application->get_input_filename();
		}
	}

	if (!map_path.empty())
	{
		this->application->load_map(map_path);
	}
}

void plato::Application::PlatoWindow::load_database(bool open)
{
	std::string path;
	{
		auto data_path = std::getenv("NECRONOMICON_DATA_PATH");
		if (data_path != nullptr)
		{
			std::string suggestion = std::string(data_path) + "/database";
			if (open)
			{
				path = this->application->get_input_filename(suggestion);
			}
			else
			{
				path = this->application->get_output_filename(suggestion);
			}
		}
		else
		{
			if (open)
			{
				path = this->application->get_input_filename();
			}
			else
			{
				path = this->application->get_output_filename();
			}
		}
	}

	if (!path.empty())
	{
		try
		{
			this->application->load_brochure(path);
		}
		catch (const std::exception& e)
		{
			std::fprintf(stderr, "couldn't load %s:\n\t%s\n", path.c_str(), e.what());
		}
	}
}
