// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_VISIBLE_TILE_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_VISIBLE_TILE_WORLD_UPDATE_PASS_HPP

#include <set>
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "thoreau/map.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace chomsky
{
	class VisibleTileWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef TransformWorldUpdatePass::TransformState TransformState;
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TileElevationState;
		typedef WorldState<thoreau::TileCoordinate, bool> VisibleTileState;
		VisibleTileWorldUpdatePass(
			const TransformState* transform_state,
			const TileElevationState* tile_elevation_state,
			VisibleTileState* visible_tile_state);
		~VisibleTileWorldUpdatePass() = default;

		void draw_model(
			int tag, const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;
		void draw_terrain(
			int tag, const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event) override;

	private:
		void check(int tag, const kompvter::ModelDrawEvent& event);

		const TransformState* transform_state;
		const TileElevationState* tile_elevation_state;
		VisibleTileState* visible_tile_state;
	};
}

#endif
