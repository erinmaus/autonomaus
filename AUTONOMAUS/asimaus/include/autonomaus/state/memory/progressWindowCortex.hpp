// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_PROGRESS_WINDOW_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_PROGRESS_WINDOW_CORTEX_HPP

#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class ProgressWindowCortex : public GUICortex
	{
	public:
		ProgressWindowCortex(Autonomaus& autonomaus);
		~ProgressWindowCortex() = default;

		Cortex::Result cancel();
		Cortex::Result close();

	private:
		static const float CLOSE_TIME;

		chomsky::GUIQuery cancel_button_query;
		chomsky::GUIQuery close_button_query;
	};
}

#endif
