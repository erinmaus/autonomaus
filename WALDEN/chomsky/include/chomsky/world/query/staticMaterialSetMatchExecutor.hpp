// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_QUERY_STATIC_MATERIAL_SET_MATCH_EXECUTOR_HPP
#define CHOMSKY_WORLD_QUERY_STATIC_MATERIAL_SET_MATCH_EXECUTOR_HPP

#include "chomsky/resources/textureFingerprintManager.hpp"
#include "chomsky/world/blocks/staticWorldUpdateBlock.hpp"
#include "chomsky/world/query/materialSetMatch.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace chomsky
{
	class StaticMaterialSetMatchExecutor
	{
	public:
		StaticMaterialSetMatchExecutor(
			const StaticWorldUpdateBlock* static_block,
			const TextureFingerprintManager& texture_fingerprint_manager);
		~StaticMaterialSetMatchExecutor() = default;

		bool execute(
			const thoreau::TileCoordinate& location,
			const MaterialSetMatch& match) const;

	private:
		const StaticWorldUpdateBlock* static_block;
		const TextureFingerprintManager* texture_fingerprint_manager;
	};
}

#endif
