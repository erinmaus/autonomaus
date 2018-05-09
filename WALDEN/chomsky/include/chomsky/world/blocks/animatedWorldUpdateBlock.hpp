// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_BLOCKS_ANIMATED_WORLD_UPDATE_BLOCK_HPP
#define CHOMSKY_WORLD_BLOCKS_ANIMATED_WORLD_UPDATE_BLOCK_HPP

#include "chomsky/world/actorBoundsWorldUpdatePass.hpp"
#include "chomsky/world/actorMaterialsWorldUpdatePass.hpp"
#include "chomsky/world/actorDraw.hpp"
#include "chomsky/world/actorDrawWorldUpdatePass.hpp"
#include "chomsky/world/actorPointCloudWorldUpdatePass.hpp"
#include "chomsky/world/materialSet.hpp"
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/worldUpdateManager.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/world/blocks/worldUpdateBlock.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/worldCoordinate.hpp"

namespace chomsky
{
	class AnimatedWorldUpdateBlock : public WorldUpdateBlock
	{
	public:
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TileElevationState;
		AnimatedWorldUpdateBlock(
			WorldUpdateManager& world_update_manager,
			const TileElevationState* tile_elevation_state);
		~AnimatedWorldUpdateBlock() = default;

		typedef WorldState<thoreau::WorldCoordinate, kvlc::Bounds> ActorBoundsState;
		const ActorBoundsState* get_actor_bounds_state() const;

		typedef WorldState<thoreau::WorldCoordinate, ActorDraw> ActorDrawState;
		const ActorDrawState* get_actor_draw_state() const;

		typedef WorldState<thoreau::WorldCoordinate, MaterialSet> ActorMaterialsState;
		const ActorMaterialsState* get_actor_materials_state() const;

		typedef ActorPointCloudWorldUpdatePass::ActorPointCloud ActorPointCloud;
		typedef ActorPointCloudWorldUpdatePass::ActorPointCloudPointer ActorPointCloudPointer;
		typedef ActorPointCloudWorldUpdatePass::ActorPointCloudState ActorPointCloudState;
		const ActorPointCloudState* get_actor_point_cloud_state() const;

	private:
		const TileElevationState* tile_elevation_state;

		ActorBoundsState actor_bounds_state;
		ActorDrawState actor_draw_state;
		ActorMaterialsState actor_materials_state;
		ActorPointCloudState actor_point_cloud_state;

		ActorBoundsWorldUpdatePass actor_bounds_pass;
		ActorDrawWorldUpdatePass actor_draw_pass;
		ActorMaterialsWorldUpdatePass actor_materials_pass;
		ActorPointCloudWorldUpdatePass actor_point_cloud_pass;
	};
}

#endif
