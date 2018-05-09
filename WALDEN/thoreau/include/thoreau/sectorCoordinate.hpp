// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef THOREAU_SECTOR_COORDINATE_HPP
#define THOREAU_SECTOR_COORDINATE_HPP

#include <cstdint>
#include <glm/glm.hpp>
#include "thoreau/detail/coordinate.hpp"

namespace thoreau
{
	typedef Coordinate SectorCoordinate;

	SectorCoordinate sector_from_tile(const Coordinate& tile_coordinate);
	SectorCoordinate sector_from_world(const Coordinate& world_coordinate);
	SectorCoordinate sector_from_world(const glm::vec3& position);
}

#endif
