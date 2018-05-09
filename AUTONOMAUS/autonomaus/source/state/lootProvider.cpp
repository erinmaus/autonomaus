// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include "mantorok/io/datReader.hpp"
#include "chomsky/gui/itemIconSilhouette.hpp"
#include "chomsky/io/gui/itemIconSilhouette.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/lootProvider.hpp"

autonomaus::LootProvider::LootProvider(const WorldProvider& world_provider)
{
	this->world_provider = &world_provider;
}

void autonomaus::LootProvider::attach(GameStateManager& manager, Slot& slot)
{
	Base::attach(manager, slot);
	this->loot_manager = std::make_unique<glooper::LootManager>(slot.get_working());
}

void autonomaus::LootProvider::detach()
{
	this->loot_manager.reset();

	Base::detach();
}

void autonomaus::LootProvider::add_item(
	GameStateManager& game_state,
	const View& view,
	const std::string& item,
	const chomsky::ItemIconSilhouette& icon)
{
	game_state.execute<LootProvider>(
		view,
		[item, icon](auto& provider)
		{
			provider.loot_manager->add(item, icon);
		});
}

void autonomaus::LootProvider::remove_item(
	GameStateManager& game_state,
	const View& view,
	const std::string& item)
{
	game_state.execute<LootProvider>(
		view,
		[item](auto& provider)
		{
			provider.loot_manager->remove(item);
		});
}

void autonomaus::LootProvider::load_items(
	GameStateManager& game_state,
	const View& view,
	const std::string& path)
{
	game_state.execute<LootProvider>(
		view,
		[path, &game_state](auto& provider)
		{
			auto& resource_manager = game_state.get_resource_manager();
			resource_manager.for_each_file(path,
				[&provider](auto& filename)
				{
					chomsky::ItemIconSilhouette icon;

					std::ifstream stream(filename, std::ios::binary);
					mantorok::DATReader reader(stream);
					mantorok::DATDeserializer<chomsky::ItemIconSilhouette>::deserialize(icon, reader);

					provider.loot_manager->add(icon.to_string(), icon);
				});
		});
}

void autonomaus::LootProvider::update()
{
	this->loot_manager->update(
		*this->world_provider->get_dirty_tile_state(),
		*this->world_provider->get_static_block());
}
