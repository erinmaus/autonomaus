// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <limits>
#include <glm/gtc/matrix_transform.hpp>
#include "principle/deps/imgui/imgui.h"
#include "principle/scene/staticCamera.hpp"
#include "continent7/application.hpp"
#include "kommon/graphics/image.hpp"
#include "thoreau/sectorCoordinate.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/thoreau.hpp"

continent7::Application::ExportWindow::ExportWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
}

std::string continent7::Application::ExportWindow::get_title() const
{
	return "Export Map";
}

void continent7::Application::ExportWindow::open()
{
	principle::Window::open();
	auto data_path = std::getenv("NECRONOMICON_DATA_PATH");
	if (data_path != nullptr)
	{
		this->destination = std::string(data_path) + "/map";
	}
}

void continent7::Application::ExportWindow::update_gui()
{
	if (this->destination.empty())
	{
		ImGui::Text("Destination: (none)");
	}
	else
	{
		ImGui::TextWrapped("Destination: %s", this->destination.c_str());
	}

	ImGui::SameLine();
	if (ImGui::Button("Select"))
	{
		auto result = this->application->get_output_filename(this->destination);
		if (!result.empty())
		{
			this->destination = result;
		}
	}

	ImGui::Separator();
	ImGui::Spacing();

	ImGui::Checkbox("Export Elevations", &this->export_elevation);
	ImGui::Checkbox("Export Flags", &this->export_flags);
	ImGui::Checkbox("Export Only Modified", &this->only_modified);

	ImGui::Text("Export Mode");
	ImGui::SameLine();
	ImGui::RadioButton("Merge", &this->export_mode, mode_merge);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(
			"Tile properties will be merged, "
			"with the destination map having priority.");
	}

	ImGui::SameLine();
	ImGui::RadioButton("Replace", &this->export_mode, mode_replace);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Tiles will be replaced on a sector-by-sector basis.");
	}

	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Export"))
	{
		if (!destination.empty())
		{
			export_map();
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Discard"))
	{
		close();
	}
}

void continent7::Application::ExportWindow::export_map()
{
	auto& map = this->application->get_game_manager().get_runtime_map();
	thoreau::Map destination_map;
	{
		std::ifstream stream(this->destination);
		destination_map.load(stream);
		if (stream.bad())
		{
			destination_map.clear();
		}
	}

	thoreau::TileCoordinate current_tile_coordinate;
	if (map.get_first_tile(current_tile_coordinate))
	{
		do
		{
			auto sector_coordinate = thoreau::sector_from_tile(current_tile_coordinate);

			if (this->application->player_sector_only &&
				this->application->player_sectors.count(sector_coordinate) == 0)
			{
				continue;
			}

			if (!this->application->visible_tile_state.has(current_tile_coordinate) &&
				this->application->modified_tiles.count(current_tile_coordinate) == 0)
			{
				continue;
			}

			if (this->only_modified &&
				this->application->modified_tiles.count(current_tile_coordinate) == 0)
			{
				continue;
			}

			auto& source_tile = map.get_tile(current_tile_coordinate);
			auto& destination_tile = destination_map.add_tile(current_tile_coordinate);
			if (this->export_elevation)
			{
				switch (this->export_mode)
				{
					case mode_replace:
						destination_tile.set_elevation(
							source_tile.get_min_elevation(),
							source_tile.get_max_elevation());
						break;
					case mode_merge:
						if (!destination_tile.has_elevation())
						{
							destination_tile.set_elevation(
								source_tile.get_min_elevation(),
								source_tile.get_max_elevation());
						}
						break;
				}
			}

			if (this->export_flags)
			{
				switch (this->export_mode)
				{
					case mode_replace:
						destination_tile.set_flags(source_tile.get_flags());
						break;
					case mode_merge:
						destination_tile.append_flags(source_tile.get_flags());
						break;
				}
			}

			destination_tile.set_height(source_tile.get_height());
		} while (map.get_next_tile(current_tile_coordinate, current_tile_coordinate));
	}

	std::ofstream stream(this->destination, std::ios::trunc);
	destination_map.save(stream);
	if (stream.bad())
	{
		std::fprintf(stderr, "failed to save map to '%s'\n", this->destination.c_str());
	}
	else
	{
		std::printf("saved map to '%s\n", this->destination.c_str());
	}
}
