// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "thoreau/tileCoordinate.hpp"
#include "thoreau/thoreau.hpp"

thoreau::TileCoordinate thoreau::tile_from_input(int x, int y, int layer)
{
	if (x < TileCoordinate::MIN_COORDINATE)
	{
		x = TileCoordinate::MIN_COORDINATE;
	}
	else if  (x > TileCoordinate::MAX_COORDINATE)
	{
		x = TileCoordinate::MAX_COORDINATE;
	}

	if (y < TileCoordinate::MIN_COORDINATE)
	{
		y = TileCoordinate::MIN_COORDINATE;
	}
	else if  (y > TileCoordinate::MAX_COORDINATE)
	{
		y = TileCoordinate::MAX_COORDINATE;
	}

	if (layer < TileCoordinate::MIN_LAYER)
	{
		layer = TileCoordinate::MIN_LAYER;
	}
	else if (layer > TileCoordinate::MAX_LAYER)
	{
		layer = TileCoordinate::MAX_LAYER;
	}

	return TileCoordinate(x, y, layer);
}

thoreau::TileCoordinate thoreau::tile_from_world(
	const Coordinate& world_coordinate)
{
	return TileCoordinate(
		world_coordinate.x / TILE_SIZE,
		world_coordinate.y / TILE_SIZE,
		world_coordinate.layer);
}

thoreau::TileCoordinate thoreau::tile_from_world(
	const glm::vec3& world_coordinate)
{
	return TileCoordinate(
		world_coordinate.x / TILE_SIZE,
		world_coordinate.z / TILE_SIZE);
}

thoreau::TileCoordinate thoreau::tile_from_sector(
	const Coordinate& sector_coordinate)
{
	return TileCoordinate(
		sector_coordinate.x * SECTOR_SIZE,
		sector_coordinate.y * SECTOR_SIZE,
		sector_coordinate.layer);
}
