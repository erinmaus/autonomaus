// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_BANK_WINDOW_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_BANK_WINDOW_CORTEX_HPP

#include <map>
#include "autonomaus/core/utility.hpp"
#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class BankWindowCortex : public GUICortex
	{
	public:
		BankWindowCortex(Autonomaus& autonomaus);
		~BankWindowCortex() = default;

		int get_free_space() const;
		int get_used_space() const;

		bool has_item(const std::string& name, int quantity = 1) const;
		int count_item(const std::string& name) const;

		enum
		{
			transfer_all = -1,
			transfer_all_but_one = -2
		};

		Result withdraw(
			const std::string& name,
			int quantity = 1,
			bool noted = false,
			int match_flags = fuzzy_match_default);
		Result deposit(
			const std::string& name,
			int quantity = 1,
			int match_flags = fuzzy_match_default);

		Result equip(
			const std::string& name,
			int match_flags = fuzzy_match_default);

		Result poke_bank(
			const std::string& name,
			const std::string& action,
			int match_flags = fuzzy_match_default);
		Result poke_inventory(
			const std::string& name,
			const std::string& action,
			int match_flags = fuzzy_match_default);

		int get_num_tabs() const;
		int get_active_tab() const;
		int get_num_items_in_tab(int tab) const;

		enum
		{
			examine_unknown = 0,
			examine_ambiguous,
			examine_everything
		};
		Result examine_bank(int mode = examine_unknown);

		Result deposit_inventory(int tab = -1);
		Result deposit_familiar(int tab = -1);
		Result deposit_equipment(int tab = -1);
		Result deposit_coins(int tab = -1);

		Result wait(float max_time);
		Result close();

		void mark_dirty();
		void invalidate_tab(int tab_index, int item_index = 0);
		bool is_dirty() const;

		bool is_ready() const override;
		void update() override;

	private:
		chomsky::GUIQuery tabs_query;
		chomsky::GUIQuery scroll_bar_query;
		chomsky::GUIQuery items_query;
		chomsky::GUIQuery title_query;
		chomsky::GUIQuery background_query;
		chomsky::GUIQuery tab_content_separator_query;
		chomsky::GUIQuery equipment_button_query;
		chomsky::GUIQuery equipment_panel_query;
		chomsky::GUIQuery equipment_panel_items_query;
		chomsky::GUIQuery inventory_items_query;
		chomsky::GUIQuery close_button_query;
		chomsky::GUIQuery search_button_query;
		chomsky::GUIQuery note_items_button_query;
		chomsky::GUIQuery deposit_x_query;
		chomsky::GUIQuery deposit_equipment_query;
		chomsky::GUIQuery deposit_familiar_query;
		chomsky::GUIQuery deposit_coins_query;
		chomsky::GUIQuery x_usage_query;

		Result deposit_all_x(const std::string& button, int tab);

		bool is_searching() const;
		bool is_equipment_panel_open() const;
		bool is_noting_enabled() const;

		static const int SCROLL_REGION_WIDTH;
		static const int SCROLL_REGION_HEIGHT;
		static const int SCROLL_REGION_OFFSET_X;
		static const int SCROLL_REGION_OFFSET_Y;
		static const int ITEM_HEIGHT;
		static const int ITEM_WIDTH;
		float current_scroll = 0.0f;
		float current_scroll_offset = 0.0f;
		glm::vec2 item_content_offset = glm::vec2(0.0f);
		int max_num_rows = 0, top_row = 0;
		void update_scroll_view();

		void update_tabs();
		int update_items();
		void update_usage(const std::string& type);

		bool try_get_item_index(
			const chomsky::GUIContent& item_content,
			int& index);

		bool dirty = false;

		float tab_2_separator = HUGE_VALF;

		static const float LOAD_TIME;
		bool was_visible = false;
		bool loading = false;
		int item_count = 0;
		float last_load_time = 0.0f;

		std::map<std::string, std::string> item_key_to_runtime_key;

		struct IterateAction;
		struct ExamineAction;
		struct ItemAction;
		struct WithdrawAction;
		struct PokeBankAction;

		struct InventoryAction;
		struct DepositAction;
		struct PokeInventoryAction;

		typedef std::tuple<int /* tab */, int /* index */> ItemLocation;
		bool try_get_item_locations(
			const std::string& item,
			std::vector<ItemLocation>& result,
			int match_flags = fuzzy_match_default);
	};
}

#endif
