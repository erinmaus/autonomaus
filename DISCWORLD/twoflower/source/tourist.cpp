// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include "planners/visitPlanner.hpp"
#include "planners/goalPlanner.hpp"
#include "twoflower/tourist.hpp"

twoflower::Plan twoflower::visit(
	const Brochure& brochure,
	const Resource& from,
	const Resource& to,
	const Wizard& wizard)
{
	VisitPlanner planner(brochure, wizard);
	if (!planner.visit(from, to))
	{
		return Plan();
	}

	return planner.plan();
}

twoflower::Plan twoflower::goal(
	const Brochure& brochure,
	const Resource& resource,
	const Wizard& wizard,
	bool step,
	int max_depth)
{
	return goal(brochure, resource, 1, wizard, step, max_depth);
}

twoflower::Plan twoflower::goal(
	const Brochure& brochure,
	const Resource& resource,
	int quantity,
	const Wizard& wizard,
	bool step,
	int max_depth)
{
	GoalPlanner planner(brochure, wizard, step, max_depth);
	if (!planner.goal(resource, quantity))
	{
		return Plan();
	}

	return planner.plan();
}
