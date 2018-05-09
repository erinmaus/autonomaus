// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/blocks/worldUpdateBlock.hpp"

chomsky::WorldUpdateBlock::WorldUpdateBlock(
	WorldUpdateManager& world_update_manager)
{
	this->world_update_manager = &world_update_manager;
}

chomsky::WorldUpdateBlock::~WorldUpdateBlock()
{
	for (auto& pass: this->passes)
	{
		this->world_update_manager->remove_pass(pass);
	}
}

void chomsky::WorldUpdateBlock::update()
{
	for (auto& pass: this->passes)
	{
		this->world_update_manager->update_pass(pass);
	}
}

chomsky::WorldUpdateManager*
chomsky::WorldUpdateBlock::get_world_update_manager() const
{
	return this->world_update_manager;
}

void chomsky::WorldUpdateBlock::register_pass(WorldUpdatePass* pass)
{
	this->world_update_manager->add_pass(pass);
	this->passes.push_back(pass);
}
