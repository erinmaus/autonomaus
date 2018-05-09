// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_TARGET_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_TARGET_CORTEX_HPP

#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class TargetCortex : public GUICortex
	{
	public:
		TargetCortex(Autonomaus& autonomaus);
		~TargetCortex() = default;

		int get_current_health() const;

		std::vector<std::string> get_buffs() const;
		std::vector<std::string> get_debuffs() const;

		bool has_buff(const std::string& value) const;
		bool has_debuff(const std::string& value) const;

	private:
		chomsky::GUIQuery current_health_query;
		chomsky::GUIQuery buffs_query;
		chomsky::GUIQuery debuffs_query;
	};
}

#endif
