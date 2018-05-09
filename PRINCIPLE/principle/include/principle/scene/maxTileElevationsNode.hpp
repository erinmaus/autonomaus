// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_MAX_TILE_ELEVATIONS_NODE_HPP
#define PRINCIPLE_SCENE_MAX_TILE_ELEVATIONS_NODE_HPP

#include "principle/scene/tileElevationsNode.hpp"

namespace principle
{
	class MaxTileElevationsNode : public TileElevationsNode
	{
	public:
		MaxTileElevationsNode(
			Scene* scene,
			const GameManager::TileElevationState* tile_elevation_state);
		~MaxTileElevationsNode() = default;

		float get_elevation(float min_y, float max_y) const;
	};
}

inline principle::MaxTileElevationsNode::MaxTileElevationsNode(
	Scene* scene,
	const GameManager::TileElevationState* tile_elevation_state) :
		TileElevationsNode(scene, tile_elevation_state)
{
	// Nothing.
}

inline float principle::MaxTileElevationsNode::get_elevation(
	float min_y, float max_y) const
{
	return max_y;
}

#endif
