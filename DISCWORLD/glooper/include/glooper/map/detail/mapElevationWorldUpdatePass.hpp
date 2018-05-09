// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_MAP_DETAIL_MAP_ELEVATION_WORLD_UPDATE_PASS_HPP
#define GLOOPER_MAP_DETAIL_MAP_ELEVATION_WORLD_UPDATE_PASS_HPP

#include <set>
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "thoreau/sectorCoordinate.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/map.hpp"

namespace glooper
{
	class MapElevationWorldUpdatePass : public chomsky::WorldUpdatePass
	{
	public:
		typedef chomsky::WorldState<thoreau::TileCoordinate, chomsky::TileElevationExtents> TileElevationState;
		typedef chomsky::WorldState<thoreau::SectorCoordinate, bool> RuntimeSectorFlagState;

		MapElevationWorldUpdatePass(
			const TileElevationState* terrain_tile_elevation_state,
			const TileElevationState* water_tile_elevation_state,
			const thoreau::Map* map,
			TileElevationState* tile_elevation_state,
			RuntimeSectorFlagState* runtime_sector_flag_state,
			thoreau::Map* runtime_map);
		~MapElevationWorldUpdatePass() = default;

		void stop() override;

		void draw_terrain(
			int tag, const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event) override;
		void invalidate_tag(int tag) override;

	private:
		const TileElevationState* terrain_tile_elevation_state;
		const TileElevationState* water_tile_elevation_state;
		const thoreau::Map* map;

		TileElevationState* tile_elevation_state;
		RuntimeSectorFlagState* runtime_sector_flag_state;
		thoreau::Map* runtime_map;

		std::set<int> tags;
		bool is_dirty = false;
	};
}

#endif
