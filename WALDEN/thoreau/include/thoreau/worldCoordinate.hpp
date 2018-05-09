// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef THOREAU_WORLD_COORDINATE_HPP
#define THOREAU_WORLD_COORDINATE_HPP

#include <glm/glm.hpp>
#include "thoreau/detail/coordinate.hpp"

namespace thoreau
{
	typedef Coordinate WorldCoordinate;

	WorldCoordinate world_from_tile(const Coordinate& tile_coordinate);
	WorldCoordinate world_from_position(const glm::vec3& position);
}

#endif
