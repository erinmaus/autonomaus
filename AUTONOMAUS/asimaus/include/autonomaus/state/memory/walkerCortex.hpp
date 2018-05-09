// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_WALKER_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_WALKER_CORTEX_HPP

#include <unordered_set>
#include "autonomaus/state/memory/cortex.hpp"
#include "thoreau/map.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/pathFinder.hpp"

namespace autonomaus
{
	class ViewCortex;

	class WalkerCortex : public Cortex
	{
	public:
		WalkerCortex(Autonomaus& autonomaus);
		~WalkerCortex() = default;

		Result find_path(const thoreau::TileCoordinate& destination);

		Result disable_minimap();
		Result enable_minimap();
		bool is_minimap_enabled() const;

		Result step();

		Result cancel();
		Result finish();

		bool at_destination(float distance) const;
		bool pending() const;

	private:
		static const int MAX_VISIBILITY_RADIUS;
		static const int MAX_SNAP_RADIUS;
		static const int MAX_MINIMAP_RADIUS;
		static const int MAX_WORLD_RADIUS;
		static const int PAN_RADIUS;

		struct FindPathAction;
		struct StepAction;

		thoreau::PathFinder current_path;
		std::size_t get_nearest_path_point(const thoreau::PathFinder& path);
		std::size_t get_next_obstacle(const thoreau::PathFinder& path, std::size_t index);

		bool reachable_without_obstacle(
			const thoreau::TileCoordinate& start,
			const thoreau::TileCoordinate& stop) const;
		bool do_reachable_without_obstacle(
			const thoreau::Map& map,
			const thoreau::TileCoordinate& current,
			const thoreau::TileCoordinate& stop,
			std::unordered_set<thoreau::TileCoordinate>& visited) const;

		bool is_obstacle(const thoreau::PathFinder& path, std::size_t index) const;

		bool is_door_blocking(
			const thoreau::TileCoordinate& current_coordinate,
			const thoreau::TileCoordinate& next_coordinate) const;
		bool is_portal_blocking(
			const thoreau::TileCoordinate& current_coordinate,
			const thoreau::TileCoordinate& next_coordinate) const;
		bool is_obstacle_blocking(
			const thoreau::TileCoordinate& current_coordinate,
			const thoreau::TileCoordinate& previous_coordinate) const;

		bool find_most_visible_tile(
			const thoreau::PathFinder& path,
			std::size_t index,
			std::size_t count,
			thoreau::TileCoordinate& result) const;

		int compute_tile_f_score(const thoreau::TileCoordinate& tile_coordinate) const;
		bool should_skip_tile(
			const thoreau::Map& map,
			const thoreau::TileCoordinate& previous_coordinate,
			const thoreau::TileCoordinate& current_coordinate,
			int offset_x, int offset_y) const;
		void do_compute_tile_f_score(
			const thoreau::Map& map,
			const ViewCortex* view,
			const thoreau::TileCoordinate& center_coordinate,
			const thoreau::TileCoordinate& previous_coordinate,
			int offset_x, int offset_y,
			int& f_score,
			std::unordered_set<thoreau::TileCoordinate>& visited) const;

		bool minimap_enabled = true;
	};
}

#endif
