// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_BUFF_BAR_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_BUFF_BAR_CORTEX_HPP

#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class BuffBarCortex : public GUICortex
	{
	public:
		BuffBarCortex(Autonomaus& autonomaus);
		~BuffBarCortex() = default;

		// TODO: Support time
		std::vector<std::pair<std::string, float>> get_buffs() const;
		std::vector<std::pair<std::string, float>> get_debuffs() const;

		bool has_buff(const std::string& value) const;
		bool has_debuff(const std::string& value) const;

	private:
		chomsky::GUIQuery buffs_query;
		chomsky::GUIQuery debuffs_query;
	};
}

#endif
