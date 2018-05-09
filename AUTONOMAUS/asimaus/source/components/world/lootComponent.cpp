// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <set>
#include "autonomaus/components/world/lootComponent.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"
#include "autonomaus/graphics/guiDrawHook.hpp"
#include "autonomaus/state/lootProvider.hpp"
#include "autonomaus/state/mapProvider.hpp"
#include "thoreau/thoreau.hpp"

autonomaus::LootComponent::LootComponent(Autonomaus& autonomaus) :
	Component(autonomaus)
{
	// Nothing.
}

void autonomaus::LootComponent::show()
{
	this->visible = true;
}

void autonomaus::LootComponent::update()
{
	if (this->visible)
	{
		draw();
	}
}

void autonomaus::LootComponent::reload()
{
	// Nothing.
}

std::string autonomaus::LootComponent::get_name() const
{
	return "Loot";
}

void autonomaus::LootComponent::draw()
{
	ImGui::Begin("Loot", &this->visible);
	{
		auto& loot = *get_autonomaus().get_loot_state();
		auto item_cortex = get_autonomaus().get_userdata<ItemCortex>();

		for (auto i: loot)
		{
			std::string name;
			if (item_cortex == nullptr || !item_cortex->has_name(i.second))
			{
				name = "(unknown)";
			}
			else
			{
				name = item_cortex->get_name(i.second);
			}

			ImGui::BulletText(
				"(%d, %d; %d): %s",
				i.first.x, i.first.y, i.first.layer,
				name.c_str());
		}
	}
	ImGui::End();

	draw_loot();
}

void autonomaus::LootComponent::draw_loot()
{
	auto& loot = *get_autonomaus().get_loot_state();
	auto& map = get_autonomaus().get_map_state()->get_map();

	auto item_cortex = get_autonomaus().get_userdata<ItemCortex>();
	if (item_cortex == nullptr)
	{
		return;
	}

	std::set<thoreau::TileCoordinate> unvisited;
	for (auto& i: this->loot_draws)
	{
		unvisited.insert(i.first);
	}

	std::map<thoreau::TileCoordinate, std::string> names;
	for (auto& i: loot)
	{
		if (!item_cortex->has_name(i.second))
		{
			continue;
		}

		auto name = item_cortex->get_name(i.second);
		names[i.first] += name + "\n";
		unvisited.erase(i.first);
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
		world_position.y = tile.get_elevation();
		world_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;\

		// For tables, etc.
		if (tile.is_impassable())
		{
			world_position.y += tile.get_height();
		}

		auto& draw = this->loot_draws[i.first];

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
		this->loot_draws.erase(i);
	}
}