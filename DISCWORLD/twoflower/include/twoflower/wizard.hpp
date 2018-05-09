// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef TWOFLOWER_WIZARD_HPP
#define TWOFLOWER_WIZARD_HPP

#include <utility>
#include <vector>
#include "twoflower/relationships/action.hpp"
#include "twoflower/relationships/resource.hpp"

namespace twoflower
{
	class Action;
	class Resource;

	class Wizard
	{
	public:
		virtual ~Wizard() = default;

		/// Gets the cost of a single action, repeated `quantity` times.
		///
		/// See `has` for specifics about the `quantity` parameter.
		virtual float cost(const Action& action, float quantity) const = 0;

		/// Gets the distance between two resources via `action`.
		///
		/// This is only used by traveling methods (e.g., path-finding). If the
		/// Wizard is not expected to be used for that purpose, the value
		/// HUGE_VALF should be returned.
		virtual float distance(
			const Resource& from,
			const Resource& to,
			const Action& action) const = 0;

		/// Gets the neighbors of the provided `resource`.
		///
		/// `neighbors` is a collection of pairs with `resource`'s' actions and
		/// `resource`'s outputs. A single `Action` can result in multiple
		/// neighbors, though whether that's useful or not is yet to be seen.
		///
		/// For example, an 'Iron platebody' item may have the neighbors:
		///
		/// { BuyAction, Iron Platebody }
		/// { LootAction, Iron platebody }
		///
		/// ... and so on.
		///
		/// On the other hand, the 'Olde Boote Store' location may have the
		/// neighbors:
		///
		/// { WalkAction, Olde Pubbe }
		/// { WalkAction, Ye Medieval City }
		///
		/// It's up to the Wizard to decide what qualifies as a neighbor.
		/// Generally, no filtering should be performed based on conditions
		/// derived from game state--only constant things.
		///
		/// For example, if there's a 'fight' action and the Wizard is a
		/// pacifist, then it can skip that action. However, if there is a 'buy'
		/// action and the Wizard currently doesn't have enough money, it should
		/// probably be provided as an option so the planner can attempt to
		/// solve that problem.
		virtual void neighbors(
			const Resource& resource,
			std::vector<std::pair<Action, Resource>>& neighbors) const = 0;

		/// Returns the number of steps an action will generate.
		///
		/// A step is considered a change in state that may allow other actions
		/// to succeed. For example, completing a quest or gaining a level in
		/// a skill.
		///
		/// In the end the Wizard decides what qualifies as a step, but more
		/// frequent steps will cause performance issues during planning.
		virtual int steps(const Action& action, float quantity) const = 0;

		/// Returns a new quantity that produces a single step for the provided
		/// action.
		///
		/// Return condition: steps(action, reduce(action, quantity)) == 1
		virtual float reduce(const Action& action, float quantity) const = 0;

		/// Returns true if the Wizard has the provided resource with the
		/// provided quantity, false otherwise.
		///
		/// Fractional values should probably be rounded up, but it's up to the
		/// wizard. Fractional values are useful for when a single resource has
		/// multiple uses. For example, you can only have integral
		/// 'Rings of Smiting', but if a 'Ring of Smiting' lasts 100 'Smite'
		/// actions, then has may be called by the planner with <1 quantity
		/// (e.g., 1/100) 'Ring of Smiting'.
		virtual bool has(const Resource& resource, float quantity) const = 0;

		/// Returns how many of the provided resource the Wizard has.
		///
		/// For permanent things, like say key items in some RPGs, a value of
		/// HUGE_VALF is acceptable.
		virtual float count(const Resource& resource) const = 0;
	};
}

#endif
