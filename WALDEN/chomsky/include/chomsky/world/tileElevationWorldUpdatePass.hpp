// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_TILE_ELEVATION_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_TILE_ELEVATION_WORLD_UPDATE_PASS_HPP

#include <unordered_map>
#include <set>
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/sectorCoordinate.hpp"

namespace chomsky
{
	class TileElevationWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TileElevationState;

		TileElevationWorldUpdatePass(
			TileElevationState* state);
		~TileElevationWorldUpdatePass() = default;

		void start() override;
		void stop() override;

		void invalidate_tag(int tag) override;

	protected:
		TileElevationState* get_output_state();
		TileElevationState& get_working_state();

		bool has_tag(int tag) const;
		void add_sector(int tag, const thoreau::SectorCoordinate& sector_coordinate);
		void remove_sector(int tag, const thoreau::SectorCoordinate& sector_coordinate);

	private:
		void collect_layers();

		TileElevationState* state;
		TileElevationState working_state;

		bool is_dirty = false;
		std::set<int> tags;
		std::unordered_map<int, thoreau::SectorCoordinate> sectors;
	};
}

#endif
