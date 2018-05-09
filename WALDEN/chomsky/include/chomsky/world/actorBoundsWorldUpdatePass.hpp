// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_ACTOR_BOUNDS_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_ACTOR_BOUNDS_WORLD_UPDATE_PASS_HPP

#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/terrainElevationWorldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/worldCoordinate.hpp"
#include "kvlc/geometry/bounds.hpp"

namespace chomsky
{
	class ActorBoundsWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TileElevationState;
		typedef WorldState<thoreau::WorldCoordinate, kvlc::Bounds> BoundsState;

		ActorBoundsWorldUpdatePass(
			const TileElevationState* tile_elevation_state,
			BoundsState* bounds_state);
		~ActorBoundsWorldUpdatePass() = default;

		void start() override;

		void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;

	private:
		const TileElevationState* tile_elevation_state;
		BoundsState* bounds_state;
	};
}

#endif
