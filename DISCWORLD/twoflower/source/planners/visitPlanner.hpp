// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef TWOFLOWER_PLANS_VISIT_PLANNER_HPP
#define TWOFLOWER_PLANS_VISIT_PLANNER_HPP

#include "planner.hpp"

namespace twoflower
{
	class VisitPlanner : public Planner
	{
	public:
		VisitPlanner(const Brochure& brochure, const Wizard& wizard);

		bool visit(const Resource& from, const Resource& to);
	};
}

#endif
