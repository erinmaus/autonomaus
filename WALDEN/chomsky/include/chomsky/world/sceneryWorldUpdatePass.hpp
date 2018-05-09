// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_SCENERY_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_SCENERY_WORLD_UPDATE_PASS_HPP

#include <set>
#include "chomsky/resources/tileSpacePointCloud.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/terrainElevationWorldUpdatePass.hpp"
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace chomsky
{
	class SceneryWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TileElevationState;
		typedef WorldState<thoreau::TileCoordinate, TileSpacePointCloud> TileCloudState;
		typedef TransformWorldUpdatePass::TransformState TransformState;

		SceneryWorldUpdatePass(
			const TransformState* transform_state,
			const TileElevationState* scenery_elevation_state,
			TileCloudState* scenery_point_cloud_state);
		~SceneryWorldUpdatePass() = default;

		void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;

		void invalidate_tag(int tag) override;

	private:
		const TransformState* transform_state;
		const TileElevationState* scenery_elevation_state;
		TileCloudState* scenery_point_cloud_state;

		std::set<int> model_tags;
	};
}

#endif
