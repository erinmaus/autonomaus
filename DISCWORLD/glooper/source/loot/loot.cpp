// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "glooper/item/loot.hpp"

bool glooper::Loot::has(const thoreau::TileCoordinate& tile_coordinate) const
{
	return count(tile_coordinate) != 0;
}

std::size_t glooper::Loot::count() const
{
	return this->items.size();
}

std::size_t glooper::Loot::count(const thoreau::TileCoordinate& tile_coordinate) const
{
	return this->items.count(tile_coordinate);
}

bool glooper::Loot::empty() const
{
	return this->items.empty();
}

void glooper::Loot::clear()
{
	this->items.clear();
}

void glooper::Loot::add(
	const thoreau::TileCoordinate& tile_coordinate,
	const std::string& item)
{
	auto key = std::make_pair(tile_coordinate, item);
	if (this->items_set.count(key) == 0)
	{
		this->items.emplace(tile_coordinate, item);
		this->items_set.insert(key);
	}
}

void glooper::Loot::remove(
	const thoreau::TileCoordinate& tile_coordinate,
	const std::string& item)
{
	auto key = std::make_pair(tile_coordinate, item);
	this->items_set.erase(key);

	for (auto i = this->items.begin(); i != this->items.end(); /* Nop. */)
	{
		if (i->first == tile_coordinate && i->second == item)
		{
			i = this->items.erase(i);
		}
		else
		{
			++i;
		}
	}
}

glooper::Loot::const_iterator glooper::Loot::begin() const
{
	return this->items.begin();
}

glooper::Loot::const_iterator glooper::Loot::begin(
	const thoreau::TileCoordinate& tile_coordinate) const
{
	return this->items.lower_bound(tile_coordinate);
}

glooper::Loot::const_iterator glooper::Loot::end() const
{
	return this->items.end();
}

glooper::Loot::const_iterator glooper::Loot::end(
	const thoreau::TileCoordinate& tile_coordinate) const
{
	return this->items.upper_bound(tile_coordinate);
}
