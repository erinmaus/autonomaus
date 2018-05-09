// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_QUERY_ACTOR_MATERIAL_SET_MATCH_EXECUTOR_HPP
#define CHOMSKY_WORLD_QUERY_ACTOR_MATERIAL_SET_MATCH_EXECUTOR_HPP

#include "chomsky/resources/textureFingerprintManager.hpp"
#include "chomsky/world/blocks/animatedWorldUpdateBlock.hpp"
#include "chomsky/world/query/materialSetMatch.hpp"
#include "thoreau/worldCoordinate.hpp"

namespace chomsky
{
	class ActorMaterialSetMatchExecutor
	{
	public:
		ActorMaterialSetMatchExecutor(
			const AnimatedWorldUpdateBlock* animated_block,
			const TextureFingerprintManager& texture_fingerprint_manager);
		~ActorMaterialSetMatchExecutor() = default;

		bool execute(
			const thoreau::WorldCoordinate& location,
			const MaterialSetMatch& match) const;

	private:
		const AnimatedWorldUpdateBlock* animated_block;
		const TextureFingerprintManager* texture_fingerprint_manager;
	};
}

#endif
