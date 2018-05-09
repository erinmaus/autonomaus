// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <set>
#include "chomsky/world/query/staticTileSpacePointCloudMatchExecutor.hpp"
#include "glooper/item/lootManager.hpp"

glooper::LootManager::LootManager(Loot& loot)
{
	this->loot = &loot;
}

bool glooper::LootManager::has(const std::string& item) const
{
	return this->items.count(item) != 0;
}

void glooper::LootManager::add(
	const std::string& item,
	const chomsky::ItemIconSilhouette& silhouette)
{
	this->items[item] = silhouette;
	this->is_dirty = true;
}

void glooper::LootManager::remove(const std::string& item)
{
	this->items.erase(item);
}

void glooper::LootManager::update(
	const chomsky::StaticWorldUpdateBlock::DirtyTileState& dirty_tile_state,
	const chomsky::StaticWorldUpdateBlock& static_block)
{
	chomsky::StaticTileSpacePointCloudMatchExecutor executor(&static_block);

	std::set<std::pair<thoreau::TileCoordinate, std::string>> removed_items;
	for (auto& i: *this->loot)
	{
		auto tile_coordinate = i.first;
		auto iter = this->items.find(i.second);
		if (iter == this->items.end() ||
			!executor.execute(tile_coordinate, iter->second.get_point_cloud_match()))
		{
			removed_items.emplace(i);
		}
	}

	for (auto& i: removed_items)
	{
		this->loot->remove(i.first, i.second);
	}

	std::set<thoreau::TileCoordinate> unvisited_tiles;
	if (this->is_dirty)
	{
		for (auto& i: *static_block.get_point_cloud_state())
		{
			unvisited_tiles.insert(i.first);
		}
	}
	else
	{
		for (auto& i: dirty_tile_state)
		{
			unvisited_tiles.insert(i.first);
		}
	}

	for (auto tile_coordinate: unvisited_tiles)
	{
		for (auto& j: this->items)
		{
			auto& item = j.first;
			auto& match = j.second.get_point_cloud_match();

			if (executor.execute(tile_coordinate, match))
			{
				this->loot->add(tile_coordinate, item);
			}
		}
	}

	this->is_dirty = false;
}
