// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/query/staticTileSpacePointCloudMatchExecutor.hpp"

chomsky::StaticTileSpacePointCloudMatchExecutor::StaticTileSpacePointCloudMatchExecutor(
	const StaticWorldUpdateBlock* static_block)
{
	this->static_block = static_block;
}

bool chomsky::StaticTileSpacePointCloudMatchExecutor::execute(
	const thoreau::TileCoordinate& location,
	const TileSpacePointCloudMatch& match) const
{
	auto point_cloud_state = this->static_block->get_point_cloud_state();

	for (auto& i: match)
	{
		auto tile_coordinate = location;
		auto offset = i.first;
		tile_coordinate.x += offset.x;
		tile_coordinate.y += offset.y;

		if (!point_cloud_state->has(tile_coordinate))
		{
			return false;
		}

		auto& query_tile_point_cloud = i.second;
		auto& point_cloud = point_cloud_state->get(tile_coordinate);
		if (!point_cloud.contains(query_tile_point_cloud))
		{
			return false;
		}
	}

	return true;
}
