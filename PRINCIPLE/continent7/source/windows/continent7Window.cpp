// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include <cstdio>
#include "principle/deps/imgui/imgui.h"
#include "continent7/application.hpp"
#include "thoreau/thoreau.hpp"

continent7::Application::Continent7Window::Continent7Window(
	Application* application) :
		principle::Window(application)
{
	this->application = application;

	set_can_close(false);
}

std::string continent7::Application::Continent7Window::get_title() const
{
	return "CONTINENT7";
}

void continent7::Application::Continent7Window::update_gui()
{
	ImGui::Text("Operations");
	ImGui::Separator();
	if (ImGui::Button("Find Obstacles"))
	{
		this->application->toolbox.push_undo_stack();
		this->application->apply_tile_heuristic();
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Apply obstacle heuristic to tiles in current layer.");
	}

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
	if (ImGui::Button("Undo"))
	{
		this->application->toolbox.undo();
	}
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Undo stack size: %lu", this->application->toolbox.get_undo_stack_size());
	}

	ImGui::Checkbox("Automatic Extraction", &this->application->automatic_enabled);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Automatically perform tile heuristic and don't capture trace data.");
	}

	ImGui::Text("Tool Settings");
	ImGui::Separator();
	ImGui::InputInt("Current Layer", &this->application->current_layer);
	ImGui::Checkbox("Override Layer", &this->application->override_layer);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Use the current layer for all operations, regardless of source.");
	}
	ImGui::InputFloat("Elevation Deviation", &this->application->max_elevation_deviation);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Maximum deviation allowed between neighboring tiles.");
	}
	ImGui::InputFloat("Elevation Range", &this->application->tile_elevation_range);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(
			"Tile elevation will be assigned this min/max range. "
			"If the range is too small, then layer finding will fail.");
	}
	ImGui::Checkbox("Make Ray Hits Passable", &this->application->make_passable);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Force tiles hit by rays to be passable.");
	}
	ImGui::Checkbox("Restrain Tag", &this->application->restrain_tag);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Restrain recursive ray hits to single tag.");
	}
	ImGui::Checkbox("Player Sector Only", &this->application->player_sector_only);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(
			"Only extract tiles within player's current sector.\n"
			"Doesn't apply if the minimap isn't visible.");
	}
	ImGui::Checkbox("Adjacent Sectors", &this->application->include_adjacent);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(
			"Also include adjacent sectors to the player's current sector.\n"
			"Requires 'Player Sector Only' set.");
	}
	ImGui::InputFloat3("Token Size", &this->application->token_size.x);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(
			"Virtual token half-size, for finding impassable tiles.\n"
			"A box with the dimensions (2*x, 2*y, 2*z) will be collided\n"
			"with the scene at each tile center to determine if a tile is\n"
			" passable or not.");
	}
	ImGui::InputFloat("Token Elevation", &this->application->token_elevation);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip(
			"Virtual token elevation offset.\n"
			"The actual elevation offset is reduced by the Y-value of\n"
			"the half-size.");
	}
	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Sectors"))
	{
		ImGui::BeginGroup();
		if (this->application->visible_sectors.empty())
		{
			ImGui::Text("(no sectors)");
		}
		else
		{
			for (auto& sector: this->application->visible_sectors)
			{
				ImGui::PushID(&sector.first);

				ImGui::Text(
					"%d, %d; %d: (%d, %d) through (%d, %d)",
					sector.first.x, sector.first.y, sector.first.layer,
					sector.first.x * thoreau::SECTOR_SIZE,
					sector.first.y * thoreau::SECTOR_SIZE,
					(sector.first.x + 1) * thoreau::SECTOR_SIZE,
					(sector.first.y + 1) * thoreau::SECTOR_SIZE);

				if (ImGui::Button("Elevations"))
				{
					// TODO: Open PreviewWindow
				}
				if (ImGui::IsItemHovered())
				{
					auto iter = this->application->sector_elevation_textures.find(sector.first);
					if (iter != this->application->sector_elevation_textures.end())
					{
						ImGui::BeginTooltip();
						ImGui::Image(
							iter->second.get(),
							ImVec2(iter->second->get_width(), iter->second->get_height()));
						ImGui::EndTooltip();
					}
				}

				ImGui::SameLine();
				if (ImGui::Button("Walls"))
				{
					// TODO: Open PreviewWindow
				}
				if (ImGui::IsItemHovered())
				{
					auto iter = this->application->sector_wall_textures.find(sector.first);
					if (iter != this->application->sector_wall_textures.end())
					{
						ImGui::BeginTooltip();
						ImGui::Image(
							iter->second.get(),
							ImVec2(iter->second->get_width(), iter->second->get_height()));
						ImGui::EndTooltip();
					}
				}

				ImGui::PopID();
			}
		}
		ImGui::EndGroup();
	}

	if (ImGui::CollapsingHeader("Tools"))
	{
		ImGui::BulletText("P: Mark tile passable");
		ImGui::BulletText("I: Mark tile impassable");
		ImGui::BulletText("M: Set elevation from ray");
		ImGui::BulletText("N: Set elevation from ray, recursive");
		ImGui::BulletText("F: Edit single tile properties, clearing existing selection");
		ImGui::BulletText("Shift+F: Edit tile properties, adding to existing selection");
		ImGui::BulletText("C: View Continent1 candidates on current tile.");
	}
}

void continent7::Application::Continent7Window::import_map()
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
		auto& map = this->application->get_game_manager().get_map();
		map.clear();

		std::ifstream stream(map_path);
		std::size_t count = map.load(stream);

		if (stream.bad())
		{
			std::fprintf(stderr, "couldn't load map '%s'\n", map_path.c_str());
		}
		else
		{
			std::fprintf(stderr, "loaded %lu tiles from map '%s'\n", count, map_path.c_str());
		}
	}
}
