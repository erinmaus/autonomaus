// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_SCENERY_ELEVATION_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_SCENERY_ELEVATION_WORLD_UPDATE_PASS_HPP

#include <set>
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/multiTileElevationExtents.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace chomsky
{
	class SceneryElevationWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TerrainElevationState;
		typedef WorldState<thoreau::TileCoordinate, MultiTileElevationExtents> SceneryElevationState;
		typedef TransformWorldUpdatePass::TransformState TransformState;

		SceneryElevationWorldUpdatePass(
			const TransformState* transform_state,
			const TerrainElevationState* terrain_elevation_state,
			SceneryElevationState* scenery_extent_state,
			SceneryElevationState* relative_scenery_extent_state);
		~SceneryElevationWorldUpdatePass() = default;

		void start() override;
		void stop() override;

		void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;

		void invalidate_tag(int tag) override;

	private:
		const TransformState* transform_state;
		const TerrainElevationState* terrain_elevation_state;
		SceneryElevationState* scenery_elevation_state;
		SceneryElevationState* relative_scenery_elevation_state;
		SceneryElevationState working_scenery_elevation_state;

		std::set<int> model_tags;
	};
}

#endif
