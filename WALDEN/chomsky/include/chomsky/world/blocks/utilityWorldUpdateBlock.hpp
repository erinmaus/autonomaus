// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_BLOCKS_UTILITY_WORLD_UPDATE_BLOCK_HPP
#define CHOMSKY_WORLD_BLOCKS_UTILITY_WORLD_UPDATE_BLOCK_HPP

#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/boundsWorldUpdatePass.hpp"
#include "chomsky/world/worldUpdateManager.hpp"
#include "chomsky/world/blocks/worldUpdateBlock.hpp"

namespace chomsky
{
	class UtilityWorldUpdateBlock : public WorldUpdateBlock
	{
	public:
		UtilityWorldUpdateBlock(WorldUpdateManager& world_update_manager);
		~UtilityWorldUpdateBlock() = default;

		typedef TransformWorldUpdatePass::TransformState TransformState;
		const TransformState* get_transform_state() const;

		const kvlc::Bounds* get_bounds() const;

	private:
		TransformState transform_state;
		kvlc::Bounds bounds;

		TransformWorldUpdatePass transform_pass;
		BoundsWorldUpdatePass bounds_pass;
	};
}

#endif
