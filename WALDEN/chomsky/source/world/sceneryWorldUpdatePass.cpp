// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/sceneryWorldUpdatePass.hpp"
#include "chomsky/world/detail/common.hpp"
#include "chomsky/world/detail/scenery.hpp"

chomsky::SceneryWorldUpdatePass::SceneryWorldUpdatePass(
	const TransformState* transform_state,
	const TileElevationState* scenery_elevation_state,
	TileCloudState* scenery_point_cloud_state)
{
	this->transform_state = transform_state;
	this->scenery_elevation_state = scenery_elevation_state;
	this->scenery_point_cloud_state = scenery_point_cloud_state;
}

void chomsky::SceneryWorldUpdatePass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (!is_scenery_draw_event(event))
	{
		auto iter = this->model_tags.find(tag);
		if (iter != this->model_tags.end())
		{
			invalidate_tag(tag);
		}

		return;
	}

	if (this->model_tags.find(tag) != this->model_tags.end())
	{
		return;
	}

	add_model_to_tile_space_point_cloud_state(
		tag, 
		*this->transform_state->get(tag), *model,
		event.get_vertex_index_mapper(),
		event.get_draw_parameters(),
		event.get_model_matrix(),
		true,
		*this->scenery_elevation_state,
		nullptr,
		*this->scenery_point_cloud_state);
	this->model_tags.insert(tag);
}

void chomsky::SceneryWorldUpdatePass::invalidate_tag(int tag)
{
	auto iter = this->model_tags.find(tag);
	if (iter != this->model_tags.end())
	{
		for (auto& tile_cloud: *this->scenery_point_cloud_state)
		{
			tile_cloud.second.remove_all(tag);
		}

		this->model_tags.erase(iter);
	}
}
