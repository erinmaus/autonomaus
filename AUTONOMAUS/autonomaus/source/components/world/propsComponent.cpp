// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <sstream>
#include <cstring>
#include "autonomaus/state/propsProvider.hpp"
#include "autonomaus/components/world/propsComponent.hpp"
#include "autonomaus/deps/imgui/imgui.h"
#include "autonomaus/graphics/guiDrawHook.hpp"
#include "autonomaus/graphics/worldDrawHook.hpp"
#include "autonomaus/state/mapProvider.hpp"

autonomaus::PropsComponent::PropsComponent(Autonomaus& autonomaus) :
	Component(autonomaus)
{
	// Nothing.
}

void autonomaus::PropsComponent::show()
{
	this->visible = true;
}

void autonomaus::PropsComponent::reload()
{
	this->props_draws.clear();
}

void autonomaus::PropsComponent::update()
{
	if (this->visible)
	{
		draw();
	}
}

std::string autonomaus::PropsComponent::get_name() const
{
	return "Props";
}

void autonomaus::PropsComponent::draw()
{
	ImGui::Begin("Props", &this->visible);
	{
		ImGui::BeginGroup();
		{
			char input[256] = {};
			std::strncpy(input, this->pending_definition.c_str(), sizeof(input));
			if (ImGui::InputText("Definition", input, sizeof(input)))
			{
				this->pending_definition = input;
			}

			ImGui::SameLine();
			if (ImGui::Button("Add") && !this->pending_definition.empty())
			{
				if (this->definitions.count(this->pending_definition) == 0)
				{
					PropsProvider::add_definition(
						get_autonomaus().get_game_state(),
						get_autonomaus().get_props_state(),
						this->pending_definition);
					this->definitions.insert(this->pending_definition);
				}

				this->pending_definition.clear();
			}
		}
		ImGui::EndGroup();

		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Definitions");
		ImGui::BeginGroup();
		{
			std::string definition_pending_removal;
			for (auto& definition: this->definitions)
			{
				ImGui::PushID(definition.c_str());

				ImGui::Bullet();
				if (ImGui::SmallButton("-"))
				{
					PropsProvider::remove_definition(
						get_autonomaus().get_game_state(),
						get_autonomaus().get_props_state(),
						definition);
					definition_pending_removal = definition;
				}
				ImGui::SameLine();
				ImGui::Text("%s", definition.c_str());

				ImGui::PopID();
			}

			if (!definition_pending_removal.empty())
			{
				this->definitions.erase(definition_pending_removal);
			}
		}
		ImGui::EndGroup();
	}
	ImGui::End();

	draw_props();
}

void autonomaus::PropsComponent::draw_props()
{
	auto& props = *get_autonomaus().get_props_state();
	auto& map = get_autonomaus().get_map_state()->get_map();

	std::set<thoreau::TileCoordinate> unvisited;
	for (auto& i: this->props_draws)
	{
		unvisited.insert(i.first);
	}

	std::map<thoreau::TileCoordinate, std::string> names;
	for (auto& i: props)
	{
		auto name = i.get_name();
		names[i.get_tile_coordinate()] += name + "\n";
		unvisited.erase(i.get_tile_coordinate());
	}

	auto& gui = get_autonomaus().get_gui_draw_hook().before();
	for (auto& i: names)
	{
		auto tile_coordinate = i.first;

		if (!map.has_tile(tile_coordinate))
		{
			continue;
		}

		auto& tile = map.get_tile(tile_coordinate);
		if (!tile.has_elevation())
		{
			continue;
		}

		glm::vec3 world_position;
		world_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
		world_position.y = tile.get_elevation() + 512.0f;
		world_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

		auto& draw = this->props_draws[i.first];

		gui.start(draw);
		gui.draw_text(
			i.second,
			world_position + glm::vec3(8),
			glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		gui.draw_text(i.second, world_position);
		gui.stop();
	}

	for (auto i: unvisited)
	{
		this->props_draws.erase(i);
	}
}
