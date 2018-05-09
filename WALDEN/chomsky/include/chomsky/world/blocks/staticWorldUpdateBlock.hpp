// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_BLOCKS_STATIC_WORLD_UPDATE_BLOCK_HPP
#define CHOMSKY_WORLD_BLOCKS_STATIC_WORLD_UPDATE_BLOCK_HPP

#include "chomsky/resources/tileSpacePointCloud.hpp"
#include "chomsky/world/materialsWorldUpdatePass.hpp"
#include "chomsky/world/propWorldUpdatePass.hpp"
#include "chomsky/world/sceneryElevationWorldUpdatePass.hpp"
#include "chomsky/world/sceneryWorldUpdatePass.hpp"
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/worldUpdateManager.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/world/blocks/worldUpdateBlock.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace chomsky
{
	class StaticWorldUpdateBlock : public WorldUpdateBlock
	{
	public:
		typedef TransformWorldUpdatePass::TransformState TransformState;
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TileElevationState;
		typedef WorldState<thoreau::TileCoordinate, bool> DirtyTileState;
		StaticWorldUpdateBlock(
			WorldUpdateManager& world_update_manager,
			DirtyTileState* dirty_tile_state,
			const TransformState* transform_state,
			const TileElevationState* tile_elevation_state);
		~StaticWorldUpdateBlock() = default;

		typedef WorldState<thoreau::TileCoordinate, MaterialSet> MaterialsState;
		const MaterialsState* get_materials_state() const;

		typedef WorldState<thoreau::TileCoordinate, TileSpacePointCloud> TileSpacePointCloudState;
		const TileSpacePointCloudState* get_point_cloud_state() const;

	private:
		DirtyTileState* dirty_tile_state;
		const TransformState* transform_state;
		const TileElevationState* tile_elevation_state;

		MaterialsState materials_state;
		TileElevationState scenery_elevation_state;
		TileElevationState relative_scenery_elevation_state;
		TileSpacePointCloudState point_cloud_state;

		PropWorldUpdatePass prop_pass;
		MaterialsWorldUpdatePass materials_pass;
	};
}

#endif
