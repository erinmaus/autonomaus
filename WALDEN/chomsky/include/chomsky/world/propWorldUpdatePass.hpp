// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_PROP_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_PROP_WORLD_UPDATE_PASS_HPP

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
	class PropWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef TransformWorldUpdatePass::TransformState TransformState;
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TileElevationState;
		typedef WorldState<thoreau::TileCoordinate, TileSpacePointCloud> TileCloudState;
		typedef WorldState<thoreau::TileCoordinate, bool> DirtyTileState;

		PropWorldUpdatePass(
			const TransformState* transform_state,
			const TileElevationState* tile_elevation_state,
			DirtyTileState* dirty_tile_state,
			TileCloudState* prop_cloud_state);
		~PropWorldUpdatePass() = default;

		void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;

		void invalidate_tag(int tag) override;

	private:
		const TransformState* transform_state;
		const TileElevationState* tile_elevation_state;
		DirtyTileState* dirty_tile_state;
		TileCloudState* prop_cloud_state;

		std::set<int> model_tags;
	};
}

#endif
