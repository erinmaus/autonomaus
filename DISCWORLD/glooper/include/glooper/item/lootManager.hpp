// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_ITEM_LOOT_MANAGER_HPP
#define GLOOPER_ITEM_LOOT_MANAGER_HPP

#include "chomsky/world/blocks/staticWorldUpdateBlock.hpp"
#include "glooper/item/loot.hpp"
#include "glooper/resources/resourceListener.hpp"
#include "glooper/resources/resourceManager.hpp"

namespace glooper
{
	class LootManager
	{
	public:
		LootManager(Loot& loot);
		LootManager(const LootManager& other) = delete;
		~LootManager() = default;

		bool has(const std::string& item) const;
		void add(
			const std::string& item,
			const chomsky::ItemIconSilhouette& silhouette);
		void remove(const std::string& item);

		void update(
			const chomsky::StaticWorldUpdateBlock::DirtyTileState& dirty_tile_state,
			const chomsky::StaticWorldUpdateBlock& static_block);

		LootManager& operator =(const LootManager& other) = delete;

	private:
		ResourceManager* resource_manager;
		Loot* loot;

		bool is_dirty = false;
		std::map<std::string, chomsky::ItemIconSilhouette> items;
	};
}

#endif
