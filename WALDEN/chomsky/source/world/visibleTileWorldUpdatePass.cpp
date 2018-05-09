// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "chomsky/world/visibleTileWorldUpdatePass.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"

chomsky::VisibleTileWorldUpdatePass::VisibleTileWorldUpdatePass(
	const TransformState* transform_state,
	const TileElevationState* tile_elevation_state,
	VisibleTileState* visible_tile_state)
{
	this->transform_state = transform_state;
	this->tile_elevation_state = tile_elevation_state;
	this->visible_tile_state = visible_tile_state;
}

void chomsky::VisibleTileWorldUpdatePass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	check(tag, event);
}

void chomsky::VisibleTileWorldUpdatePass::draw_terrain(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	check(tag, event);
}

void chomsky::VisibleTileWorldUpdatePass::check(
	int tag, const kompvter::ModelDrawEvent& event)
{
	if (!this->transform_state->has(tag))
	{
		return;
	}

	auto& points = this->transform_state->get(tag);
	auto current = kvre::IndexedVertexIterator::begin(
		event.get_vertex_index_mapper(), event.get_draw_parameters());
	auto end = kvre::IndexedVertexIterator::end(
		event.get_vertex_index_mapper(), event.get_draw_parameters());
	auto view_projection_matrix = event.get_projection_matrix() * event.get_view_matrix();

	while (current != end)
	{
		auto position = points[*current];
		++current;

		for (float x = -1; x <= 1; ++x)
		{
			for (float z = -1; z <= 1; ++z)
			{
				auto offset = glm::vec3(x * 256, 0.0f, z * 256);
				auto adjusted_position = position + offset;

				auto tile_coordinate = thoreau::tile_from_world(adjusted_position);
				if (this->tile_elevation_state->has(tile_coordinate))
				{
					auto& extents = this->tile_elevation_state->get(tile_coordinate);
					if (!extents.try_find_tag(position.y, tile_coordinate.layer))
					{
						continue;
					}
				}

				auto p = view_projection_matrix * glm::vec4(adjusted_position, 1.0f);
				p /= p.w;
				if (p.x < -1.0f || p.x > 1.0f ||
					p.y < -1.0f || p.y > 1.0f ||
					p.z < -1.0f || p.z > 1.0f)
				{
					continue;
				}

				this->visible_tile_state->add(tile_coordinate) = true;
			}
		}
	}
}
