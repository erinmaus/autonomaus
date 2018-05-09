// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef THOREAU_TILE_COORDINATE_HPP
#define THOREAU_TILE_COORDINATE_HPP

#include <cstdint>
#include <glm/glm.hpp>
#include "thoreau/detail/coordinate.hpp"

namespace thoreau
{
	typedef Coordinate TileCoordinate;

	TileCoordinate tile_from_input(int x, int y, int layer = 0);
	TileCoordinate tile_from_world(const Coordinate& world_coordinate);
	TileCoordinate tile_from_world(const glm::vec3& position);

	TileCoordinate tile_from_sector(const Coordinate& world_coordinate);
}

#endif
