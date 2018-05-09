// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include "mapp/briggsWizard.hpp"
#include "mapp/luggage.hpp"
#include "mapp/atlas/atlas.hpp"
#include "mapp/atlas/location.hpp"
#include "mapp/atlas/link.hpp"

mapp::BriggsWizard::BriggsWizard(
	const Atlas& atlas, const twoflower::Brochure& brochure, const Luggage& luggage) :
	atlas(&atlas),
	brochure(&brochure),
	luggage(&luggage)
{
	// Nothing.
}

float mapp::BriggsWizard::cost(const twoflower::Action& action, float quantity) const
{
	return action.get_cost_multiplier() * quantity;
}

float mapp::BriggsWizard::distance(
	const twoflower::Resource& from,
	const twoflower::Resource& to,
	const twoflower::Action& action) const
{
	return action.get_cost_multiplier();
}

void mapp::BriggsWizard::neighbors(
	const twoflower::Resource& resource,
	std::vector<std::pair<twoflower::Action, twoflower::Resource>>& neighbors) const
{
	Location location;
	if (atlas->location(resource, location))
	{
		auto links = atlas->links(location);
		for (auto& link: links)
		{
			Location destination;
			if (atlas->destination(link, destination))
			{
				neighbors.emplace_back(link.action, destination.resource);
			}
		}
	}
}

int mapp::BriggsWizard::steps(const twoflower::Action& action, float quantity) const
{
	return 1;
}

float mapp::BriggsWizard::reduce(const twoflower::Action& action, float quantity) const
{
	return 1.0f;
}

bool mapp::BriggsWizard::has(const twoflower::Resource& resource, float quantity) const
{
	if (resource.get_type() == "location")
	{
		return true;
	}

	return luggage->has(resource, quantity);
}

float mapp::BriggsWizard::count(const twoflower::Resource& resource) const
{
	if (resource.get_type() == "location")
	{
		return HUGE_VALF;
	}

	return luggage->count(resource);
}
