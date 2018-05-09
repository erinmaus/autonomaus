// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "glooper/npc/npcCabinet.hpp"

bool glooper::NPCCabinet::has(NPC::ID id) const
{
	return npcs.count(id) != 0;
}

bool glooper::NPCCabinet::was_removed(NPC::ID id) const
{
	return !has(id) && id <= this->max_id && id != NPC::ID_NONE;
}

glooper::NPC& glooper::NPCCabinet::get(NPC::ID id)
{
	assert(has(id));
	return this->npcs.find(id)->second;
}

const glooper::NPC& glooper::NPCCabinet::get(NPC::ID id) const
{
	assert(has(id));
	return this->npcs.find(id)->second;
}

glooper::NPC& glooper::NPCCabinet::add()
{
	auto id = this->current_id++;
	auto& npc = this->npcs.emplace(id, NPC(id)).first->second;

	return npc;
}

void glooper::NPCCabinet::remove(NPC::ID id)
{
	this->npcs.erase(id);
}

glooper::NPCCabinet::const_iterator glooper::NPCCabinet::begin() const
{
	return this->npcs.begin();
}

glooper::NPCCabinet::const_iterator glooper::NPCCabinet::end() const
{
	return this->npcs.end();
}
