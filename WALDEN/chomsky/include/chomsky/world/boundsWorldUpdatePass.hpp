// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_BOUNDS_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_BOUNDS_WORLD_UPDATE_PASS_HPP

#include <map>
#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"
#include "kvlc/geometry/bounds.hpp"

namespace chomsky
{
	class BoundsWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef TransformWorldUpdatePass::TransformState TransformState;

		BoundsWorldUpdatePass(
			const TransformState* transform_state,
			kvlc::Bounds* bounds);
		~BoundsWorldUpdatePass() = default;

		void stop() override;

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
		void expand(int tag, const kompvter::ModelDrawEvent& event);

		const TransformState* transform_state;
		kvlc::Bounds* bounds;
		std::map<int, kvlc::Bounds> tags;
	};
}

#endif
