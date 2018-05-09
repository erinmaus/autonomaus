// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/blocks/utilityWorldUpdateBlock.hpp"

chomsky::UtilityWorldUpdateBlock::UtilityWorldUpdateBlock(
	WorldUpdateManager& world_update_manager) :
		WorldUpdateBlock(world_update_manager),
		transform_pass(&transform_state),
		bounds_pass(&transform_state, &bounds)
{
	register_pass(&transform_pass);
	register_pass(&bounds_pass);
}

const chomsky::UtilityWorldUpdateBlock::TransformState*
chomsky::UtilityWorldUpdateBlock::get_transform_state() const
{
	return &this->transform_state;
}

const kvlc::Bounds* chomsky::UtilityWorldUpdateBlock::get_bounds() const
{
	return &this->bounds;
}
