// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_QUERY_ACTOR_TILE_SPACE_POINT_CLOUD_MATCH_EXECUTOR_HPP
#define CHOMSKY_WORLD_QUERY_ACTOR_TILE_SPACE_POINT_CLOUD_MATCH_EXECUTOR_HPP

#include "chomsky/resources/tileSpacePointCloud.hpp"
#include "chomsky/world/blocks/animatedWorldUpdateBlock.hpp"
#include "chomsky/world/query/tileSpacePointCloudMatch.hpp"
#include "thoreau/worldCoordinate.hpp"

namespace chomsky
{
	class ActorTileSpacePointCloudMatchExecutor
	{
	public:
		ActorTileSpacePointCloudMatchExecutor(
			const AnimatedWorldUpdateBlock* animated_block);
		~ActorTileSpacePointCloudMatchExecutor() = default;

		bool execute(
			const thoreau::WorldCoordinate& location,
			const TileSpacePointCloudMatch& match) const;

	private:
		const AnimatedWorldUpdateBlock* animated_block;
	};
}

#endif
