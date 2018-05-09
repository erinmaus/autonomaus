// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <vector>
#include <boost/container/flat_map.hpp>
#include "chomsky/resources/tileSpacePointCloud.hpp"
#include "chomsky/world/terrainElevationWorldUpdatePass.hpp"
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/detail/common.hpp"
#include "thoreau/thoreau.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"

void chomsky::add_model_to_tile_space_point_cloud_state(
	int tag,
	const std::vector<glm::vec3>& points,
	const kvre::Model& model,
	const kompvter::VertexIndexMapper& index_mapper,
	const kompvter::DrawParameters& draw_parameters,
	const glm::mat4& model_matrix,
	bool translate_y,
	const WorldState<thoreau::TileCoordinate, TileElevationExtents>& scenery_elevation_state,
	WorldState<thoreau::TileCoordinate, bool>* dirty_tile_state,
	WorldState<thoreau::TileCoordinate, TileSpacePointCloud>& tile_space_point_cloud_state)
{
	glm::vec3 translation;
	{
		auto transformed_origin = model_matrix * glm::vec4(glm::vec3(0.0f), 1.0f);
		translation = glm::vec3(transformed_origin);
	}

	struct Vertex
	{
		glm::vec3 position;
		glm::vec3 color;
	};
	boost::container::flat_map<std::size_t, Vertex> vertices;
	{
		auto begin = kvre::IndexedVertexIterator::begin(
			index_mapper, draw_parameters);
		auto end = kvre::IndexedVertexIterator::end(
			index_mapper, draw_parameters);
		vertices.reserve(std::distance(begin, end));

		for (auto& i = begin; i != end; ++i)
		{
			auto index = *i;

			Vertex vertex;
			vertex.position = model.get_position(index);
			vertex.color = glm::vec3(model.get_color(index));

			vertices[index] = vertex;
		}
	}

	auto center_coordinate = thoreau::tile_from_world(translation);
	if (scenery_elevation_state.has(center_coordinate))
	{
		auto& elevations = scenery_elevation_state.get(center_coordinate);
		elevations.try_find_tag(translation.y, center_coordinate.layer);
	}

	for (auto& i: vertices)
	{
		auto& vertex = i.second;
		auto position = vertex.position;
		auto color = vertex.color;

		auto relative_tile_coordinate = thoreau::tile_from_world(position);
		auto tile_relative_position = position;
		tile_relative_position.x -= relative_tile_coordinate.x * thoreau::TILE_SIZE;
		tile_relative_position.z -= relative_tile_coordinate.y * thoreau::TILE_SIZE;

		if (tile_relative_position.x < 0.0f)
		{
			tile_relative_position.x = thoreau::TILE_SIZE + tile_relative_position.x;
		}

		if (tile_relative_position.z < 0.0f)
		{
			tile_relative_position.z = thoreau::TILE_SIZE + tile_relative_position.z;
		}

		auto tile_coordinate = center_coordinate;
		tile_coordinate.x += relative_tile_coordinate.x;
		tile_coordinate.y += relative_tile_coordinate.y;

		auto& tile_cloud = tile_space_point_cloud_state.add(tile_coordinate);
		tile_cloud.add(tile_relative_position, color, tag);

		if (dirty_tile_state != nullptr)
		{
			auto& dirty_tile = dirty_tile_state->add(tile_coordinate);
			dirty_tile = true;
		}
	}
}

thoreau::TileCoordinate chomsky::world_position_to_tile(
	const glm::vec3& world_position,
	const WorldState<thoreau::TileCoordinate, TileElevationExtents>& tile_elevation_state)
{
	auto tile_coordinate = thoreau::tile_from_world(world_position);
	if (tile_elevation_state.has(tile_coordinate))
	{
		auto& elevations = tile_elevation_state.get(tile_coordinate);
		if (!elevations.try_find_tag(world_position.y, tile_coordinate.layer))
		{
			tile_coordinate.layer = 0;
		}
	}

	return tile_coordinate;
}
