// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_PROP_PROPS_HPP
#define GLOOPER_PROP_PROPS_HPP

#include <map>
#include <set>
#include <string>
#include "glooper/prop/prop.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace glooper
{
	class Props
	{
	public:
		Props() = default;
		~Props() = default;

		bool has(const thoreau::TileCoordinate& tile_coordinate) const;
		bool has(const std::string& name) const;

		std::size_t count() const;
		std::size_t count(const thoreau::TileCoordinate& tile_coordinate) const;
		std::size_t count(const std::string& name) const;
		bool empty() const;

		void clear();

		void add(const Prop& prop);
		void add(
			const std::string& name,
			const thoreau::TileCoordinate& tile_coordinate);
		void remove(const Prop& prop);
		void remove(
			const std::string& name,
			const thoreau::TileCoordinate& tile_coordinate);

	private:
		typedef std::multimap<thoreau::TileCoordinate, Prop> SpaceMap;
		SpaceMap tile_to_prop;

		typedef std::multimap<std::string, thoreau::TileCoordinate> PropMap;
		PropMap prop_to_tile;

		typedef std::set<Prop> PropSet;
		PropSet props;

	public:
		typedef PropSet::const_iterator const_iterator;
		const_iterator begin() const;
		const_iterator end() const;

		typedef PropMap::const_iterator const_prop_iterator;
		const_prop_iterator begin(const std::string& name) const;
		const_prop_iterator end(const std::string& name) const;

		typedef SpaceMap::const_iterator const_space_iterator;
		const_space_iterator begin(const thoreau::TileCoordinate& tile_coordinate) const;
		const_space_iterator end(const thoreau::TileCoordinate& tile_coordinate) const;
	};
}

#endif
