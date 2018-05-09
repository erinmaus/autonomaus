// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_GAME_CURRENCY_HPP
#define MAPP_GAME_CURRENCY_HPP

#include <string>
#include "twoflower/relationships/resource.hpp"

namespace mapp
{
	struct Currency
	{
		twoflower::Resource resource;
	};

	struct CurrencyDefinition
	{
		std::string name;

		CurrencyDefinition() = default;
		CurrencyDefinition(const Currency& value);
		CurrencyDefinition operator =(const Currency& value);
	};

	typedef std::vector<Currency> Currencies;
	typedef std::pair<int, Currency> Price;
}

inline mapp::CurrencyDefinition::CurrencyDefinition(const Currency& value)
{
	*this = value;
}

inline mapp::CurrencyDefinition mapp::CurrencyDefinition::operator =(const Currency& value)
{
	name = value.resource.get_name();
	return *this;
}

#endif
