// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/blocks/animatedWorldUpdateBlock.hpp"

chomsky::AnimatedWorldUpdateBlock::AnimatedWorldUpdateBlock(
	WorldUpdateManager& world_update_manager,
	const TileElevationState* tile_elevation_state) :
		WorldUpdateBlock(world_update_manager),
		actor_bounds_pass(tile_elevation_state, &actor_bounds_state),
		actor_draw_pass(tile_elevation_state, &actor_draw_state),
		actor_materials_pass(tile_elevation_state, &actor_materials_state),
		actor_point_cloud_pass(tile_elevation_state, &actor_draw_state, &actor_point_cloud_state)
{
	this->tile_elevation_state = tile_elevation_state;

	register_pass(&this->actor_bounds_pass);
	register_pass(&this->actor_draw_pass);
	register_pass(&this->actor_materials_pass);
	register_pass(&this->actor_point_cloud_pass);
}

const chomsky::AnimatedWorldUpdateBlock::ActorBoundsState*
chomsky::AnimatedWorldUpdateBlock::get_actor_bounds_state() const
{
	return &this->actor_bounds_state;
}

const chomsky::AnimatedWorldUpdateBlock::ActorDrawState*
chomsky::AnimatedWorldUpdateBlock::get_actor_draw_state() const
{
	return &this->actor_draw_state;
}

const chomsky::AnimatedWorldUpdateBlock::ActorMaterialsState*
chomsky::AnimatedWorldUpdateBlock::get_actor_materials_state() const
{
	return &this->actor_materials_state;
}

const chomsky::AnimatedWorldUpdateBlock::ActorPointCloudState*
chomsky::AnimatedWorldUpdateBlock::get_actor_point_cloud_state() const
{
	return &this->actor_point_cloud_state;
}
