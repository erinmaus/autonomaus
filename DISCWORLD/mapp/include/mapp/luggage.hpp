// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_LUGGAGE_HPP
#define MAPP_LUGGAGE_HPP

#include "twoflower/relationships/resource.hpp"

namespace mapp
{
	class Luggage
	{
	public:
		virtual ~Luggage() = default;

		virtual bool has(const twoflower::Resource& resource, float quantity) const = 0;
		virtual float count(const twoflower::Resource& resource) const = 0;
	};
}

#endif
