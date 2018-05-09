// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MAP_HPP
#define AUTONOMAUS_STATE_MAP_HPP

#include <memory>
#include "thoreau/map.hpp"
#include "glooper/map/minimap.hpp"

namespace autonomaus
{
	class Map
	{
	public:
		friend class MapProvider;

		Map() = default;
		~Map() = default;

		const thoreau::Map& get_map() const;
		const glooper::Minimap& get_minimap() const;

	private:
		thoreau::Map map;
		glooper::Minimap minimap;
	};
}

#endif
