// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_NAVIGATION_WALKER_HPP
#define AUTONOMAUS_NAVIGATION_WALKER_HPP

#include <limits>
#include <set>
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/state/world.hpp"
#include "autonomaus/state/view.hpp"
#include "thoreau/map.hpp"
#include "thoreau/pathFinder.hpp"

namespace autonomaus
{
	class Walker
	{
	public:
		Walker(
			const Autonomaus& autonomaus,
			float max_distance = 3.0f);
		~Walker() = default;

		float get_max_distance() const;
		void set_max_distance(float value);

		const thoreau::PathFinder& get_path() const;
		const View& get_view() const;

		bool try_find_path(const thoreau::TileCoordinate& tile_coordinate);
		bool try_find_path(
			const thoreau::TileCoordinate& from,
			const thoreau::TileCoordinate& to);

		enum
		{
			none = 0,
			greater_visibility = 1 << 0,
			greater_distance = 1 << 1,
			continue_past_obstacle = 1 << 2
		};
		bool try_find_next_tile(
			thoreau::TileCoordinate& result,
			float max_distance,
			int flags);

		bool snap_to_closest_tile(float max_distance);
		bool rewind(float distance);

		const thoreau::TileCoordinate& get_target_tile_coordinate() const;
		const thoreau::TileCoordinate& get_current_tile_coordinate() const;
		std::size_t get_current_path_index() const;
		std::size_t get_target_path_index() const;

		bool is_at_door() const;
		bool is_at_portal() const;
		bool is_at_obstacle() const;

		bool is_line_clear(
			const thoreau::TileCoordinate& a,
			const thoreau::TileCoordinate& b);

		void update();

	private:
		const Autonomaus* autonomaus;
		const thoreau::Map* map;

		bool should_skip_tile(
			const thoreau::TileCoordinate& previous_coordinate,
			const thoreau::TileCoordinate& current_coordinate,
			int offset_x, int offset_y);
		bool try_find_visible_tile(
			const thoreau::TileCoordinate& tile_coordinate,
			thoreau::TileCoordinate& result);
		void do_try_find_visible_tile(
			const thoreau::TileCoordinate& center_coordinate,
			const thoreau::TileCoordinate& previous_coordinate,
			int offset_x, int offset_y,
			std::set<thoreau::TileCoordinate>& visible,
			std::set<thoreau::TileCoordinate>& visited);
		int compute_tile_f_score(const thoreau::TileCoordinate& tile_coordinate);
		void do_compute_tile_f_score(
			const thoreau::TileCoordinate& tile_coordinate,
			const thoreau::TileCoordinate& previous_coordinate,
			int offset_x, int offset_y,
			int& f_score,
			std::set<thoreau::TileCoordinate>& visited);

		bool find_next_obstacle(
			std::size_t start_index,
			std::size_t& index,
			thoreau::TileCoordinate& tile_coordinate);
		bool find_distant_tile(
			thoreau::TileCoordinate& result,
			std::size_t max_index,
			float max_distance);
		bool find_visible_tile(
			thoreau::TileCoordinate& result,
			std::size_t max_index,
			float max_distance);

		thoreau::TileCoordinate target_tile_coordinate;
		float max_distance;

		thoreau::TileCoordinate current_tile_coordinate;
		std::size_t current_path_index;

		View view;

		// TODO: Doors opened by default.
		// (E.g., Bob's axes.)
		bool is_door_blocking(
			const thoreau::TileCoordinate& current_coordinate,
			const thoreau::TileCoordinate& next_coordinate) const;
		bool is_portal_blocking(
			const thoreau::TileCoordinate& current_coordinate,
			const thoreau::TileCoordinate& next_coordinate) const;
		bool is_obstacle_blocking(
			const thoreau::TileCoordinate& current_coordinate,
			const thoreau::TileCoordinate& previous_coordinate) const;

		thoreau::PathFinder path_finder;
	};
}

#endif
