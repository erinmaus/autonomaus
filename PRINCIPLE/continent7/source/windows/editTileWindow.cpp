// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "continent7/application.hpp"
#include "principle/deps/imgui/imgui.h"
#include "thoreau/sectorCoordinate.hpp"

continent7::Application::EditTileWindow::EditTileWindow(Application* application) :
	principle::Window(application)
{
	this->application = application;
}

void continent7::Application::EditTileWindow::close()
{
	this->tiles.clear();
	principle::Window::close();
}

std::string continent7::Application::EditTileWindow::get_title() const
{
	return "Edit Tile";
}

void continent7::Application::EditTileWindow::set_tile(
	const thoreau::TileCoordinate& tile_coordinate)
{
	auto& map = this->application->get_game_manager().get_runtime_map();
	if (!map.has_tile(tile_coordinate))
	{
		return;
	}

	this->tiles.clear();
	this->tiles.insert(tile_coordinate);

	auto& tile = map.get_tile(tile_coordinate);
	this->current_min_elevation = tile.get_min_elevation();
	this->current_max_elevation = tile.get_max_elevation();
	this->current_height = tile.get_height();
	this->current_flags = tile.get_flags();

	open();
}

void continent7::Application::EditTileWindow::add_tile(
	const thoreau::TileCoordinate& tile_coordinate)
{
	auto& map = this->application->get_game_manager().get_runtime_map();
	if (!map.has_tile(tile_coordinate))
	{
		return;
	}

	if (this->tiles.empty())
	{
		set_tile(tile_coordinate);
	}
	else
	{
		this->tiles.insert(tile_coordinate);
	}

	open();
}

