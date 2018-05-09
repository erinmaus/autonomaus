// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_MAKE_X_WINDOW_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_MAKE_X_WINDOW_CORTEX_HPP

#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class MakeXWindowCortex : public GUICortex
	{
	public:
		MakeXWindowCortex(Autonomaus& autonomaus);
		~MakeXWindowCortex() = default;

		Cortex::Result select(
			const std::string& item,
			const std::string& category = std::string());
		Cortex::Result set_quantity(int value);

		Cortex::Result make();

		Cortex::Result close();

		std::string get_current_category() const;
		std::string get_current_item() const;
		int get_current_quantity() const;

	private:
		static const float ACTION_WAIT_TIME_MIN;
		static const float ACTION_WAIT_TIME_MAX;

		chomsky::GUIQuery items_query;
		chomsky::GUIQuery selected_item_query;
		chomsky::GUIQuery categories_query;
		chomsky::GUIQuery categories_option_query;
		chomsky::GUIQuery quantity_input_query;
		chomsky::GUIQuery quantity_input_value_query;
		chomsky::GUIQuery make_button_query;
		chomsky::GUIQuery close_button_query;
		chomsky::GUIQuery scroll_bar_query;

		struct SelectAction;
	};
}

#endif
