// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include "twoflower/plan.hpp"

void twoflower::Plan::add(const Action& action, int count, float cost)
{
	bullets.emplace_back(action, count, cost);
}

twoflower::Plan::Bullet twoflower::Plan::at(std::size_t index) const
{
	if (index > bullets.size())
	{
		return std::make_tuple(Action(), 0, 0.0f);
	}

	return bullets.at(index);
}

std::size_t twoflower::Plan::count() const
{
	return bullets.size();
}

bool twoflower::Plan::empty() const
{
	return bullets.empty();
}

void twoflower::Plan::clear()
{
	bullets.clear();
}

twoflower::Plan::operator bool() const
{
	return !empty();
}

twoflower::Plan::const_iterator twoflower::Plan::begin() const
{
	return bullets.begin();
}

twoflower::Plan::const_iterator twoflower::Plan::end() const
{
	return bullets.end();
}
