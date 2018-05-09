// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "thoreau/worldCoordinate.hpp"
#include "thoreau/thoreau.hpp"

thoreau::WorldCoordinate thoreau::world_from_position(const glm::vec3& position)
{
	return WorldCoordinate(position.x, position.z, 0);
}

thoreau::WorldCoordinate thoreau::world_from_tile(
	const Coordinate& tile_coordinate)
{
	return WorldCoordinate(
		tile_coordinate.x * thoreau::TILE_SIZE + thoreau::TILE_SIZE / 2,
		tile_coordinate.y * thoreau::TILE_SIZE + thoreau::TILE_SIZE / 2,
		tile_coordinate.layer);
}
