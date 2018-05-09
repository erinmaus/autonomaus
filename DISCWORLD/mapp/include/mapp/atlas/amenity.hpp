// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_ATLAS_AMENITY_HPP
#define MAPP_ATLAS_AMENITY_HPP

#include <string>
#include "twoflower/relationships/resource.hpp"
#include "thoreau/shape.hpp"

namespace mapp
{
	struct Amenity
	{
		twoflower::Resource resource;
	};

	struct AmenityDefinition
	{
		std::string name;

		AmenityDefinition() = default;
		AmenityDefinition(const Amenity& amenity);
		AmenityDefinition operator =(const Amenity& amenity);
	};

	typedef std::vector<Amenity> Amenities;
}

inline mapp::AmenityDefinition::AmenityDefinition(const Amenity& amenity)
{
	*this = amenity;
}

inline mapp::AmenityDefinition mapp::AmenityDefinition::operator =(const Amenity& amenity)
{
	name = amenity.resource.get_name();
	return *this;
}

#endif
