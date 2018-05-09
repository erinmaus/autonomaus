// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_GAME_ITEM_HPP
#define MAPP_GAME_ITEM_HPP

#include <string>
#include "twoflower/relationships/resource.hpp"

namespace mapp
{
	struct Item
	{
		twoflower::Resource resource;
		int object_id;
		int value;

		static const int NULL_OBJECT_ID = -1;
	};

	struct ItemDefinition
	{
		std::string name;
		int object_id = Item::NULL_OBJECT_ID;
		int value = 0;

		ItemDefinition() = default;
		ItemDefinition(const Item& item);
		ItemDefinition operator =(const Item& item);
	};

	typedef std::vector<Item> Items;
}

inline mapp::ItemDefinition::ItemDefinition(const Item& item)
{
	*this = item;
}

inline mapp::ItemDefinition mapp::ItemDefinition::operator =(const Item& item)
{
	name = item.resource.get_name();
	object_id = item.object_id;
	value = item.value;
	return *this;
}

#endif
