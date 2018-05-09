// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_RIGHT_CLICK_MENU_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_RIGHT_CLICK_MENU_CORTEX_HPP

#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace autonomaus
{
	class RightClickMenuCortex : public GUICortex
	{
	public:
		RightClickMenuCortex(Autonomaus& autonomaus);
		~RightClickMenuCortex() = default;

		bool match(const std::string& action, const std::string& target = std::string()) const;
		bool match_any(const std::vector<std::string>& actions, const std::string& target = std::string()) const;

		int get_num_options() const;
		std::vector<std::string> get_option_text(int option) const;
		std::string get_option_string(int option) const;

		Cortex::Result select(int option);
		Cortex::Result select(const std::string& action, const std::string& target = std::string());
		Cortex::Result select_any(const std::vector<std::string>& actions, const std::string& target = std::string());

	private:
		chomsky::GUIQuery options_query;
		chomsky::GUIQuery cancel_query;
	};
}

#endif
