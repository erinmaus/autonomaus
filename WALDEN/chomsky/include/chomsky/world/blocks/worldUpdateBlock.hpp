// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_BLOCKS_WORLD_UPDATE_BLOCK_HPP
#define CHOMSKY_WORLD_BLOCKS_WORLD_UPDATE_BLOCK_HPP

#include "chomsky/world/worldUpdateManager.hpp"

namespace chomsky
{
	class WorldUpdateBlock
	{
	public:
		WorldUpdateBlock(WorldUpdateManager& world_update_manager);
		virtual ~WorldUpdateBlock();

		virtual void update();

	protected:
		WorldUpdateManager* get_world_update_manager() const;
		void register_pass(WorldUpdatePass* pass);

	private:
		WorldUpdateManager* world_update_manager;
		std::vector<WorldUpdatePass*> passes;
	};
}

#endif
