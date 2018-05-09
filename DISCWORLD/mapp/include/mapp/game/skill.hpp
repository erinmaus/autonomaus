// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_GAME_SKILL_HPP
#define MAPP_GAME_SKILL_HPP

#include <string>
#include <vector>
#include "twoflower/relationships/resource.hpp"

namespace mapp
{
	struct Skill
	{
		twoflower::Resource resource;
	};

	struct SkillDefinition
	{
		std::string name;

		SkillDefinition() = default;
		SkillDefinition(const Skill& value);
		SkillDefinition operator =(const Skill& value);
	};

	enum class SkillXPCurve
	{
		normal,
		elite
	};
	int xp_for_level(int level, SkillXPCurve curve = SkillXPCurve::normal);

	typedef std::vector<Skill> Skills;
}

inline mapp::SkillDefinition::SkillDefinition(const Skill& value)
{
	*this = value;
}

inline mapp::SkillDefinition mapp::SkillDefinition::operator =(const Skill& value)
{
	name = value.resource.get_name();
	return *this;
}

#endif
