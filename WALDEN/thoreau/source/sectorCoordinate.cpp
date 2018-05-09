// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "thoreau/sectorCoordinate.hpp"
#include "thoreau/thoreau.hpp"

thoreau::SectorCoordinate thoreau::sector_from_tile(
	const Coordinate& tile_coordinate)
{
	return SectorCoordinate(
		tile_coordinate.x / SECTOR_SIZE,
		tile_coordinate.y / SECTOR_SIZE,
		tile_coordinate.layer);
}

thoreau::SectorCoordinate thoreau::sector_from_world(
	const glm::vec3& world_coordinate)
{
	return SectorCoordinate(
		world_coordinate.x / (TILE_SIZE * SECTOR_SIZE),
		world_coordinate.z / (TILE_SIZE * SECTOR_SIZE));
}
