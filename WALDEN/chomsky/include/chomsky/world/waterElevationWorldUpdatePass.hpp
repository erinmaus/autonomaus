// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_WATER_ELEVATION_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_WATER_ELEVATION_WORLD_UPDATE_PASS_HPP

#include "chomsky/world/tileElevationWorldUpdatePass.hpp"
#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"

namespace chomsky
{
	class WaterElevationWorldUpdatePass : public TileElevationWorldUpdatePass
	{
	public:
		typedef TransformWorldUpdatePass::TransformState TransformState;

		WaterElevationWorldUpdatePass(
			const TransformState* transform_state,
			TileElevationState* state);
		~WaterElevationWorldUpdatePass() = default;

		void draw_water(
			int tag,
			const kvre::Terrain* water,
			const kompvter::ModelDrawEvent& event) override;

	private:
		const TransformState* transform_state;
	};
}

#endif
