// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_BATCHED_MODEL_MANAGER_HPP
#define PRINCIPLE_BATCHED_MODEL_MANAGER_HPP

#include <map>
#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvre/model/batchedModelDraws.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace principle
{
	class BatchedModelManager : public chomsky::WorldUpdatePass
	{
	public:
		BatchedModelManager() = default;
		~BatchedModelManager() = default;

		bool has(int tag) const;
		const kvre::BatchedModelDraws* get(int tag) const;

		void draw_model(
			int tag, const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;
		void invalidate_tag(int tag) override;

	private:
		std::map<int, kvre::BatchedModelDraws> draws;
	};
}

#endif
