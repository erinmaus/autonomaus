// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_ACTOR_POINT_CLOUD_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_ACTOR_POINT_CLOUD_WORLD_UPDATE_PASS_HPP

#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <unordered_map>
#include <glm/glm.hpp>
#include "chomsky/resources/tileSpacePointCloud.hpp"
#include "chomsky/world/actorDraw.hpp"
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/worldCoordinate.hpp"

namespace chomsky
{
	class ActorPointCloudWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TileElevationState;
		typedef WorldState<thoreau::TileCoordinate, TileSpacePointCloud> ActorPointCloud;
		typedef std::shared_ptr<ActorPointCloud> ActorPointCloudPointer;
		typedef WorldState<thoreau::WorldCoordinate, ActorPointCloudPointer> ActorPointCloudState;
		typedef WorldState<thoreau::WorldCoordinate, ActorDraw> ActorDrawState;

		ActorPointCloudWorldUpdatePass(
			const TileElevationState* tile_elevation_state,
			ActorDrawState* actor_draw_state,
			ActorPointCloudState* actor_point_cloud_state);
		~ActorPointCloudWorldUpdatePass() = default;

		void start() override;
		void stop() override;

		void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;

		void invalidate_model(const kvre::Model* model) override;

	private:
		const TileElevationState* tile_elevation_state;
		ActorDrawState* actor_draw_state;
		ActorPointCloudState* actor_point_cloud_state;

		std::unordered_map<const kvre::Model*, ActorPointCloudPointer> working_point_clouds;
		std::unordered_multimap<thoreau::WorldCoordinate, ActorPointCloudPointer> working_static_attachments;
		std::unordered_multimap<thoreau::WorldCoordinate, const kvre::Model*> working_static_models;

		typedef std::set<const kvre::Model*> ModelSet;
		std::map<ModelSet, ActorPointCloudPointer> finalized_point_clouds;
	};
}

#endif
