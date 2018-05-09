// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <future>
#include "glooper/map/mapWorldUpdateBlock.hpp"

glooper::MapWorldUpdateBlock::MapWorldUpdateBlock(
	chomsky::WorldUpdateManager& world_update_manager,
	const thoreau::Map* map,
	const TransformState* transform_state,
	const kvlc::Bounds* bounds,
	thoreau::Map* runtime_map) :
		WorldUpdateBlock(world_update_manager),
		collision_scene_pass(transform_state, bounds, &collision_scene, &scene_tag_draw_tag_map),
		terrain_elevation_pass(transform_state, &terrain_tile_elevation_state),
		water_elevation_pass(transform_state, &water_tile_elevation_state),
		map_elevation_pass(&terrain_tile_elevation_state, &water_tile_elevation_state, map, &tile_elevation_state, &runtime_sector_flag_state, runtime_map)
{
	this->map = map;
	this->transform_state = transform_state;
	this->bounds = bounds;
	this->runtime_map = runtime_map;

	register_pass(&collision_scene_pass);
	register_pass(&terrain_elevation_pass);
	register_pass(&water_elevation_pass);
	register_pass(&map_elevation_pass);
}

const glooper::MapWorldUpdateBlock::TileElevationState*
glooper::MapWorldUpdateBlock::get_tile_elevation_state() const
{
	return &this->tile_elevation_state;
}

const glooper::MapWorldUpdateBlock::TileElevationState*
glooper::MapWorldUpdateBlock::get_water_elevation_state() const
{
	return &this->tile_elevation_state;
}

const glooper::MapWorldUpdateBlock::SceneTagDrawTagMap*
glooper::MapWorldUpdateBlock::get_scene_tag_draw_tag_map() const
{
	return &this->scene_tag_draw_tag_map;
}

const glooper::MapWorldUpdateBlock::RuntimeSectorFlagState*
glooper::MapWorldUpdateBlock::get_runtime_sector_flag_state() const
{
	return &this->runtime_sector_flag_state;
}

const kvlc::Scene* glooper::MapWorldUpdateBlock::get_collision_scene() const
{
	return &this->collision_scene;
}

void glooper::MapWorldUpdateBlock::update()
{
	auto collision = std::async(
		std::launch::async,
		[this]
		{
			this->get_world_update_manager()->update_pass(&this->collision_scene_pass);
		});

	auto terrain = std::async(
		std::launch::async,
		[this]
		{
			this->get_world_update_manager()->update_pass(&this->terrain_elevation_pass);
		});
	auto water = std::async(
		std::launch::async,
		[this]
		{
			this->get_world_update_manager()->update_pass(&this->water_elevation_pass);
		});
	water.wait();
	terrain.wait();

	get_world_update_manager()->update_pass(&this->map_elevation_pass);
	collision.wait();
}
