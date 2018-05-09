// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_TOOL_TIP_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_TOOL_TIP_CORTEX_HPP

#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class ToolTipCortex : public GUICortex
	{
	public:
		ToolTipCortex(Autonomaus& autonomaus);
		~ToolTipCortex() = default;

		std::vector<std::string> get_text() const;
		std::string to_string() const;

		bool match(const std::string& value);
		bool match_any(const std::vector<std::string>& values);

	private:
		chomsky::GUIQuery values_query;
	};
}

#endif
