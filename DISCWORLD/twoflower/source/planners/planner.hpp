//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef TWOFLOWER_PLANS_PLANNER_HPP
#define TWOFLOWER_PLANS_PLANNER_HPP

#include <stack>
#include <boost/container/flat_map.hpp>
#include "twoflower/brochure.hpp"
#include "twoflower/wizard.hpp"
#include "twoflower/plan.hpp"
#include "twoflower/relationships/action.hpp"
#include "twoflower/relationships/resource.hpp"

namespace twoflower
{
	class Planner
	{
	public:
		Planner(const Brochure& brochure, const Wizard& wizard);
		virtual ~Planner() = default;

	public:
		Plan& plan();
		const Plan& plan() const;

	protected:
		const Brochure& brochure() const;
		const Wizard& wizard() const;

		struct Luggage
		{
		public:
			Luggage() = default;
			~Luggage() = default;

			void add(const Resource& resource, float quantity);
			void remove(const Resource& resource, float quantity);

			bool has(const Resource& resource, float quantity) const;
			float count(const Resource& resource) const;

			float distance(const Luggage& other) const;
			bool match(const Luggage& other) const;

		private:
			typedef boost::container::flat_map<int, float> ResourceMap;
			ResourceMap resources;

		public:
			typedef ResourceMap::const_iterator const_iterator;

			const_iterator begin() const;
			const_iterator end() const;
		};

		Luggage& luggage();
		const Luggage& luggage() const;

		void visit(const Resource& resource);
		bool visited(const Resource& resource);

		void push();
		void pop();
		std::size_t depth() const;

	private:
		const Brochure* brochure_instance;
		const Wizard* wizard_instance;

		typedef std::unordered_set<int> VisitedResources;
		std::stack<VisitedResources> visitor_stack;

		Luggage state;

		Plan current_plan;
	};
}

#endif
