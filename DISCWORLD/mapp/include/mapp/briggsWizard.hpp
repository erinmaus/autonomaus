// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_BRIGGS_WIZARD_HPP
#define MAPP_BRIGGS_WIZARD_HPP

#include "twoflower/wizard.hpp"
#include "twoflower/brochure.hpp"

namespace mapp
{
	class Atlas;
	class Luggage;

	class BriggsWizard : public twoflower::Wizard
	{
	public:
		BriggsWizard(
			const Atlas& atlas,
			const twoflower::Brochure& brochure,
			const Luggage& luggage);
		~BriggsWizard() = default;

		float cost(const twoflower::Action& action, float quantity) const override;
		float distance(
			const twoflower::Resource& from,
			const twoflower::Resource& to,
			const twoflower::Action& action) const override;

		void neighbors(
			const twoflower::Resource& resource,
			std::vector<std::pair<twoflower::Action, twoflower::Resource>>& neighbors) const override;

		int steps(const twoflower::Action& action, float quantity) const override;

		float reduce(const twoflower::Action& action, float quantity) const override;

		bool has(const twoflower::Resource& resource, float quantity) const override;
		float count(const twoflower::Resource& resource) const override;

	private:
		const Atlas* atlas;
		const twoflower::Brochure* brochure;
		const Luggage* luggage;
	};
}

#endif
