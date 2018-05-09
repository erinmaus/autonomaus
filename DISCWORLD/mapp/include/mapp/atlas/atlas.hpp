// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_ATLAS_ATLAS_HPP
#define MAPP_ATLAS_ATLAS_HPP

#include <string>
#include <unordered_map>
#include "mapp/atlas/amenity.hpp"
#include "mapp/atlas/link.hpp"
#include "mapp/atlas/location.hpp"
#include "twoflower/brochure.hpp"

namespace mapp
{
	class Atlas
	{
	public:
		Atlas(twoflower::Brochure& brochure);
		~Atlas() = default;

		bool find(const std::string& address, Location& result) const;
		Locations find(const std::string& address) const;
		bool fuzzy_find(const std::string& address, Location& result) const;
		Locations fuzzy_find(const std::string& address) const;
		bool find(const thoreau::TileCoordinate& tile_coordinate, Location& result) const;

		std::string address(const Location& location) const;

		Location add(const LocationDefinition& location);
		Location add(const LocationDefinition& location, const Location& parent);
		void remove(const Location& location);
		Location update(const Location& location, const LocationDefinition& definition);

		Amenity add(const AmenityDefinition& amenity, const Location& parent);
		void remove(const Amenity& amenity);
		Amenity update(const Amenity& amenity, const AmenityDefinition& definition);

		Location parent(const Location& location) const;
		bool parent(const twoflower::Resource& resource, Location& result) const;
		Locations children(const Location& location) const;

		bool location(const twoflower::Resource& resource, Location& result) const;
		bool amenity(const twoflower::Resource& resource, Amenity& result) const;

		Amenities amenities(const Location& location) const;

		Link connect(TravelType type, const Location& to);
		Link connect(
			TravelType type,
			const Location& from,
			const Location& to,
			float cost = HUGE_VALF);
		void disconnect(
			const Location& from,
			const Location& to,
			TravelType type = TravelType::none);
		void remove(const Link& link);

		Link link(const twoflower::Action& action) const;
		bool destination(const Link& link, Location& result) const;
		bool destination(const twoflower::Action& action, Location& result) const;

		Links links(const Location& location) const;

		const Location& root() const;

	private:
		void ensure_action_definition(const std::string& type, bool getter);
		void ensure_resource_type(const std::string& type);
		void ensure_root_location();

		bool do_find_child_location_by_tile(
			const thoreau::TileCoordinate& tile_coordinate,
			const Location& parent,
			Location& result) const;

		void recompute_bounds(const Location& location);

		twoflower::Brochure* brochure;
		Location root_location;

		mutable std::unordered_map<int, Location> cache_locations;
		mutable std::unordered_map<int, int> cache_parents;
		mutable std::unordered_map<int, Locations> cache_children;
	};
}

#endif
