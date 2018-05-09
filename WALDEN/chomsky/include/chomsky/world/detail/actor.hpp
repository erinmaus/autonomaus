// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_DETAIL_ACTOR_HPP
#define CHOMSKY_WORLD_DETAIL_ACTOR_HPP

#include "chomsky/world/worldState.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kvre/model/model.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/worldCoordinate.hpp"

namespace chomsky
{
	class Silhouette;
	class TileElevationExtents;
	class TileSpacePointCloud;
	class TerrainElevationWorldUpdatePass;

	bool is_actor_draw_event(const kompvter::ModelDrawEvent& event);

	void project_actor_silhouette_weight(
		const kvre::Model& model,
		const kompvter::ModelDrawEvent& event,
		const kvlc::Bounds& bounds,
		bool apply_bone_transforms,
		Silhouette& result);
	void project_actor_silhouette_colors(
		const kvre::Model& model,
		const kompvter::ModelDrawEvent& event,
		const kvlc::Bounds& bounds,
		Silhouette& red, Silhouette& green, Silhouette& blue);

	thoreau::WorldCoordinate get_actor_position(
		const glm::mat4& model_matrix,
		const WorldState<thoreau::TileCoordinate, TileElevationExtents>* tile_elevation_state);
}

#endif
