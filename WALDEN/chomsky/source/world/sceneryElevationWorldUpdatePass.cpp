// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <boost/container/flat_map.hpp>
#include "chomsky/world/sceneryElevationWorldUpdatePass.hpp"
#include "chomsky/world/detail/scenery.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/thoreau.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "kvre/profile/profile.hpp"

chomsky::SceneryElevationWorldUpdatePass::SceneryElevationWorldUpdatePass(
	const TransformState* transform_state,
	const TerrainElevationState* terrain_elevation_state,
	SceneryElevationState* scenery_elevation_state,
	SceneryElevationState* relative_scenery_elevation_state)
{
	this->transform_state = transform_state;
	this->terrain_elevation_state = terrain_elevation_state;
	this->scenery_elevation_state = scenery_elevation_state;
	this->relative_scenery_elevation_state = relative_scenery_elevation_state;
}

void chomsky::SceneryElevationWorldUpdatePass::start()
{
	if (this->relative_scenery_elevation_state != nullptr)
	{
		this->relative_scenery_elevation_state->clear();
	}
}

void chomsky::SceneryElevationWorldUpdatePass::stop()
{
	if (this->scenery_elevation_state != nullptr)
	{
		*this->scenery_elevation_state = working_scenery_elevation_state;
	}

	if (this->relative_scenery_elevation_state != nullptr)
	{
		for (auto& i: this->working_scenery_elevation_state)
		{
			auto& working_tile_coordinate = i.first;
			auto& working_tile = i.second;

			if (!this->terrain_elevation_state->has(working_tile_coordinate))
			{
				continue;
			}

			auto& terrain_tile = this->terrain_elevation_state->get(working_tile_coordinate);
			auto& scenery_tile = this->relative_scenery_elevation_state->add(working_tile_coordinate);

			typedef std::pair<float, float> MinMaxY;
			boost::container::flat_map<int, MinMaxY> tile_extents;
			tile_extents.reserve(terrain_tile.count());
			for (auto working_extent: working_tile)
			{
				int tag = std::get<TileElevationExtents::TAG>(working_extent);
				for (auto terrain_extent: terrain_tile)
				{
					float min_threshold, max_threshold;
					{
						int layer = std::get<TileElevationExtents::TAG>(terrain_extent);
						min_threshold = std::get<TileElevationExtents::MIN_Y>(terrain_extent);

						int next_layer = layer + 1;
						if (terrain_tile.has(next_layer))
						{
							max_threshold = terrain_tile.get_min(next_layer);
						}
						else
						{
							max_threshold = HUGE_VALF;
						}

						float min_elevation = working_tile.get_min_above(tag, min_threshold);
						float max_elevation = working_tile.get_max_below(tag, max_threshold);

						if (min_elevation == HUGE_VALF || max_elevation == HUGE_VALF)
						{
							continue;
						}

						auto current = tile_extents.find(layer);
						if (current == tile_extents.end())
						{
							auto min_max = std::make_pair(min_elevation, max_elevation);
							tile_extents.emplace(layer, min_max);
						}
						else
						{
							auto& min_max = current->second;
							min_max.first = std::min(min_max.first, min_elevation);
							min_max.second = std::max(min_max.second, max_elevation);
						}
					}
				}
			}

			scenery_tile.clear();
			for (auto extent: tile_extents)
			{
				scenery_tile.test(extent.first, extent.second.first);
				scenery_tile.test(extent.first, extent.second.second);
			}
		}
	}
}

void chomsky::SceneryElevationWorldUpdatePass::draw_model(
	int tag, const kvre::Model* model, const kompvter::ModelDrawEvent& event)
{
	if (!is_scenery_draw_event(event))
	{
		if (this->model_tags.find(tag) != this->model_tags.end())
		{
			invalidate_tag(tag);
		}

		return;
	}

	if (this->model_tags.count(tag) != 0)
	{
		return;
	}

	auto begin = kvre::IndexedVertexIterator::begin(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());
	auto end = kvre::IndexedVertexIterator::end(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());

	auto& points = this->transform_state->get(tag);
	for (auto& i = begin; i != end; ++i)
	{
		auto position = points[*i];
		auto tile_coordinate = thoreau::tile_from_world(glm::vec3(position));

		auto& tile = this->working_scenery_elevation_state.add(tile_coordinate);
		tile.test(tag, position.y);
	}

	this->model_tags.insert(tag);
}

void chomsky::SceneryElevationWorldUpdatePass::invalidate_tag(
	int tag)
{
	auto iter = this->model_tags.find(tag);
	if (iter != this->model_tags.end())
	{
		for (auto& tile: this->working_scenery_elevation_state)
		{
			tile.second.remove_tag(tag);
		}

		for (auto& tile: *this->scenery_elevation_state)
		{
			tile.second.remove_tag(tag);
		}

		this->model_tags.erase(iter);
	}
}
