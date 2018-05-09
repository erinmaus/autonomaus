// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef TWOFLOWER_PLAN_HPP
#define TWOFLOWER_PLAN_HPP

#include <tuple>
#include <vector>
#include "twoflower/relationships/action.hpp"

namespace twoflower
{
	class Plan
	{
	public:
		Plan() = default;
		~Plan() = default;

		void add(const Action& action, int count, float cost);

		static const std::size_t ACTION = 0;
		static const std::size_t COUNT = 1;
		static const std::size_t COST = 2;
		typedef std::tuple<Action, int, float> Bullet;
		Bullet at(std::size_t index) const;

		std::size_t count() const;
		bool empty() const;

		void clear();

		operator bool() const;

	private:
		typedef std::vector<Bullet> BulletList;
		BulletList bullets;

	public:
		typedef BulletList::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
