// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_RIBBON_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_RIBBON_CORTEX_HPP

#include <string>
#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class RibbonCortex : public GUICortex
	{
	public:
		RibbonCortex(Autonomaus& autonomaus);
		~RibbonCortex() = default;

		Cortex::Result lock();
		Cortex::Result unlock();
		bool is_locked() const;

		Cortex::Result open(const std::string& category);
		Cortex::Result open(const std::string& category, const std::string& tab);

		bool is_open(const std::string& name) const;
		bool is_active(const std::string& name) const;

	private:
		chomsky::GUIQuery ribbon_buttons_query;
		chomsky::GUIQuery ribbon_lock_button_query;
		chomsky::GUIQuery ribbon_pop_up_query;
		chomsky::GUIQuery ribbon_pop_up_buttons_query;
		chomsky::GUIQuery panel_tabs_query;
	};
}

#endif
