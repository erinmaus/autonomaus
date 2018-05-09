// TWOFLOWER
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef TWOFLOWER_RELATIONSHIPS_ACTION_HPP
#define TWOFLOWER_RELATIONSHIPS_ACTION_HPP

#include <string>
#include <cmath>

namespace twoflower
{
	class Action
	{
	public:
		Action() = default;
		~Action() = default;

		class Builder;
		Builder builder();

		int get_id() const;
		const std::string& get_type() const;
		const std::string& get_name() const;
		bool is_getter() const;
		const std::string& get_task() const;
		float get_cost_multiplier() const;
		bool has_cost_multiplier() const;

	private:
		int id = 0;
		std::string type;
		std::string name;
		bool getter = false;
		std::string task;
		float cost_multiplier = HUGE_VALF;
	};

	class Action::Builder
	{
	public:
		Builder(Action& action);

		void set_id(int id);
		void set_type(const std::string& value);
		void set_name(const std::string& value);
		void set_is_getter(bool value);
		void set_task(const std::string& value);
		void set_cost_multiplier(float value);
		void unset_cost_multiplier();

	private:
		Action* action;
	};
}

#endif
