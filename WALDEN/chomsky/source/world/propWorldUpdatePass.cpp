// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/propWorldUpdatePass.hpp"
#include "chomsky/world/detail/common.hpp"
#include "chomsky/world/detail/prop.hpp"

chomsky::PropWorldUpdatePass::PropWorldUpdatePass(
	const TransformState* transform_state,
	const TileElevationState* tile_elevation_state,
	DirtyTileState* dirty_tile_state,
	TileCloudState* prop_cloud_state)
{
	this->transform_state = transform_state;
	this->tile_elevation_state = tile_elevation_state;
	this->dirty_tile_state = dirty_tile_state;
	this->prop_cloud_state = prop_cloud_state;
}

void chomsky::PropWorldUpdatePass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (!is_prop_draw_event(event))
	{
		auto iter = this->model_tags.find(tag);
		if (iter != this->model_tags.end())
		{
			invalidate_tag(tag);
		}

		return;
	}

	if (!this->transform_state->has(tag))
	{
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
		false,
		*this->tile_elevation_state,
		this->dirty_tile_state,
		*this->prop_cloud_state);
	this->model_tags.insert(tag);
}

void chomsky::PropWorldUpdatePass::invalidate_tag(int tag)
{
	auto iter = this->model_tags.find(tag);
	if (iter != this->model_tags.end())
	{
		for (auto i = this->prop_cloud_state->begin(); i != this->prop_cloud_state->end(); /* Nop. */)
		{
			i->second.remove_all(tag);
			if (i->second.count() == 0)
			{
				i = this->prop_cloud_state->remove(i);
			}
			else
			{
				++i;
			}
		}
		for (auto& tile_cloud: *this->prop_cloud_state)
		{
			tile_cloud.second.remove_all(tag);
		}

		this->model_tags.erase(iter);
	}
}
