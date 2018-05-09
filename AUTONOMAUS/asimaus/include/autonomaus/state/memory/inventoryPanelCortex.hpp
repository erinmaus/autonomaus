// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_INVENTORY_PANEL_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_INVENTORY_PANEL_CORTEX_HPP

#include <string>
#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class InventoryPanelCortex : public GUICortex
	{
	public:
		InventoryPanelCortex(Autonomaus& autonomaus);
		~InventoryPanelCortex() = default;

		Cortex::Result open();

		int get_free_space() const;
		int get_used_space() const;

		enum
		{
			match_noted = 1 << 0,
			match_unnoted = 1 << 1,
			match_variants = 1 << 2
		};

		bool has_item(
			const std::string& name,
			int quantity = 1,
			int flags = match_unnoted | match_variants) const;
		int count_item(
			const std::string& name,
			int flags = match_unnoted | match_variants) const;

		std::vector<int> get_item_slots(
			const std::string& name,
			int flags = match_unnoted | match_variants) const;

		Cortex::Result poke_item(
			const std::string& name,
			const std::string& action,
			int flags = match_unnoted | match_variants);
		Cortex::Result poke_slot(
			int slot,
			const std::string& action);

		Cortex::Result examine_inventory(bool everything = false);

		int get_coins() const;

		void mark_dirty();
		bool is_dirty() const;

		bool is_ready() const override;
		void update() override;

	private:
		chomsky::GUIQuery slots_query;
		chomsky::GUIQuery items_query;
		void update_items();

		chomsky::GUIQuery coins_query;
		void update_coins();

		static const int NUM_INVENTORY_SLOTS;
		bool obscured = false;
		bool dirty = false;

		struct ExamineAction;
	};
}

#endif
