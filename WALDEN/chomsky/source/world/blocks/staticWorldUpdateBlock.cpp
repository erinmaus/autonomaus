// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/blocks/staticWorldUpdateBlock.hpp"

chomsky::StaticWorldUpdateBlock::StaticWorldUpdateBlock(
	WorldUpdateManager& world_update_manager,
	DirtyTileState* dirty_tile_state,
	const TransformState* transform_state,
	const TileElevationState* tile_elevation_state) :
		WorldUpdateBlock(world_update_manager),
		prop_pass(transform_state, tile_elevation_state, dirty_tile_state, &point_cloud_state),
		materials_pass(tile_elevation_state, dirty_tile_state, &this->materials_state)
{
	this->dirty_tile_state = dirty_tile_state;
	this->transform_state = transform_state;
	this->tile_elevation_state = tile_elevation_state;

	register_pass(&this->prop_pass);
	register_pass(&this->materials_pass);
}

const chomsky::StaticWorldUpdateBlock::MaterialsState*
chomsky::StaticWorldUpdateBlock::get_materials_state() const
{
	return &this->materials_state;
}

const chomsky::StaticWorldUpdateBlock::TileSpacePointCloudState*
chomsky::StaticWorldUpdateBlock::get_point_cloud_state() const
{
	return &this->point_cloud_state;
}
