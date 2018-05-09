// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_ATTACKER_HPP
#define AUTONOMAUS_STATE_ATTACKER_HPP

#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/state/message.hpp"

namespace autonomaus
{
	class Attacker
	{
	public:
		Attacker(Autonomaus& autonomaus);

		enum
		{
			filter_basics = 1,
			filter_thresholds,
			filter_ultimates,
			filter_visible,
			filter_invisible,
			filter_user
		};
		bool has_filter(int value) const;
		void add_filter(int value);
		void remove_filter(int value);

		enum
		{
			action_none = 0,
			action_wait,
			action_ability,
			action_auto_attack,
			action_spell,
			action_prayer,
			action_poke_item,
			action_equip_item,
			action_use_item,
			action_poke,
			action_user
		};
		Message next();

		void notify(const Message& message);

		bool pending() const;

		void update();

	private:
		Autonomaus* autonomaus;
	};
}

#endif
