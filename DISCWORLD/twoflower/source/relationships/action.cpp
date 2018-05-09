// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include "twoflower/relationships/action.hpp"

twoflower::Action::Builder twoflower::Action::builder()
{
	return Builder(*this);
}

int twoflower::Action::get_id() const
{
	return id;
}

const std::string& twoflower::Action::get_type() const
{
	return type;
}

const std::string& twoflower::Action::get_name() const
{
	return name;
}

bool twoflower::Action::is_getter() const
{
	return getter;
}

const std::string& twoflower::Action::get_task() const
{
	return task;
}

float twoflower::Action::get_cost_multiplier() const
{
	if (cost_multiplier == HUGE_VALF)
	{
		return 1.0f;
	}

	return cost_multiplier;
}

bool twoflower::Action::has_cost_multiplier() const
{
	return cost_multiplier != HUGE_VALF;
}

twoflower::Action::Builder::Builder(Action& action) :
	action(&action)
{
	// Nothing.
}

void twoflower::Action::Builder::set_id(int value)
{
	action->id = value;
}

void twoflower::Action::Builder::set_type(const std::string& value)
{
	action->type = value;
}

void twoflower::Action::Builder::set_name(const std::string& value)
{
	action->name = value;
}

void twoflower::Action::Builder::set_is_getter(bool value)
{
	action->getter = value;
}

void twoflower::Action::Builder::set_task(const std::string& value)
{
	action->task = value;
}

void twoflower::Action::Builder::set_cost_multiplier(float value)
{
	action->cost_multiplier = value;
}

void twoflower::Action::Builder::unset_cost_multiplier()
{
	action->cost_multiplier = HUGE_VALF;
}
