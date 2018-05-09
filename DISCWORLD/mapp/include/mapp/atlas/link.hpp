// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_ATLAS_LINK_HPP
#define MAPP_ATLAS_LINK_HPP

#include <vector>
#include "mapp/atlas/location.hpp"
#include "twoflower/relationships/action.hpp"

namespace mapp
{
	enum class TravelType
	{
		none,
		walk,
		teleport,
		shortcut,
		ship,
		object,
		npc
	};

	std::string get_travel_type_literal(TravelType type);
	TravelType get_travel_type_from_literal(const std::string& literal);

	struct Link
	{
		twoflower::Action action;
		Location destination;
		TravelType type;
	};

	typedef std::vector<Link> Links;
}

#endif
