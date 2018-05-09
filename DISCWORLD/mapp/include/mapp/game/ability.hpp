// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_GAME_ABILITY_HPP
#define MAPP_GAME_ABILITY_HPP

#include <string>
#include <vector>
#include "twoflower/relationships/resource.hpp"

namespace mapp
{
	struct Ability
	{
		twoflower::Resource resource;
	};

	struct AbilityDefinition
	{
		std::string name;

		AbilityDefinition() = default;
		AbilityDefinition(const Ability& value);
		AbilityDefinition operator =(const Ability& value);
	};
}

inline mapp::AbilityDefinition::AbilityDefinition(const Ability& value)
{
	*this = value;
}

inline mapp::AbilityDefinition mapp::AbilityDefinition::operator =(const Ability& value)
{
	name = value.resource.get_name();
	return *this;
}

#endif
