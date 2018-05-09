// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include "goalPlanner.hpp"
#include "twoflower/tourist.hpp"

twoflower::GoalPlanner::GoalPlanner(
	const Brochure& brochure,
	const Wizard& wizard,
	bool is_stepping,
	int max_depth) :
	Planner(brochure, wizard),
	is_stepping(is_stepping),
	max_depth(max_depth)
{
	// Nothing.
}

bool twoflower::GoalPlanner::goal(const Resource& resource, int quantity)
{
	std::vector<std::pair<Action, Resource>> neighbors;
	wizard().neighbors(resource, neighbors);

	std::sort(
		neighbors.begin(),
		neighbors.end(),
		[this, quantity](auto& a, auto& b)
		{
			return this->wizard().cost(a.first, quantity) < this->wizard().cost(b.first, quantity);
		});

	auto current = neighbors.begin();
	while (current != neighbors.end())
	{
		auto result = step(current->first, quantity);
		if (result == Step::failure)
		{
			plan().clear();
			++current;
		}
		else if (result == Step::success)
		{
			return true;
		}
	}

	return plan().empty();
}

twoflower::GoalPlanner::Step twoflower::GoalPlanner::step(const Action& action, int quantity)
{
	if (max_depth != PLAN_UNLIMITED_DEPTH && (int)depth() > max_depth)
	{
		return Step::failure;
	}

	if (quantity < 1)
	{
		return Step::failure;
	}

	push();

	float iteration_quantity;
	if (is_stepping)
	{
		iteration_quantity = wizard().reduce(action, quantity);
	}
	else
	{
		iteration_quantity = quantity;
	}

	auto requirements = brochure().requirements(action);
	for (auto requirement: requirements)
	{
		if (requirement.is_output())
		{
			continue;
		}

		int q;
		if (requirement.is_input())
		{
			q = std::ceil(requirement.get_count() * iteration_quantity);
		}
		else
		{
			q = requirement.get_count();
		}

		if (!wizard().has(requirement.get_resource(), q) &&
			!luggage().has(requirement.get_resource(), q))
		{
			auto result = satisfy(requirement, iteration_quantity);
			if (result != Step::success)
			{
				pop();
				return result;
			}
		}
	}

	for (auto requirement: requirements)
	{
		if (requirement.is_output())
		{
			luggage().add(requirement.get_resource(), requirement.get_count() * iteration_quantity);
		}

		if (requirement.is_input())
		{
			luggage().remove(requirement.get_resource(), requirement.get_count() * iteration_quantity);
		}
	}

	pop();

	plan().add(
		action,
		std::ceil(iteration_quantity),
		wizard().cost(action, iteration_quantity));

	if (std::ceil(iteration_quantity) < quantity)
	{
		return Step::working;
	}
	else
	{
		return Step::success;
	}
}

twoflower::GoalPlanner::Step twoflower::GoalPlanner::satisfy(const Requirement& requirement, int quantity)
{
	auto& resource = requirement.get_resource();
	if (visited(resource))
	{
		return Step::failure;
	}
	visit(resource);

	std::vector<std::pair<Action, Resource>> neighbors;
	wizard().neighbors(resource, neighbors);

	std::sort(
		neighbors.begin(),
		neighbors.end(),
		[this, quantity](auto& a, auto& b)
		{
			return this->wizard().cost(a.first, quantity) < this->wizard().cost(b.first, quantity);
		});

	for (auto& neighbor: neighbors)
	{
		Requirement output;
		{
			auto requirements = brochure().requirements(neighbor.first);
			bool found_output = false;
			for (auto requirement: requirements)
			{
				if (requirement.is_output() && requirement.get_resource().get_id() == resource.get_id())
				{
					output = requirement;
					found_output = true;
					break;
				}
			}

			if (!found_output)
			{
				continue;
			}
		}

		auto r = requirement.get_count() - (wizard().count(resource) + luggage().count(resource));
		int q = std::ceil(r / output.get_count());

		auto result = step(neighbor.first, q * quantity);
		if (result != Step::failure)
		{
			return result;
		}
	}

	return Step::failure;
}
