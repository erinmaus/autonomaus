// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_QUERY_STATIC_TILE_SPACE_POINT_CLOUD_MATCH_EXECUTOR_HPP
#define CHOMSKY_WORLD_QUERY_STATIC_TILE_SPACE_POINT_CLOUD_MATCH_EXECUTOR_HPP

#include "chomsky/resources/tileSpacePointCloud.hpp"
#include "chomsky/world/blocks/staticWorldUpdateBlock.hpp"
#include "chomsky/world/query/tileSpacePointCloudMatch.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace chomsky
{
	class StaticTileSpacePointCloudMatchExecutor
	{
	public:
		StaticTileSpacePointCloudMatchExecutor(
			const StaticWorldUpdateBlock* static_block);
		~StaticTileSpacePointCloudMatchExecutor() = default;

		bool execute(
			const thoreau::TileCoordinate& location,
			const TileSpacePointCloudMatch& match) const;

	private:
		const StaticWorldUpdateBlock* static_block;
	};
}

#endif
