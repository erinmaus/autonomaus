// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef THOREAU_PATH_FINDER_HPP
#define THOREAU_PATH_FINDER_HPP

#include <vector>
#include <set>
#include "thoreau/map.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/sectorCoordinate.hpp"

namespace thoreau
{
	class PathFinder
	{
	public:
		explicit PathFinder(const Map& map);
		PathFinder(const Map& map, const thoreau::PathFinder& other);
		~PathFinder() = default;

		bool get_can_move_diagonally() const;
		void set_can_move_diagonally(bool value);

		float get_neighbor_weighting_radius() const;
		void set_neighbor_weighting_radius(float value);

		float get_wall_weight() const;
		void set_wall_weight(float value);
		float get_impassable_weight() const;
		void set_impassable_weight(float value);
		float get_door_weight() const;
		void set_door_weight(float value);
		float get_portal_weight() const;
		void set_portal_weight(float value);

		void add_sector_restriction(
			const thoreau::SectorCoordinate& sector_coordinate);
		void clear_sector_restrictions();
		bool has_sector_restrictions() const;

		bool try_find_path(
			const TileCoordinate& start_coordinate,
			const TileCoordinate& stop_coordinate);

	private:
		virtual float tile_distance(
			const TileCoordinate& a,
			const TileCoordinate& b) const;
		virtual float tile_heuristic(
			const TileCoordinate& tile_coordinate,
			const TileCoordinate& stop_coordinate) const;
		virtual void get_neighbors(
			const TileCoordinate& tile_coordinate,
			std::vector<TileCoordinate>& result) const;

		const Map* map;

		bool can_move_diagonally = false;
		float wall_weight = 4.0f;
		float impassable_weight = 4.0f;
		float door_weight = 8.0f;
		float portal_weight = 16.0f;
		float neighbor_weighting_radius = 0.0f;

		std::vector<thoreau::TileCoordinate> result;
		std::set<thoreau::SectorCoordinate> sectors;

	public:
		typedef std::vector<thoreau::TileCoordinate>::const_iterator const_iterator;

		std::size_t length() const;
		bool empty() const;

		thoreau::TileCoordinate at(std::size_t index) const;

		void clear();

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
