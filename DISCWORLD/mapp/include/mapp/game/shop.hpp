// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_GAME_SHOP_HPP
#define MAPP_GAME_SHOP_HPP

#include <string>
#include "mapp/game/currency.hpp"
#include "twoflower/relationships/resource.hpp"

namespace mapp
{
	struct Shop
	{
		twoflower::Resource resource;
		Location location;
	};

	struct ShopDefinition
	{
		std::string name;
		Location location;

		ShopDefinition() = default;
		ShopDefinition(const Shop& value);
		ShopDefinition operator =(const Shop& value);
	};

	typedef std::vector<Shop> Shops;
	typedef std::vector<Price> ShopPrice;
}

inline mapp::ShopDefinition::ShopDefinition(const Shop& value)
{
	*this = value;
}

inline mapp::ShopDefinition mapp::ShopDefinition::operator =(const Shop& value)
{
	name = value.resource.get_name();
	location = value.location;
	return *this;
}

#endif
