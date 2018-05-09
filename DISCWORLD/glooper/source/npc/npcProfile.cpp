// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "glooper/npc/npcProfile.hpp"
#include "glooper/npc/npcManager.hpp"

glooper::NPCProfile::NPCProfile(
	const std::string& name, NPCManager& manager) :
		scope(new Scope(name, manager))
{
	// Nothing.
}

bool glooper::NPCProfile::get_is_valid() const
{
	if (scope)
	{
		assert(scope->manager != nullptr);
		return scope->manager->has_profile(this->scope->name);
	}

	return false;
}

std::string glooper::NPCProfile::get_name() const
{
	if (scope)
	{
		return scope->name;
	}

	return std::string();
}

glooper::NPCProfile::Scope::Scope(const std::string& name, NPCManager& manager)
{
	this->name = name;
	this->manager = &manager;
}
