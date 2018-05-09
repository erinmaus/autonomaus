// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstring>
#include <fstream>
#include "mantorok/io/datWriter.hpp"
#include "chomsky/io/world/query/tileSpacePointCloudMatch.hpp"
#include "chomsky/world/query/actorTileSpacePointCloudMatchExecutor.hpp"
#include "chomsky/world/query/staticTileSpacePointCloudMatchExecutor.hpp"
#include "principle/deps/imgui/imgui.h"
#include "zeitgeist/application.hpp"

zeitgeist::Application::PointCloudMatchBuilderWindow::PointCloudMatchBuilderWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
}

void zeitgeist::Application::PointCloudMatchBuilderWindow::add(
	const chomsky::TileSpacePointCloudMatch& match)
{
	PointCloudMatch result;
	result.name = std::to_string(matches.size() + 1);
	result.value = match;

	this->matches.push_back(result);
}

void zeitgeist::Application::PointCloudMatchBuilderWindow::add(
	const thoreau::WorldCoordinate& location)
{
	auto& actor_point_cloud_state = this->application->get_game_manager().get_actor_point_cloud_state();
	if (!actor_point_cloud_state.has(location))
	{
		return;
	}

	PointCloudMatch match;
	match.name = std::to_string(matches.size() + 1);

	auto& point_cloud = actor_point_cloud_state.get(location);
	for (auto& i: *point_cloud)
	{
		auto offset = i.first;
		auto& tile_point_cloud = i.second;

		match.value.set(offset, tile_point_cloud);
	}

	this->matches.push_back(match);
	open();
}

std::string zeitgeist::Application::PointCloudMatchBuilderWindow::get_title() const
{
	return "Point Cloud Match Builder";
}

void zeitgeist::Application::PointCloudMatchBuilderWindow::update_gui()
{
	if (ImGui::Button("Clear"))
	{
		ImGui::OpenPopup(get_id("confirm_clear").c_str());
	}

	ImGui::Text("Queries");

	int id = 0;
	int pending_removal = -1;
	for (auto& match: this->matches)
	{
		ImGui::PushID(id);

		char name[256];
		std::strncpy(name, match.name.c_str(), sizeof(name));
		if (ImGui::InputText("Name", name, sizeof(name)))
		{
			match.name = name;
		}

		if (ImGui::Button("Export"))
		{
			auto filename = this->application->get_output_filename();
			if (!filename.empty())
			{
				std::ofstream stream(filename, std::ios::binary);
				mantorok::DATWriter writer(stream);
				mantorok::DATSerializer<chomsky::TileSpacePointCloudMatch>::serialize(
					match.value, writer);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Test (Animated)"))
		{
			this->application->query_results_window.clear();

			chomsky::ActorTileSpacePointCloudMatchExecutor executor(
				&this->application->get_game_manager().get_animated_block());

			bool results = false;
			for (auto& i: this->application->get_game_manager().get_actor_materials_state())
			{
				if (executor.execute(i.first, match.value))
				{
					this->application->query_results_window.add_world(i.first);
					results = true;
				}
			}

			if (!results)
			{
				ImGui::OpenPopup(get_id("query_test_failure").c_str());
			}
			else
			{
				this->application->query_results_window.open();
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Test (Static)"))
		{
			this->application->query_results_window.clear();

			chomsky::StaticTileSpacePointCloudMatchExecutor executor(
				&this->application->get_game_manager().get_static_block());

			bool results = false;
			for (auto& i: this->application->get_game_manager().get_static_point_cloud_state())
			{
				if (executor.execute(i.first, match.value))
				{
					this->application->query_results_window.add_tile(i.first);
					results = true;
				}
			}

			if (!results)
			{
				ImGui::OpenPopup(get_id("query_test_failure").c_str());
			}
			else
			{
				this->application->query_results_window.open();
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Remove"))
		{
			pending_removal = id;
		}

		ImGui::BulletText("Width: %d", match.value.get_width() + 1);
		ImGui::BulletText("Height: %d", match.value.get_height() + 1);
		ImGui::BulletText("Depth: %d", match.value.get_depth() + 1);

		ImGui::PopID();
		++id;
	}

	if (pending_removal >= 0)
	{
		this->matches.erase(this->matches.begin() + pending_removal);
	}

	if (ImGui::BeginPopup(get_id("confirm_clear").c_str()))
	{
		ImGui::Text("Are you sure you wish to clear the material queries?");
		if (ImGui::Button("Yes"))
		{
			this->matches.clear();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}

	if (ImGui::BeginPopup(get_id("query_test_failure").c_str()))
	{
		ImGui::Text("No matches found while testing query.");

		ImGui::SameLine();
		if (ImGui::Button("Alright"))
		{
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}
