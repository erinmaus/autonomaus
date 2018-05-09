// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_COLLISION_SCENE_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_COLLISION_SCENE_WORLD_UPDATE_PASS_HPP

#include <map>
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "kvlc/collision/scene.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/worldCoordinate.hpp"

namespace chomsky
{
	class CollisionSceneWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef TransformWorldUpdatePass::TransformState TransformState;
		typedef WorldState<kvlc::Scene::Tag /* scene_tag */, int /* draw_tag */> SceneTagDrawTagMap;

		CollisionSceneWorldUpdatePass(
			const TransformState* transform_state,
			const kvlc::Bounds* bounds,
			kvlc::Scene* scene,
			SceneTagDrawTagMap* scene_tag_draw_tag_map);

		void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;
		void draw_terrain(
			int tag,
			const kvre::Terrain* terrain,
			const kompvter::ModelDrawEvent& event) override;

		void invalidate_tag(int tag) override;

	private:
		void add_mesh(int tag, const kompvter::ModelDrawEvent& event);

		const TransformState* transform_state;
		const kvlc::Bounds* bounds;
		SceneTagDrawTagMap* scene_tag_draw_tag_map;
		kvlc::Scene* scene;

		std::map<int /* draw_tag */, kvlc::Scene::Tag /* scene_tag */> tags;
	};
}

#endif