void continent7::Application::EditTileWindow::update_gui()
{
	if (ImGui::InputFloat("Min Elevation", &this->current_min_elevation))
	{
		set_value([this](auto& tile) { tile.set_elevation(this->current_min_elevation, tile.get_max_elevation()); });
	}
	ImGui::SameLine();
	if (ImGui::Button("inf"))
	{
		this->current_min_elevation = -HUGE_VALF;
		set_value([this](auto& tile) { tile.set_elevation(this->current_min_elevation, tile.get_max_elevation()); });
	}

	if (ImGui::InputFloat("Max Elevation", &this->current_max_elevation))
	{
		set_value([this](auto& tile) { tile.set_elevation(tile.get_min_elevation(), this->current_max_elevation); });
	}
	ImGui::SameLine();
	if (ImGui::Button("inf"))
	{
		this->current_max_elevation = -HUGE_VALF;
		set_value([this](auto& tile) { tile.set_elevation(tile.get_min_elevation(), this->current_max_elevation); });
	}

	if (ImGui::InputFloat("Height", &this->current_height))
	{
		set_value([this](auto& tile) { tile.set_height(this->current_height); });
	}
	ImGui::SameLine();
	if (ImGui::Button("inf"))
	{
		this->current_height = -HUGE_VALF;
		set_value([this](auto& tile) { tile.set_height(this->current_height); });
	}

	int flags = 0;
	bool is_append = false, is_unset = false;

	bool tile_impassable = this->current_flags & thoreau::Tile::flag_impassable;
	if (ImGui::Checkbox("impassable", &tile_impassable))
	{
		flags |= thoreau::Tile::flag_impassable;

		if (tile_impassable)
		{
			is_append = true;
		}
		else
		{
			is_unset = true;
		}
	}

	bool tile_passable = this->current_flags & thoreau::Tile::flag_passable;
	if (ImGui::Checkbox("passable", &tile_passable))
	{
		flags |= thoreau::Tile::flag_passable;

		if (tile_passable)
		{
			is_append = true;
		}
		else
		{
			is_unset = true;
		}
	}

	bool tile_water = this->current_flags & thoreau::Tile::flag_water;
	if (ImGui::Checkbox("water", &tile_water))
	{
		flags |= thoreau::Tile::flag_water;

		if (tile_water)
		{
			is_append = true;
		}
		else
		{
			is_unset = true;
		}
	}

	ImGui::Text("Walls");
	ImGui::Indent();
	{
		bool tile_wall_left = this->current_flags & thoreau::Tile::flag_wall_left;
		if (ImGui::Checkbox("wall_left", &tile_wall_left))
		{
			flags |= thoreau::Tile::flag_wall_left;

			if (tile_wall_left)
			{
				is_append = true;
			}
			else
			{
				is_unset = true;
			}
		}

		bool tile_wall_right = this->current_flags & thoreau::Tile::flag_wall_right;
		if (ImGui::Checkbox("wall_right", &tile_wall_right))
		{
			flags |= thoreau::Tile::flag_wall_right;

			if (tile_wall_right)
			{
				is_append = true;
			}
			else
			{
				is_unset = true;
			}
		}

		bool tile_wall_top = this->current_flags & thoreau::Tile::flag_wall_top;
		if (ImGui::Checkbox("wall_top", &tile_wall_top))
		{
			flags |= thoreau::Tile::flag_wall_top;

			if (tile_wall_top)
			{
				is_append = true;
			}
			else
			{
				is_unset = true;
			}
		}

		bool tile_wall_bottom = this->current_flags & thoreau::Tile::flag_wall_bottom;
		if (ImGui::Checkbox("wall_bottom", &tile_wall_bottom))
		{
			flags |= thoreau::Tile::flag_wall_bottom;

			if (tile_wall_bottom)
			{
				is_append = true;
			}
			else
			{
				is_unset = true;
			}
		}

		bool tile_wall_diagonal = this->current_flags & thoreau::Tile::flag_wall_diagonal;
		if (ImGui::Checkbox("wall_diagonal", &tile_wall_diagonal))
		{
			flags |= thoreau::Tile::flag_wall_diagonal;

			if (tile_wall_diagonal)
			{
				is_append = true;
			}
			else
			{
				is_unset = true;
			}
		}

		ImGui::Indent();
		{
			bool tile_wall_diagonal_left = this->current_flags & thoreau::Tile::flag_wall_diagonal_left;
			if (ImGui::Checkbox("wall_diagonal_left", &tile_wall_diagonal_left))
			{
				flags |= thoreau::Tile::flag_wall_diagonal_left;

				if (tile_wall_diagonal_left)
				{
					is_append = true;
				}
				else
				{
					is_unset = true;
				}
			}

			bool tile_wall_diagonal_right = this->current_flags & thoreau::Tile::flag_wall_diagonal_right;
			if (ImGui::Checkbox("wall_diagonal_right", &tile_wall_diagonal_right))
			{
				flags |= thoreau::Tile::flag_wall_diagonal_right;

				if (tile_wall_diagonal_right)
				{
					is_append = true;
				}
				else
				{
					is_unset = true;
				}
			}
		}
		ImGui::Unindent();
	}
	ImGui::Unindent();

	ImGui::Text("Doors");
	ImGui::Indent();
	{
		bool tile_door_left = this->current_flags & thoreau::Tile::flag_door_left;
		if (ImGui::Checkbox("door_left", &tile_door_left))
		{
			flags |= thoreau::Tile::flag_door_left;

			if (tile_door_left)
			{
				is_append = true;
			}
			else
			{
				is_unset = true;
			}
		}

		bool tile_door_right = this->current_flags & thoreau::Tile::flag_door_right;
		if (ImGui::Checkbox("door_right", &tile_door_right))
		{
			flags |= thoreau::Tile::flag_door_right;

			if (tile_door_right)
			{
				is_append = true;
			}
			else
			{
				is_unset = true;
			}
		}

		bool tile_door_top = this->current_flags & thoreau::Tile::flag_door_top;
		if (ImGui::Checkbox("door_top", &tile_door_top))
		{
			flags |= thoreau::Tile::flag_door_top;

			if (tile_door_top)
			{
				is_append = true;
			}
			else
			{
				is_unset = true;
			}
		}

		bool tile_door_bottom = this->current_flags & thoreau::Tile::flag_door_bottom;
		if (ImGui::Checkbox("door_bottom", &tile_door_bottom))
		{
			flags |= thoreau::Tile::flag_door_bottom;

			if (tile_door_bottom)
			{
				is_append = true;
			}
			else
			{
				is_unset = true;
			}
		}
	}
	ImGui::Unindent();

	bool tile_portal = this->current_flags & thoreau::Tile::flag_portal;
	if (ImGui::Checkbox("portal", &tile_portal))
	{
		flags |= thoreau::Tile::flag_portal;

		if (tile_portal)
		{
			is_append = true;
		}
		else
		{
			is_unset = true;
		}
	}

	bool tile_seen = this->current_flags & thoreau::Tile::flag_seen;
	if (ImGui::Checkbox("seen", &tile_seen))
	{
		flags |= thoreau::Tile::flag_seen;

		if (tile_seen)
		{
			is_append = true;
		}
		else
		{
			is_unset = true;
		}
	}

	if (is_append)
	{
		set_value([flags](auto& tile) { tile.append_flags(flags); });
		this->current_flags |= flags;
	}
	else if (is_unset)
	{
		set_value([flags](auto& tile) { tile.unset_flags(flags); });
		this->current_flags &= ~flags;
	}

	ImGui::Text("Tiles");
	if (this->tiles.empty())
	{
		ImGui::BulletText("(empty)");
	}

	for (auto& tile_coordinate: this->tiles)
	{
		ImGui::BulletText(
			"%d, %d; %d",
			tile_coordinate.x, tile_coordinate.y,
			tile_coordinate.layer);
	}
}

void continent7::Application::EditTileWindow::set_value(
	const std::function<void(thoreau::Tile&)>& callback)
{
	auto& map = this->application->get_game_manager().get_runtime_map();
	for (auto& tile_coordinate: this->tiles)
	{
		if (!map.has_tile(tile_coordinate))
		{
			continue;
		}

		auto& tile = map.get_tile(tile_coordinate);
		callback(tile);

		this->application->modified_tiles.insert(tile_coordinate);
		this->application->dirty_sectors.insert(thoreau::sector_from_tile(tile_coordinate));
	}

	this->application->notify_map_update();
}
