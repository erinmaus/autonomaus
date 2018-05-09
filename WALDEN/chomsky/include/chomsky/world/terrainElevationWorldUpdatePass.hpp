// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_TERRAIN_ELEVATION_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_TERRAIN_ELEVATION_WORLD_UPDATE_PASS_HPP

#include <unordered_map>
#include <set>
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/tileElevationWorldUpdatePass.hpp"
#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/sectorCoordinate.hpp"

namespace chomsky
{
	class TerrainElevationWorldUpdatePass : public TileElevationWorldUpdatePass
	{
	public:
		typedef TransformWorldUpdatePass::TransformState TransformState;

		TerrainElevationWorldUpdatePass(
			const TransformState* transform_state,
			TileElevationState* state);
		~TerrainElevationWorldUpdatePass() = default;

		void draw_terrain(
			int tag,
			const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event) override;

	private:
		const TransformState* transform_state;
	};
}

#endif
