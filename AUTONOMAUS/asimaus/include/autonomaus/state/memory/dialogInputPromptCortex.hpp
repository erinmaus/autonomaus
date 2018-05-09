// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_DIALOG_INPUT_PROMPT_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_DIALOG_INPUT_PROMPT_CORTEX_HPP

#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class DialogInputPromptCortex : public GUICortex
	{
	public:
		DialogInputPromptCortex(Autonomaus& autonomaus);
		~DialogInputPromptCortex() = default;

		std::string get_prompt() const;
		std::string get_input() const;

		Cortex::Result type(const std::string& text, bool ignore_case = true);
		Cortex::Result clear();
		Cortex::Result submit();

	private:
		chomsky::GUIQuery prompt_query;
		chomsky::GUIQuery input_query;
	};
}

#endif
