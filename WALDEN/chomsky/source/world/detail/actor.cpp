// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <map>
#include "chomsky/resources/silhouette.hpp"
#include "chomsky/resources/modelVertexCloud.hpp"
#include "chomsky/world/detail/actor.hpp"
#include "chomsky/world/terrainElevationWorldUpdatePass.hpp"
#include "chomsky/world/tileElevationExtents.hpp"
#include "thoreau/thoreau.hpp"
#include "kvre/draw/rangeVertexIterator.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"

bool chomsky::is_actor_draw_event(const kompvter::ModelDrawEvent& event)
{
	return !event.are_bone_transforms_identity();
}

void chomsky::project_actor_silhouette_weight(
	const kvre::Model& model,
	const kompvter::ModelDrawEvent& event,
	const kvlc::Bounds& bounds,
	bool apply_bone_transforms,
	chomsky::Silhouette& result)
{
	ModelVertexCloud vertex_cloud;
	{
		auto begin = kvre::IndexedVertexIterator::begin(
			event.get_vertex_index_mapper(), event.get_draw_parameters());
		auto end = kvre::IndexedVertexIterator::end(
			event.get_vertex_index_mapper(), event.get_draw_parameters());

		for (auto i = begin; i != end; ++i)
		{
			auto index = *i;
			ModelVertexCloud::Vertex vertex;
			vertex.position = model.get_position(index);
			vertex.color = glm::vec3(model.get_color(index));

			if (apply_bone_transforms)
			{
				int bone_index = model.get_bone(index);
				auto bone_transform = event.get_bone_transform(bone_index);

				auto transformed_position = glm::vec4(vertex.position, 1.0f);
				transformed_position = bone_transform * transformed_position;

				vertex.position = glm::vec3(transformed_position);
			}

			vertex_cloud.add(vertex);
		}
	}

	vertex_cloud.project_weight(
		result,
		glm::vec3(0.0f), bounds);
}

void chomsky::project_actor_silhouette_colors(
	const kvre::Model& model,
	const kompvter::ModelDrawEvent& event,
	const kvlc::Bounds& bounds,
	Silhouette& red, Silhouette& green, Silhouette& blue)
{
	ModelVertexCloud vertex_cloud;
	{
		auto begin = kvre::IndexedVertexIterator::begin(
			event.get_vertex_index_mapper(), event.get_draw_parameters());
		auto end = kvre::IndexedVertexIterator::end(
			event.get_vertex_index_mapper(), event.get_draw_parameters());
		vertex_cloud.extract(begin, end, model);
	}

	vertex_cloud.project_color_component(
		red,
		glm::vec3(0.0f), bounds,
		ModelVertexCloud::color_component_red);
	vertex_cloud.project_color_component(
		green,
		glm::vec3(0.0f), bounds,
		ModelVertexCloud::color_component_green);
	vertex_cloud.project_color_component(
		blue,
		glm::vec3(0.0f), bounds,
		ModelVertexCloud::color_component_blue);
}

thoreau::WorldCoordinate chomsky::get_actor_position(
	const glm::mat4& model_matrix,
	const WorldState<thoreau::TileCoordinate, TileElevationExtents>* tile_elevation_state)
{
	glm::vec3 world_position;
	{
		auto transformed_origin = model_matrix * glm::vec4(glm::vec3(0.0f), 1.0f);
		world_position = glm::vec3(transformed_origin);
	}

	thoreau::WorldCoordinate result;
	{
		result.x = world_position.x;
		result.y = world_position.z;
		result.layer = 0;

		if (tile_elevation_state != nullptr)
		{
			auto tile_coordinate = thoreau::tile_from_world(result);
			
			if (tile_elevation_state->has(tile_coordinate))
			{
				auto& elevations = tile_elevation_state->get(tile_coordinate);
				elevations.try_find_tag(world_position.y, result.layer);
			}
		}
	}

	return result;
}
