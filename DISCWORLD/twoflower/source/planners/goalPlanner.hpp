// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef TWOFLOWER_PLANS_GOAL_PLANNER_HPP
#define TWOFLOWER_PLANS_GOAL_PLANNER_HPP

#include "planner.hpp"

namespace twoflower
{
	class GoalPlanner : public Planner
	{
	public:
		GoalPlanner(
			const Brochure& brochure,
			const Wizard& wizard,
			bool is_stepping,
			int max_depth);

		bool goal(const Resource& resource, int quantity);

	private:
		enum class Step
		{
			success,
			failure,
			working
		};
		Step step(const Action& action, int quantity);
		Step satisfy(const Requirement& requirement, int quantity);

		Luggage goal_state;

		bool is_stepping;
		int max_depth;

		int current_depth = 0;
	};
}

#endif
