// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include <algorithm>>
#include "planner.hpp"

twoflower::Planner::Planner(const Brochure& brochure, const Wizard& wizard) :
	brochure_instance(&brochure), wizard_instance(&wizard)
{
	// Nothing.
}

twoflower::Plan& twoflower::Planner::plan()
{
	return current_plan;
}

const twoflower::Plan& twoflower::Planner::plan() const
{
	return current_plan;
}

const twoflower::Brochure& twoflower::Planner::brochure() const
{
	return *brochure_instance;
}

const twoflower::Wizard& twoflower::Planner::wizard() const
{
	return *wizard_instance;
}

twoflower::Planner::Luggage& twoflower::Planner::luggage()
{
	return state;
}

const twoflower::Planner::Luggage& twoflower::Planner::luggage() const
{
	return state;
}

void twoflower::Planner::visit(const Resource& resource)
{
	if (visitor_stack.empty())
	{
		throw std::runtime_error("visitor stack empty");
	}

	visitor_stack.top().insert(resource.get_id());
}

bool twoflower::Planner::visited(const Resource& resource)
{
	if (visitor_stack.empty())
	{
		throw std::runtime_error("visitor stack empty");
	}

	return visitor_stack.top().count(resource.get_id()) != 0;
}

void twoflower::Planner::push()
{
	if (visitor_stack.empty())
	{
		visitor_stack.emplace();
	}
	else
	{
		visitor_stack.emplace(visitor_stack.top());
	}
}

void twoflower::Planner::pop()
{
	if (visitor_stack.empty())
	{
		throw std::runtime_error("visitor stack empty");
	}

	visitor_stack.pop();
}

std::size_t twoflower::Planner::depth() const
{
	return visitor_stack.size();
}

void twoflower::Planner::Luggage::add(const Resource& resource, float quantity)
{
	auto iter = resources.find(resource.get_id());
	if (iter == resources.end())
	{
		iter = resources.emplace(resource.get_id(), 0.0f).first;
	}

	iter->second += quantity;
}

void twoflower::Planner::Luggage::remove(const Resource& resource, float quantity)
{
	auto iter = resources.find(resource.get_id());
	if (iter == resources.end())
	{
		iter = resources.emplace(resource.get_id(), 0.0f).first;
	}

	iter->second -= quantity;
}

bool twoflower::Planner::Luggage::has(const Resource& resource, float quantity) const
{
	return count(resource) >= quantity;
}

float twoflower::Planner::Luggage::count(const Resource& resource) const
{
	auto iter = resources.find(resource.get_id());
	if (iter == resources.end())
	{
		return 0.0f;
	}

	return iter->second;
}

float twoflower::Planner::Luggage::distance(const Luggage& other) const
{
	float sum = 0.0f;

	for (auto& i: resources)
	{
		float count = std::ceil(i.second);

		float other_count = 0.0f;
		{
			auto iter = other.resources.find(i.first);
			if (iter != other.resources.end())
			{
				other_count = iter->second;
			}
		}

		float difference = std::max(other_count - count, 0.0f);
		sum += difference;
	}

	return sum;
}

bool twoflower::Planner::Luggage::match(const Luggage& other) const
{
	return distance(other) < 1.0f;
}

twoflower::Planner::Luggage::const_iterator
twoflower::Planner::Luggage::begin() const
{
	return resources.begin();
}

twoflower::Planner::Luggage::const_iterator
twoflower::Planner::Luggage::end() const
{
	return resources.end();
}
