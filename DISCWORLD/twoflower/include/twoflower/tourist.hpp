// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef TWOFLOWER_TOURIST_HPP
#define TWOFLOWER_TOURIST_HPP

#include "twoflower/brochure.hpp"
#include "twoflower/wizard.hpp"
#include "twoflower/plan.hpp"
#include "twoflower/relationships/resource.hpp"

namespace twoflower
{
	Plan visit(
		const Brochure& brochure,
		const Resource& from,
		const Resource& to,
		const Wizard& wizard);

	static const int PLAN_INFINITY = -1; // Warning: the fun may never end...
	static const int PLAN_DEFAULT_DEPTH = 16;
	static const int PLAN_UNLIMITED_DEPTH = PLAN_INFINITY;

	Plan goal(
		const Brochure& brochure,
		const Resource& resource,
		const Wizard& wizard,
		bool step = true,
		int max_depth = PLAN_DEFAULT_DEPTH);

	Plan goal(
		const Brochure& brochure,
		const Resource& resource,
		int quantity,
		const Wizard& wizard,
		bool step = true,
		int max_depth = PLAN_DEFAULT_DEPTH);
}

#endif
