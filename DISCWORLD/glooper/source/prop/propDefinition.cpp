// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "glooper/prop/propDefinition.hpp"
#include "glooper/prop/propManager.hpp"

glooper::PropDefinition::PropDefinition(
	const std::string& name, PropManager& manager) :
		scope(new Scope(name, manager))
{
	// Nothing.
}

bool glooper::PropDefinition::get_is_valid() const
{
	if (scope)
	{
		assert(scope->manager != nullptr);
		return scope->manager->has_definition(this->scope->name);
	}

	return false;
}

std::string glooper::PropDefinition::get_name() const
{
	if (scope)
	{
		return scope->name;
	}

	return std::string();
}

glooper::PropDefinition::Scope::Scope(const std::string& name, PropManager& manager)
{
	this->name = name;
	this->manager = &manager;
}
