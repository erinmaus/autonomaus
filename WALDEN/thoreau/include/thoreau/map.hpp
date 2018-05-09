// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef THOREAU_MAP_HPP
#define THOREAU_MAP_HPP

#include <iostream>
#include <vector>
#include <boost/container/flat_map.hpp>
#include <boost/container/flat_set.hpp>
#include "thoreau/sectorCoordinate.hpp"
#include "thoreau/tile.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace thoreau
{
	class Map
	{
	public:
		Map() = default;
		~Map() = default;

		bool has_tile(int x, int y) const;
		bool has_tile(int x, int y, int layer) const;
		bool has_tile(const TileCoordinate& coordinate) const;

		Tile& insert_tile(int x, int y, int layer);
		Tile& insert_tile(const TileCoordinate& coordinate);

		Tile& add_tile(int x, int y, int layer);
		Tile& add_tile(const TileCoordinate& coordinate);

		void remove_tile(int x, int y);
		void clear();

		void merge(const Map& other);

		int get_num_layers(int x, int y) const;
		int get_num_layers(const TileCoordinate& coordinate) const;
		bool try_get_tile_layer(int x, int y, float elevation, int& layer) const;
		bool try_get_tile_layer(const TileCoordinate& coordinate, float elevation, int& layer) const;

		Tile& get_tile(int x, int y, int layer);
		Tile& get_tile(const TileCoordinate& coordinate);
		const Tile& get_tile(int x, int y, int layer) const;
		const Tile& get_tile(const TileCoordinate& coordinate) const;

		bool get_first_tile(TileCoordinate& result) const;
		bool get_next_tile(const TileCoordinate& current, TileCoordinate& result) const;

		bool can_move(
			const TileCoordinate& tile_coordinate,
			int offset_x, int offset_y, int offset_layer = 0) const;
		bool can_move(
			int from_x, int from_y, int from_layer,
			int to_x, int to_y, int to_layer) const;
		bool can_move(
			const TileCoordinate& from,
			const TileCoordinate& to) const;

		void save(std::ostream& stream) const;
		std::size_t load(std::istream& stream);

	private:
		void touch_tile(int x, int y, int layer);

		typedef boost::container::flat_set<thoreau::TileCoordinate> TileSet;
		typedef boost::container::flat_map<thoreau::SectorCoordinate, TileSet> SectorCollection;
		SectorCollection tiles_by_sector;
		TileSet empty_tile_set;

		typedef std::vector<Tile> LayerCollection;
		typedef boost::container::flat_map<TileCoordinate, LayerCollection> TileCollection;
		TileCollection tiles;

		LayerCollection& add_layer(int x, int y);
		LayerCollection& get_layer(int x, int y);
		const LayerCollection& get_layer(int x, int y) const;

		static std::size_t layer_to_offset(int layer);

	public:
		typedef TileSet::const_iterator const_sector_iterator;

		const_sector_iterator sector_begin(
			const thoreau::SectorCoordinate& sector_coordinate) const;
		const_sector_iterator sector_end(
			const thoreau::SectorCoordinate& sector_coordinate) const;
	};
}

#endif
