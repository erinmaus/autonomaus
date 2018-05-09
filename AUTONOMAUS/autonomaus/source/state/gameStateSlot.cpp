// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/state/gameStateSlot.hpp"

const autonomaus::BasicGameStateSlot::ID autonomaus::BasicGameStateSlot::ID_NONE = 0;
const autonomaus::BasicGameStateSlot::ID autonomaus::BasicGameStateSlot::ID_BASE = 1;

autonomaus::BasicGameStateSlot::BasicGameStateSlot(
	GameStateManager& manager, ID id)
{
	this->manager = &manager;
	this->id = id;
}


autonomaus::GameStateManager&
autonomaus::BasicGameStateSlot::get_manager() const
{
	return *this->manager;
}

autonomaus::BasicGameStateSlot::ID
autonomaus::BasicGameStateSlot::get_id() const
{
	return this->id;
}
