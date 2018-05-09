// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_GAME_QUEST_HPP
#define MAPP_GAME_QUEST_HPP

#include <string>
#include <vector>
#include "twoflower/relationships/resource.hpp"

namespace mapp
{
	struct Quest
	{
		twoflower::Resource resource;
	};

	struct QuestDefinition
	{
		std::string name;

		QuestDefinition() = default;
		QuestDefinition(const Quest& value);
		QuestDefinition operator =(const Quest& value);
	};
}

inline mapp::QuestDefinition::QuestDefinition(const Quest& value)
{
	*this = value;
}

inline mapp::QuestDefinition mapp::QuestDefinition::operator =(const Quest& value)
{
	name = value.resource.get_name();
	return *this;
}

#endif
