// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/state/map.hpp"

const thoreau::Map& autonomaus::Map::get_map() const
{
	return this->map;
}

const glooper::Minimap& autonomaus::Map::get_minimap() const
{
	return this->minimap;
}
