// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "continent1/mapSnapshot.hpp"

continent1::TileSnapshot& continent1::MapSnapshot::add(
	const thoreau::TileCoordinate& tile_coordinate)
{
	auto iter = this->tiles.find(tile_coordinate);
	if (iter == this->tiles.end())
	{
		auto result = this->tiles.emplace(
			tile_coordinate, TileSnapshot(tile_coordinate));

		auto sector_coordinate = thoreau::sector_from_tile(tile_coordinate);
		sector_coordinate.layer = 0;

		this->tiles_by_sector.emplace(sector_coordinate, tile_coordinate);
		iter = result.first;
	}

	return iter->second;
}

bool continent1::MapSnapshot::has(
	const thoreau::TileCoordinate& tile_coordinate) const
{
	return this->tiles.count(tile_coordinate) != 0;
}

continent1::TileSnapshot& continent1::MapSnapshot::get(
	const thoreau::TileCoordinate& tile_coordinate)
{
	assert(has(tile_coordinate));
	return this->tiles.find(tile_coordinate)->second;
}

const continent1::TileSnapshot& continent1::MapSnapshot::get(
	const thoreau::TileCoordinate& tile_coordinate) const
{
	assert(has(tile_coordinate));
	return this->tiles.find(tile_coordinate)->second;
}

std::size_t continent1::MapSnapshot::count() const
{
	return this->tiles.size();
}

bool continent1::MapSnapshot::empty() const
{
	return this->tiles.empty();
}

void continent1::MapSnapshot::clear()
{
	this->tiles.clear();
	this->tiles_by_sector.clear();
}

continent1::MapSnapshot::const_iterator continent1::MapSnapshot::begin() const
{
	return this->tiles.begin();
}

continent1::MapSnapshot::const_iterator continent1::MapSnapshot::end() const
{
	return this->tiles.end();
}

continent1::MapSnapshot::const_sector_iterator
continent1::MapSnapshot::sectors_begin(
	const thoreau::SectorCoordinate& sector_coordinate) const
{
	return this->tiles_by_sector.lower_bound(sector_coordinate);
}

continent1::MapSnapshot::const_sector_iterator
continent1::MapSnapshot::sectors_end(
	const thoreau::SectorCoordinate& sector_coordinate) const
{
	return this->tiles_by_sector.upper_bound(sector_coordinate);
}
