// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_ACTOR_DRAW_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_ACTOR_DRAW_WORLD_UPDATE_PASS_HPP

#include "chomsky/world/actorDraw.hpp"
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/worldCoordinate.hpp"

namespace chomsky
{
	class ActorDrawWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TileElevationState;
		typedef WorldState<thoreau::WorldCoordinate, ActorDraw> DrawState;

		ActorDrawWorldUpdatePass(
			const TileElevationState* tile_elevation_state,
			DrawState* draw_state);
		~ActorDrawWorldUpdatePass() = default;

		void start() override;

		void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;

	private:
		const TileElevationState* tile_elevation_state;
		DrawState* draw_state;
	};
}

#endif
