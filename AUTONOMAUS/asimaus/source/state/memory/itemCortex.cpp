// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <algorithm>
#include <fstream>
#include "mantorok/io/datWriter.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/lootProvider.hpp"
#include "chomsky/gui/itemIconSilhouette.hpp"
#include "chomsky/io/gui/itemIconSilhouette.hpp"

autonomaus::ItemCortex::ItemCortex(Autonomaus& autonomaus) :
	Cortex("Game::Item", autonomaus)
{
	auto& loot = autonomaus.get_loot_state();
	LootProvider::load_items(
		autonomaus.get_game_state(), loot,
		Autonomaus::get_config_path("items"));
}

static void save_item_icon(const chomsky::ItemIconSilhouette& icon)
{
	auto escaped_name = icon.to_string();
	std::replace(escaped_name.begin(), escaped_name.end(), '/', '_');
	std::replace(escaped_name.begin(), escaped_name.end(), ':', '-');

	std::string filename = "items/";
	filename += escaped_name;

	auto path = autonomaus::Autonomaus::get_config_path(filename);
	std::ofstream stream(path, std::ios::binary);
	mantorok::DATWriter writer(stream);
	mantorok::DATSerializer<chomsky::ItemIconSilhouette>::serialize(icon, writer);
}

void autonomaus::ItemCortex::remember_item(const std::string& runtime_key)
{
	auto& game_state = get_autonomaus().get_game_state();
	auto& loot = get_autonomaus().get_loot_state();

	game_state.execute([&loot, runtime_key](auto& game_state)
	{
		auto& resource_manager = game_state.get_resource_manager();
		auto& item_icon_manager = resource_manager.get_item_icon_manager();

		if (item_icon_manager.has_item(runtime_key))
		{
			auto& icon = item_icon_manager.get_item(runtime_key);
			save_item_icon(icon);

			LootProvider::add_item(game_state, loot, icon.to_string(), icon);
		}
	});
}

void autonomaus::ItemCortex::add_name(
	const std::string& key,
	const std::string& name,
	int object_id)
{
	std::string memory_key = "item::";
	memory_key += key;

	auto memory = get(memory_key);
	memory->set_integer(name, object_id);
	remember(memory_key, memory);
}

bool autonomaus::ItemCortex::has_name(const std::string& key) const
{
	std::string memory_key = "item::";
	memory_key += key;

	if (!has_memory(memory_key))
	{
		return false;
	}

	auto memory = get(memory_key);
	return !memory.empty();
}

std::string autonomaus::ItemCortex::get_name(const std::string& key) const
{
	std::string memory_key = "item::";
	memory_key += key;

	if (has_name(key))
	{
		auto memory = get(memory_key);
		return memory->begin()->first;
	}

	return std::string();
}

bool autonomaus::ItemCortex::is_ambiguous(const std::string& key) const
{
	return get_names(key).size() > 1;
}

std::vector<std::string> autonomaus::ItemCortex::get_names(const std::string& key) const
{
	std::string memory_key = "item::";
	memory_key += key;

	std::vector<std::string> results;
	if (has_name(key))
	{
		auto memory = get(memory_key);
		for (auto& i: *memory)
		{
			results.push_back(i.first);
		}
	}

	return results;
}
