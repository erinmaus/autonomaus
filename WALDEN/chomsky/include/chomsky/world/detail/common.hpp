// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_DETAIL_COMMON_HPP
#define CHOMSKY_WORLD_DETAIL_COMMON_HPP

#include <glm/glm.hpp>
#include "chomsky/world/worldState.hpp"
#include "kompvter/graphics/drawParameters.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvre/model/model.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace chomsky
{
	class TileElevationExtents;
	class TileSpacePointCloud;
	class TerrainElevationWorldUpdatePass;

	void add_model_to_tile_space_point_cloud_state(
		int tag,
		const std::vector<glm::vec3>& points,
		const kvre::Model& model,
		const kompvter::VertexIndexMapper& index_mapper,
		const kompvter::DrawParameters& draw_parameters,
		const glm::mat4& model_matrix,
		bool translate_y,
		const WorldState<thoreau::TileCoordinate, TileElevationExtents>& scenery_elevation_state,
		WorldState<thoreau::TileCoordinate, bool>* dirty_tile_state,
		WorldState<thoreau::TileCoordinate, TileSpacePointCloud>& tile_space_point_cloud_state);

	thoreau::TileCoordinate world_position_to_tile(
		const glm::vec3& world_position,
		const WorldState<thoreau::TileCoordinate, TileElevationExtents>& tile_elevation_state);
}

#endif
