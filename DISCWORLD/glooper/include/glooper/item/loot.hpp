// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_ITEM_LOOT_HPP
#define GLOOPER_ITEM_LOOT_HPP

#include <map>
#include <set>
#include <string>
#include "thoreau/tileCoordinate.hpp"

namespace glooper
{
	class Loot
	{
	public:
		Loot() = default;
		~Loot() = default;

		bool has(const thoreau::TileCoordinate& tile_coordinate) const;

		std::size_t count() const;
		std::size_t count(const thoreau::TileCoordinate& tile_coordinate) const;
		bool empty() const;

		void clear();

		void add(
			const thoreau::TileCoordinate& tile_coordinate,
			const std::string& item);
		void remove(
			const thoreau::TileCoordinate& tile_coordinate,
			const std::string& item);

	private:
		typedef std::multimap<thoreau::TileCoordinate, std::string> ItemMap;
		ItemMap items;

		typedef std::pair<thoreau::TileCoordinate, std::string> TileItemPair;
		std::set<TileItemPair> items_set;

	public:
		typedef ItemMap::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator begin(const thoreau::TileCoordinate& tile_coordinate) const;

		const_iterator end() const;
		const_iterator end(const thoreau::TileCoordinate& tile_coordinate) const;
	};
}

#endif
