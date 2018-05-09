// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/waterElevationWorldUpdatePass.hpp"
#include "thoreau/sectorCoordinate.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/thoreau.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"

chomsky::WaterElevationWorldUpdatePass::WaterElevationWorldUpdatePass(
	const TransformState* transform_state, TileElevationState* state) :
		TileElevationWorldUpdatePass(state)
{
	this->transform_state = transform_state;
}

void chomsky::WaterElevationWorldUpdatePass::draw_water(
	int tag, const kvre::Terrain* water,
	const kompvter::ModelDrawEvent& event)
{
	if (has_tag(tag))
	{
		return;
	}

	auto& mapper = event.get_vertex_index_mapper();
	auto& draw_parameters = event.get_draw_parameters();
	auto begin = kvre::IndexedVertexIterator::begin(mapper, draw_parameters);
	auto end = kvre::IndexedVertexIterator::end(mapper, draw_parameters);

	glm::vec3 translation;
	{
		auto transformed_origin =
			event.get_model_matrix() * glm::vec4(glm::vec3(0.0f), 1.0f);
		translation = glm::vec3(transformed_origin);
	}
	auto water_sector = thoreau::sector_from_world(translation);

	static const glm::vec3 OFFSETS[]
	{
		{ 0.0f, 0.0f, 0.0f },
		{ -1.0f, 0.0f, 0.0f },
		{ 1.0f, 0.0f, 0.0f },
		{ 0.0f, 0.0f, -1.0f },
		{ 0.0f, 0.0f, 1.0f }
	};
	static const int NUM_OFFSETS = sizeof(OFFSETS) / sizeof(glm::vec3);

	auto& points = this->transform_state->get(tag);
	auto& working_state = get_working_state();
	for (auto i = begin; i != end; ++i)
	{
		/// XXX: Is this an [R...E] bug or a Chomsky bug?
		if (*i >= points->size())
		{
			continue;
		}

		auto position = points[*i];

		for (int j = 0; j < NUM_OFFSETS; ++j)
		{
			auto tile_coordinate = thoreau::tile_from_world(
				position + OFFSETS[j]);
			auto tile_sector = thoreau::sector_from_tile(tile_coordinate);
			if (tile_sector == water_sector)
			{
				auto& tile_group = working_state.add(tile_coordinate);
				tile_group.test(tag, position.y);
			}
		}
	}

	add_sector(tag, thoreau::sector_from_world(translation));
}
