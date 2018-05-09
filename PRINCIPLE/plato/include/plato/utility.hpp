// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PLATO_UTILITY_HPP
#define PLATO_UTILITY_HPP

#include "mapp/luggage.hpp"
#include "mapp/atlas/atlas.hpp"
#include "mapp/atlas/atlas.hpp"
#include "twoflower/brochure.hpp"
#include "thoreau/map.hpp"

namespace plato
{
	class EmptyLuggage : public mapp::Luggage
	{
	public:
		EmptyLuggage() = default;

		bool has(const twoflower::Resource& resource, float quantity) const override;
		float count(const twoflower::Resource& resource) const override;
	};

	void connect_nearest_neighbors(
		twoflower::Brochure& brochure,
		mapp::Atlas& altas,
		const thoreau::Map& map);

	void connect_walkable_children(
		twoflower::Brochure& brochure,
		mapp::Atlas& atlas,
		const mapp::Location& parent,
		const thoreau::Map& map,
		bool recursive = false);
}

#endif
