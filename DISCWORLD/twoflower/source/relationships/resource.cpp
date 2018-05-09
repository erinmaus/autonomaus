// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include "twoflower/relationships/resource.hpp"

twoflower::Resource::Builder twoflower::Resource::builder()
{
	return Builder(*this);
}

int twoflower::Resource::get_id() const
{
	return id;
}

const std::string& twoflower::Resource::get_name() const
{
	return name;
}

const std::string& twoflower::Resource::get_type() const
{
	return type;
}

twoflower::Resource::Builder::Builder(Resource& resource) :
	resource(&resource)
{
	// Nothing.
}

void twoflower::Resource::Builder::set_id(int value)
{
	resource->id = value;
}

void twoflower::Resource::Builder::set_type(const std::string& value)
{
	resource->type = value;
}

void twoflower::Resource::Builder::set_name(const std::string& value)
{
	resource->name = value;
}
