// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/query/actorTileSpacePointCloudMatchExecutor.hpp"

chomsky::ActorTileSpacePointCloudMatchExecutor::ActorTileSpacePointCloudMatchExecutor(
	const AnimatedWorldUpdateBlock* animated_block)
{
	this->animated_block = animated_block;
}

bool chomsky::ActorTileSpacePointCloudMatchExecutor::execute(
	const thoreau::WorldCoordinate& location,
	const TileSpacePointCloudMatch& match) const
{
	auto point_cloud_state = this->animated_block->get_actor_point_cloud_state();
	if (!point_cloud_state->has(location))
	{
		return false;
	}

	auto& point_cloud = point_cloud_state->get(location);
	for (auto& i: match)
	{
		auto offset = i.first;
		auto& query_tile_point_cloud = i.second;
		if (!point_cloud->has(offset))
		{
			return false;
		}

		auto& target_tile_point_cloud = point_cloud->get(offset);
		if (!target_tile_point_cloud.contains(query_tile_point_cloud))
		{
			return false;
		}
	}

	return true;
}
