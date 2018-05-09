// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/graphics/drawParameters.hpp"
#include "principle/deps/imgui/imgui.h"
#include "thoreau/thoreau.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "zeitgeist/application.hpp"

zeitgeist::Application::QueryResultsWindow::QueryResultsWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
}

void zeitgeist::Application::QueryResultsWindow::add_world(
	const thoreau::WorldCoordinate& location)
{
	this->world_matches.insert(location);
}

void zeitgeist::Application::QueryResultsWindow::add_tile(
	const thoreau::TileCoordinate& location)
{
	this->tile_matches.insert(location);
}

void zeitgeist::Application::QueryResultsWindow::clear()
{
	this->world_matches.clear();
	this->tile_matches.clear();
}

std::string zeitgeist::Application::QueryResultsWindow::get_title() const
{
	return "Query Results";
}

void zeitgeist::Application::QueryResultsWindow::update_gui()
{
	auto& map = this->application->get_game_manager().get_runtime_map();
	auto& tile_elevations = this->application->get_game_manager().get_terrain_elevation_state();
	auto& actor_draws = this->application->get_game_manager().get_actor_draw_state();
	auto& draw_manager = this->application->get_game_manager().get_draw_manager();

	if (ImGui::Button("Clear"))
	{
		clear();
	}

	ImGui::SameLine();
	if (ImGui::Button("Select All Actors"))
	{
		for (auto& location: this->world_matches)
		{
			if (!actor_draws.has(location))
			{
				continue;
			}

			auto& actor_draw = actor_draws.get(location);
			for (auto i = actor_draw.tags_begin(); i != actor_draw.tags_end(); ++i)
			{
				this->application->select(*i);
			}
		}
	}

	ImGui::Text("Animated");
	int id = 0;
	for (auto location: this->world_matches)
	{
		ImGui::PushID(id);
		ImGui::Text("(%04d, %04d; %- 3d)", location.x, location.y, location.layer);

		ImGui::SameLine();
		if (ImGui::Button("Focus"))
		{
			auto tile_coordinate = thoreau::tile_from_world(location);
			tile_coordinate.layer = 0;

			bool focused = false;
			if (tile_elevations.has(tile_coordinate))
			{
				auto& elevations = tile_elevations.get(tile_coordinate);
				if (elevations.has(location.layer))
				{
					float x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
					float y = elevations.get_max(location.layer);
					float z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

					this->application->camera.focus(glm::vec3(x, y, z));
					focused = true;
				}
			}

			if (!focused && actor_draws.has(location))
			{
				auto& silhouette = actor_draws.get(location);
				auto tag = *silhouette.tags_begin();
				if (draw_manager.has(tag))
				{
					auto& draw = draw_manager.get(tag);
					this->application->camera.focus(draw.get_translation());
				}
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Select"))
		{
			if (!actor_draws.has(location))
			{
				continue;
			}

			auto& silhouette = actor_draws.get(location);
			for (auto i = silhouette.tags_begin(); i != silhouette.tags_end(); ++i)
			{
				this->application->select(*i);
			}
		}

		ImGui::PopID();
		++id;
	}

	ImGui::Text("Static");
	for (auto location: this->tile_matches)
	{
		ImGui::PushID(id);

		ImGui::Text("(%04d, %04d; %- 3d)", location.x, location.y, location.layer);

		ImGui::SameLine();
		if (ImGui::Button("Focus"))
		{
			if (map.has_tile(location))
			{
				auto& tile = map.get_tile(location);
				if (tile.has_elevation())
				{
					float x = (location.x + 0.5f) * thoreau::TILE_SIZE;
					float y = tile.get_max_elevation();
					float z = (location.y + 0.5f) * thoreau::TILE_SIZE;

					this->application->camera.focus(glm::vec3(x, y, z));
				}
			}
		}

		ImGui::PopID();
		++id;
	}

	if (ImGui::BeginPopup(get_id("confirm_clear").c_str()))
	{
		ImGui::Text("Are you sure you wish to clear the results?");
		if (ImGui::Button("Yes"))
		{
			this->world_matches.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}
