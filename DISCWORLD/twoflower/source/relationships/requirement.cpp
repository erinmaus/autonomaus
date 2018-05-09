// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include "twoflower/relationships/requirement.hpp"

twoflower::Requirement::Builder twoflower::Requirement::builder()
{
	return Builder(*this);
}

int twoflower::Requirement::get_id() const
{
	return id;
}

const twoflower::Resource& twoflower::Requirement::get_resource() const
{
	return resource;
}

const twoflower::Action& twoflower::Requirement::get_action() const
{
	return action;
}

float twoflower::Requirement::get_count() const
{
	return count;
}

bool twoflower::Requirement::is_input() const
{
	return input;
}

bool twoflower::Requirement::is_output() const
{
	return output;
}

twoflower::Requirement::Builder::Builder(Requirement& requirement) :
	requirement(&requirement)
{
	// Nothing.
}

void twoflower::Requirement::Builder::set_id(int value)
{
	requirement->id = value;
}

void twoflower::Requirement::Builder::set_resource(const Resource& value)
{
	requirement->resource = value;
}

void twoflower::Requirement::Builder::set_action(const Action& value)
{
	requirement->action = value;
}

void twoflower::Requirement::Builder::set_count(float value)
{
	if (value < 0.0f)
	{
		requirement->count = 0.0f;
	}
	else
	{
		requirement->count = value;
	}
}

void twoflower::Requirement::Builder::set_is_input(bool value)
{
	requirement->input = value;
}

void twoflower::Requirement::Builder::set_is_output(bool value)
{
	requirement->output = value;
}

