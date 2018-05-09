// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_ACTION_BAR_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_ACTION_BAR_CORTEX_HPP

#include <string>
#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class ActionBarCortex : public GUICortex
	{
	public:
		ActionBarCortex(Autonomaus& autonomaus);
		~ActionBarCortex() = default;

		int get_current_life_points() const;
		int get_maximum_life_points() const;
		int get_current_prayer_points() const;
		int get_maximum_prayer_points() const;
		int get_adrenaline() const;
		int get_current_summoning_points() const;
		int get_maximum_summoning_points() const;

		enum
		{
			action_bar_none = -1,
			action_bar_primary = 0,
			action_bar_auxiliary_1,
			action_bar_auxiliary_2,
			action_bar_auxiliary_3,
			action_bar_auxiliary_4
		};
		Cortex::Result switch_action_bar(int action_bar, int index);
		int get_action_bar_index(int action_bar);

		bool is_locked() const;
		bool is_unlocked() const;
		Cortex::Result lock();
		Cortex::Result unlock();

		bool is_minimized() const;
		bool is_maximized() const;
		Cortex::Result maximize();
		Cortex::Result minimize();

		enum
		{
			type_none = 0,
			type_ability,
			type_item,
			type_spell,
			type_prayer,
			type_unknown
		};
		int get_slot_type(int action_bar, int slot) const;
		bool get_slot_enabled(int action_bar, int slot) const;
		std::string get_slot_value(int action_bar, int slot) const;
		std::vector<std::string> get_slot_values(int action_bar, int slot) const;

		Cortex::Result activate(int action_bar, int slot);
		Cortex::Result activate(const std::vector<std::pair<int, int>>& slots);
		Cortex::Result activate(int type, const std::string& value);
		Cortex::Result activate(const std::vector<std::pair<int, std::string>>& values);

		std::pair<int, int> get_slot(int type, const std::string& value) const;
		std::vector<std::pair<int, int>> get_slots(int type, const std::string& value) const;

		int get_cooldown(const std::string& ability);
		bool is_ready(const std::string& ability, int cooldown = 0);

		bool is_revolution_enabled() const;

		void update() override;

	private:
		Memory get_action_bar(int action_bar) const;
		void update_action_bar(
			int action_bar,
			const chomsky::GUIContent& action_bar_content);

		void update_stats();
		bool try_get_stat(const std::string& stat, std::string& result);

		bool try_parse_keybind(const std::string& keybind, std::string& result);

		chomsky::GUIQuery stat_query;
		chomsky::GUIQuery slots_query;
		chomsky::GUIQuery index_query;
		chomsky::GUIQuery minimize_button_query;
		chomsky::GUIQuery lock_button_query;
		chomsky::GUIQuery auxiliary_action_bars_query;
		chomsky::GUIQuery revolution_query;
		chomsky::GUIQuery keybind_query;
	};
}

#endif
