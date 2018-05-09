// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/config.hpp"
#include "autonomaus/core/time.hpp"
#include "autonomaus/core/rng.hpp"
#include "autonomaus/core/utility.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/memory/bankWindowCortex.hpp"
#include "autonomaus/state/memory/dialogInputPromptCortex.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"
#include "autonomaus/state/memory/rightClickMenuCortex.hpp"
#include "autonomaus/state/memory/toolTipCortex.hpp"

const int autonomaus::BankWindowCortex::SCROLL_REGION_WIDTH =
	Config::get<int>("gui", "BankWindowCortex", "SCROLL_REGION_WIDTH");
const int autonomaus::BankWindowCortex::SCROLL_REGION_HEIGHT =
	Config::get<int>("gui", "BankWindowCortex", "SCROLL_REGION_HEIGHT");
const int autonomaus::BankWindowCortex::SCROLL_REGION_OFFSET_X =
	Config::get<int>("gui", "BankWindowCortex", "SCROLL_REGION_OFFSET_X");
const int autonomaus::BankWindowCortex::SCROLL_REGION_OFFSET_Y =
	Config::get<int>("gui", "BankWindowCortex", "SCROLL_REGION_OFFSET_Y");

const int autonomaus::BankWindowCortex::ITEM_WIDTH =
	Config::get<int>("gui", "BankWindowCortex", "ITEM_WIDTH");
const int autonomaus::BankWindowCortex::ITEM_HEIGHT =
	Config::get<int>("gui", "BankWindowCortex", "ITEM_HEIGHT");

const float autonomaus::BankWindowCortex::LOAD_TIME =
	Config::get<float>("gui", "BankWindowCortex", "LOAD_TIME");

struct autonomaus::BankWindowCortex::IterateAction : public Action
{
	IterateAction(BankWindowCortex* cortex);
	~IterateAction();

	Result get_result() override;
	void update() override;

	virtual void cancel();

	virtual bool done() = 0;
	virtual ItemLocation next() = 0;
	virtual Result::Status on_start() { return Result::Status::success; }
	virtual Result::Status on_tab() { return Result::Status::success; }
	virtual Result::Status on_item() { return Result::Status::success; }

	BankWindowCortex* cortex;

private:
	enum
	{
		state_start,
		state_close_equipment,
		state_on_start,
		state_next,
		state_tab,
		state_scroll,
		state_on_tab,
		state_after_tab,
		state_find,
		state_hover,
		state_on_hover,
		state_error,
		state_done
	};
	int state = state_start;
	int previous_state;
	float exception_time = 0.0f;

	Result current_result;
	ItemLocation current_iter;
	int current_tab = -1;

	Result::StatusReference status;
	Result final_result;
};

struct autonomaus::BankWindowCortex::ExamineAction : public IterateAction
{
	ExamineAction(BankWindowCortex* cortex, int mode);
	~ExamineAction();

	void cancel() override;

	bool done() override;
	ItemLocation next() override;
	Result::Status on_tab() override;
	Result::Status on_item() override;

	int mode;

	enum
	{
		state_start,
		state_tab,
		state_tab_again,
		state_item_start,
		state_item_wait,
		state_item_examine,
		state_item_done
	};
	int state = state_start;
	Cortex::Result current_motion;
	float wait_time = 0.0f;

	int num_tabs = 0;
	int current_tab = 0;
	int current_index = 0;
	std::set<int> pending_items;
	std::set<int> visited_items;
	std::string current_runtime_key;
	std::string current_item_key;
	bool update_pending_items();
};

struct autonomaus::BankWindowCortex::ItemAction : public IterateAction
{
	ItemAction(
		BankWindowCortex* cortex,
		const std::string& item,
		int match_flags,
		std::vector<ItemLocation>& candidates);

	ItemLocation next() override;
	Result::Status on_tab() override;
	Result::Status on_item() override;

	virtual Result::Status on_match(int tab_index, int item_index) = 0;
	virtual Result::Status on_empty() = 0;

	std::string item;
	int match_flags;
	std::vector<ItemLocation> candidates;

private:
	enum
	{
		state_start,
		state_wait,
		state_skip,
		state_empty,
		state_done
	};
	int state = state_start;
	float wait_time = 0.0f;

	ItemLocation current_location;
};

struct autonomaus::BankWindowCortex::WithdrawAction : public ItemAction
{
	WithdrawAction(
		BankWindowCortex* cortex,
		const std::string& item,
		int match_flags,
		int quantity,
		bool noted,
		std::vector<ItemLocation>& candidates);
	~WithdrawAction();

	void cancel() override;

	bool done() override;
	Result::Status on_start() override;
	Result::Status on_match(int tab_index, int item_index) override;
	Result::Status on_empty() override;

	int quantity;
	bool noted;

private:
	enum
	{
		state_start,
		state_wait,
		state_right_click,
		state_select,
		state_failure,
		state_success,
		state_done
	};
	int state = state_start;

	Cortex::Result current_result;
};

struct autonomaus::BankWindowCortex::PokeBankAction : public ItemAction
{
	PokeBankAction(
		BankWindowCortex* cortex,
		const std::string& item,
		int match_flags,
		const std::string& action,
		std::vector<ItemLocation>& candidates);
	~PokeBankAction();

	void cancel() override;

	bool done() override;
	Result::Status on_match(int tab_index, int item_index) override;
	Result::Status on_empty() override;

	std::string action;

private:
	enum
	{
		state_start,
		state_right_click,
		state_select,
		state_failure,
		state_success
	};
	int state = state_start;

	Cortex::Result current_result;
};

struct autonomaus::BankWindowCortex::InventoryAction : public Action
{
	InventoryAction(
		BankWindowCortex* cortex,
		const std::string& item,
		int match_flags);
	~InventoryAction();

	Result get_result() override;
	void update() override;

	virtual void cancel();

	virtual Result::Status on_match(const chomsky::GUIContent& slot) = 0;

	BankWindowCortex* cortex;

	std::string item;
	int match_flags;

private:
	enum
	{
		state_start,
		state_next,
		state_hover,
		state_examine,
		state_match,
		state_error,
		state_done
	};
	int state = state_start;
	int previous_state;
	float exception_time = 0.0f;

	Result current_result;

	std::vector<chomsky::GUIContent> slots;
	std::size_t current_slot = 0;

	Result::StatusReference status;
	Result final_result;
};

struct autonomaus::BankWindowCortex::DepositAction : public InventoryAction
{
	DepositAction(
		BankWindowCortex* cortex,
		const std::string& item,
		int match_flags,
		int quantity);
	~DepositAction();

	void cancel() override;
	Result::Status on_match(const chomsky::GUIContent& slot) override;

	int quantity;

private:
	enum
	{
		state_start,
		state_wait,
		state_right_click,
		state_select,
		state_failure,
		state_success,
		state_done
	};
	int state = state_start;

	Cortex::Result current_result;
};

struct autonomaus::BankWindowCortex::PokeInventoryAction : public InventoryAction
{
	PokeInventoryAction(
		BankWindowCortex* cortex,
		const std::string& item,
		int match_flags,
		const std::string& action);
	~PokeInventoryAction();

	void cancel() override;
	Result::Status on_match(const chomsky::GUIContent& slot) override;

	std::string action;

private:
	enum
	{
		state_start,
		state_right_click,
		state_wait,
		state_select,
		state_done
	};
	int state = state_start;

	Cortex::Result current_result;
};

autonomaus::BankWindowCortex::BankWindowCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::BankWindow", autonomaus, "*.main.windows[*]:bank-window"),
	tabs_query("*.tabs[*]"),
	scroll_bar_query("*.scroll-bar:button"),
	items_query("*.items[*]"),
	title_query("*.title"),
	background_query("*.background"),
	tab_content_separator_query("*.tab-content-separators[*].text"),
	equipment_button_query("*.buttons.equipment:button"),
	equipment_panel_query("*.equipment:equipment-panel-content"),
	equipment_panel_items_query("*.equipment:equipment-panel-content.items[*]"),
	inventory_items_query("*.inventory:bank-inventory-panel.items[*]"),
	close_button_query("*.close-button:button"),
	search_button_query("*.buttons.search:button"),
	note_items_button_query("*.buttons.note-items:button"),
	deposit_x_query("*.deposit-buttons.?:button"),
	x_usage_query("*.usage.?")
{
	on_pull(
		"free_space",
		[this](auto)
		{
			Message result;
			result.set_integer("result", this->get_free_space());
			return result;
		});

	on_pull(
		"used_space",
		[this](auto)
		{
			Message result;
			result.set_integer("result", this->get_used_space());
			return result;
		});

	on_pull(
		"num_tabs",
		[this](auto)
		{
			Message result;
			result.set_integer("result", this->get_num_tabs());
			return result;
		});

	on_pull(
		"active_tab",
		[this](auto)
		{
			Message result;
			result.set_integer("result", this->get_active_tab());
			return result;
		});

	on_pull(
		"num_items_in_tab",
		[this](auto& message)
		{
			Message result;
			result.set_integer("result", this->get_num_items_in_tab(message.get_integer("tab")));
			return result;
		});

	on_poke(
		"examine_bank",
		[this](auto& message)
		{
			if (message.has("mode"))
			{
				return this->examine_bank(message.get_integer("mode"));
			}
			else
			{
				return this->examine_bank();
			}
		});

	on_poke(
		"deposit_inventory",
		[this](auto& message)
		{
			if (message.has("tab"))
			{
				return this->deposit_inventory(message.get_integer("tab"));
			}
			else
			{
				return this->deposit_inventory();
			}
		});

	on_poke(
		"deposit_familiar",
		[this](auto& message)
		{
			if (message.has("tab"))
			{
				return this->deposit_familiar(message.get_integer("tab"));
			}
			else
			{
				return this->deposit_familiar();
			}
		});

	on_poke(
		"deposit_equipment",
		[this](auto& message)
		{
			if (message.has("tab"))
			{
				return this->deposit_equipment(message.get_integer("tab"));
			}
			else
			{
				return this->deposit_equipment();
			}
		});

	on_poke(
		"deposit_coins",
		[this](auto& message)
		{
			if (message.has("tab"))
			{
				return this->deposit_coins(message.get_integer("tab"));
			}
			else
			{
				return this->deposit_coins();
			}
		});

	on_pull(
		"has_item",
		[this](auto& message)
		{
			auto item = message.get_string("item");
			int quantity = 1;
			if (message.has("quantity"))
			{
				quantity = message.get_integer("quantity");
			}

			Message result;
			result.set_boolean("result", this->has_item(item, quantity));
			return result;
		});

	on_pull(
		"count_item",
		[this](auto& message)
		{
			Message result;
			result.set_integer("result", this->count_item(message.get_string("item")));
			return result;
		});

	on_poke(
		"withdraw",
		[this] (auto& message)
		{
			int quantity;
			if (message.has("quantity"))
			{
				quantity = message.get_integer("quantity");
			}
			else
			{
				quantity = 1;
			}

			bool noted;
			if (message.has("noted"))
			{
				noted = message.get_boolean("noted");
			}
			else
			{
				noted = false;
			}

			return this->withdraw(message.get_string("item"), quantity, noted);
		});

	on_poke(
		"deposit",
		[this] (auto& message)
		{
			int quantity;
			if (message.has("quantity"))
			{
				quantity = message.get_integer("quantity");
			}
			else
			{
				quantity = 1;
			}

			return this->deposit(message.get_string("item"), quantity);
		});

	on_poke(
		"equip",
		[this] (auto& message)
		{
			return this->equip(message.get_string("item"));
		});

	on_poke(
		"poke_bank",
		[this] (auto& message)
		{
			return this->poke_bank(
				message.get_string("item"),
				message.get_string("action"));
		});

	on_poke(
		"poke_inventory",
		[this] (auto& message)
		{
			return this->poke_inventory(
				message.get_string("item"),
				message.get_string("action"));
		});

	on_poke(
		"wait",
		[this](auto& message)
		{
			return this->wait(message.get_single("time"));
		});

	on_poke(
		"close",
		[this](auto)
		{
			return this->close();
		});
}

int autonomaus::BankWindowCortex::get_free_space() const
{
	auto f2p = get("usage::f2p");
	auto p2p = get("usage::p2p");

	auto used = f2p->get_integer("used") + p2p->get_integer("used");
	auto total = f2p->get_integer("total") + p2p->get_integer("total");

	return total - used;
}

int autonomaus::BankWindowCortex::get_used_space() const
{
	auto f2p = get("usage::f2p");
	auto p2p = get("usage::p2p");

	auto used = f2p->get_integer("used") + p2p->get_integer("used");

	return used;
}

bool autonomaus::BankWindowCortex::has_item(const std::string& item, int quantity) const
{
	return count_item(item) >= quantity;
}

int autonomaus::BankWindowCortex::count_item(const std::string& item) const
{
	auto tabs = get_all("tab::(.*)");

	int quantity = 0;
	for (auto& i: tabs)
	{
		auto& tab = i.second;

		std::size_t num_items = tab->length("name");
		for (std::size_t i = 0; i < num_items; ++i)
		{
			if (fuzzy_string_match(tab->get_string("name", i), item))
			{
				quantity += tab->get_integer("quantity", i);
			}
		}
	}

	return quantity;
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::withdraw(
	const std::string& item,
	int quantity,
	bool noted,
	int match_flags)
{
	Log::info("(BankWindowCortex) Withdrawing item '{}'...", item);

	std::vector<ItemLocation> candidates;
	if (!try_get_item_locations(item, candidates, match_flags))
	{
		Log::warn("(BankWindowCortex) Item not found in bank. Have you tried running 'examine_bank'?");
		return Result::Status::failure;
	}

	return perform(new WithdrawAction(this, item, match_flags, quantity, noted, candidates));
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::deposit(
	const std::string& item,
	int quantity,
	int match_flags)
{
	Log::info("(BankWindowCortex) Depositing item '{}'...", item);

	return perform(new DepositAction(this, item, match_flags, quantity));
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::equip(
	const std::string& item,
	int match_flags)
{
	Log::info("(BankWindowCortex) Equipping item '{}'...", item);

	std::vector<ItemLocation> candidates;
	if (!try_get_item_locations(item, candidates, match_flags))
	{
		Log::warn("(BankWindowCortex) Item not found in bank. Have you tried running 'examine_bank'?");
		return Result::Status::failure;
	}

	return perform(new PokeBankAction(this, item, match_flags, "%(Wield|Wear)", candidates));
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::poke_bank(
	const std::string& item,
	const std::string& action,
	int match_flags)
{
	Log::info("(BankWindowCortex) Performing action '{}' on item '{}' in bank...", action, item);

	std::vector<ItemLocation> candidates;
	if (!try_get_item_locations(item, candidates, match_flags))
	{
		Log::warn("(BankWindowCortex) Item not found in bank. Have you tried running 'examine_bank'?");
		return Result::Status::failure;
	}

	return perform(new PokeBankAction(this, item, match_flags, action, candidates));
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::poke_inventory(
	const std::string& item,
	const std::string& action,
	int match_flags)
{
	Log::info("(BankWindowCortex) Performing action '{}' on item '{}' in inventory...", action, item);

	return perform(new PokeInventoryAction(this, item, match_flags, action));
}

int autonomaus::BankWindowCortex::get_num_tabs() const
{
	auto memory = get("tab");
	return memory->get_integer("count");
}

int autonomaus::BankWindowCortex::get_active_tab() const
{
	auto memory = get("tab");
	return memory->get_integer("active");
}

int autonomaus::BankWindowCortex::get_num_items_in_tab(int tab) const
{
	std::string key = "tab::";
	key += std::to_string(tab);

	auto memory = get(key);
	return memory->length("key");
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::examine_bank(int mode)
{
	Log::info("(BankWindowCortex) Examining bank...");
	return perform(new ExamineAction(this, mode));
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::deposit_inventory(int tab)
{
	Log::info("(BankWindowCortex) Depositing inventory.");
	return deposit_all_x("inventory", tab);
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::deposit_familiar(int tab)
{
	Log::info("(BankWindowCortex) Depositing familiar inventory.");
	return deposit_all_x("familiar", tab);
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::deposit_equipment(int tab)
{
	Log::info("(BankWindowCortex) Depositing equipment.");
	return deposit_all_x("equipment", tab);
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::deposit_coins(int tab)
{
	Log::info("(BankWindowCortex) Depositing coins.");
	return deposit_all_x("coins", tab);
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::wait(float max_time)
{
	MotionSequence sequence(*this);
	sequence.wait(max_time, [this] { return this->is_visible() && this->is_ready(); });

	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::close()
{
	Log::info("(BankWindowCortex) Closing bank...");

	chomsky::GUIContent button;
	if (!try_get_child(this->close_button_query, button))
	{
		Log::warn("(BankWindowCortex) Couldn't get close button. Is bank open?");

		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.left_click(button, [this] { return this->is_visible(); });
	sequence.wait(
		Config::get<float>("common", "gui", "ACTION_WAIT_TIME"),
		[this] { return !this->is_visible(); });

	return perform(sequence);
}

bool autonomaus::BankWindowCortex::is_ready() const
{
	return GUICortex::is_ready() && !this->loading;
}

void autonomaus::BankWindowCortex::mark_dirty()
{
	this->dirty = true;
}

void autonomaus::BankWindowCortex::invalidate_tab(int tab_index, int item_index)
{
	Memory tab;
	std::string tab_key;
	{
		tab_key = "tab::";
		tab_key += std::to_string(tab_index);

		if (!has_memory(tab_key))
		{
			return;
		}
		else
		{
			tab = get(tab_key);
		}
	}

	auto item_cortex = get_autonomaus().get_userdata<ItemCortex>();
	if (item_cortex == nullptr)
	{
		return;
	}

	std::size_t item_count = tab->length("key");
	for (std::size_t i = (item_index < 0) ? 0 : item_index; i < item_count; ++i)
	{
		auto key = tab->get_string("key", i);
		if (!item_cortex->has_name(key) || item_cortex->is_ambiguous(key))
		{
			tab->set_string("name", std::string(), i);
		}
	}

	remember(tab_key, tab);
}

bool autonomaus::BankWindowCortex::is_dirty() const
{
	return this->dirty;
}

void autonomaus::BankWindowCortex::update()
{
	GUICortex::update();

	if (is_visible())
	{
		if (!this->was_visible)
		{
			this->loading = true;
			this->was_visible = true;
			this->item_count = 0;
			this->last_load_time = get_current_time();
		}

		update_tabs();
		update_scroll_view();

		int count = update_items();
		if (count > this->item_count)
		{
			this->loading = true;
			this->last_load_time = get_current_time();
			this->item_count = count;
		}
		else
		{
			auto current_time = get_current_time();
			if (this->last_load_time + LOAD_TIME < current_time)
			{
				this->loading = false;
			}
		}
		this->item_count = count;

		update_usage("f2p");
		update_usage("p2p");
	}
	else
	{
		this->was_visible = false;
	}
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::deposit_all_x(
	const std::string& button, int tab)
{
	auto deposit_query = deposit_x_query;
	deposit_query.set_input(0, button);

	chomsky::GUIContent content;
	if (!try_get_child(deposit_query, content))
	{
		Log::warn("(BankWindowCortex) Couldn't find deposit button.");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	if (tab >= 0 && tab < get_num_tabs() && tab != get_active_tab())
	{
		chomsky::GUIContent root;
		if (!try_get_root(root))
		{
			Log::warn("(BankWindowCortex) Couldn't get root. Bad logic!");
			return Result::Status::failure;
		}

		std::vector<chomsky::GUIContent> tabs;
		if (!this->tabs_query.collect(root, tabs) || tabs.empty())
		{
			Log::warn("(BankWindowCortex) Couldn't get tabs. Bad logic!");
			return Result::Status::failure;
		}

		if (tab >= (int)tabs.size())
		{
			Log::warn("(BankWindowCortex) Tab out-of-bounds. Bad logic!");
			return Result::Status::failure;
		}

		sequence.left_click(tabs[tab], [this] { return this->is_visible(); });
		sequence.wait(
			Config::get<float>("common", "gui", "ACTION_WAIT_TIME"),
			[this, tab] { return this->is_visible() && this->get_active_tab() == tab; });
	}
	sequence.left_click(content, [this] { return this->is_visible(); });

	return perform(sequence);
}

bool autonomaus::BankWindowCortex::is_searching() const
{
	chomsky::GUIContent content;
	if (!try_get_child(this->title_query, content))
	{
		return false;
	}

	std::string title;
	if (!try_get_text(content, title))
	{
		return false;
	}

	return title.find("search") != std::string::npos;
}

bool autonomaus::BankWindowCortex::is_equipment_panel_open() const
{
	chomsky::GUIContent content;
	return try_get_child(this->equipment_panel_query, content);
}

bool autonomaus::BankWindowCortex::is_noting_enabled() const
{
	chomsky::GUIContent button;
	if (!try_get_child(this->note_items_button_query, button))
	{
		return false;
	}

	return button.get_tags().has("hover");
}

void autonomaus::BankWindowCortex::update_scroll_view()
{
	float scroll = 0.0f;
	float area = 0.0f;
	if (try_get_vertical_scroll_bar_percent(this->scroll_bar_query, scroll) &&
		try_get_vertical_scroll_area(this->scroll_bar_query, SCROLL_REGION_HEIGHT, area))
	{
		float n_area = std::floor(area / ITEM_HEIGHT + 0.5f) * ITEM_HEIGHT;
		this->current_scroll = scroll;
		this->current_scroll_offset = std::floor((std::floor(n_area) - SCROLL_REGION_HEIGHT) * this->current_scroll);

		this->max_num_rows = std::floor(area) / ITEM_HEIGHT;

		float top = this->current_scroll_offset / ITEM_HEIGHT;
		this->top_row = std::floor(top);
	}
	else
	{
		this->current_scroll = 0.0f;
		this->current_scroll_offset = 0.0f;
		this->max_num_rows = SCROLL_REGION_HEIGHT / ITEM_HEIGHT;
		this->top_row = 0;
	}

	chomsky::GUIContent tab_zero;
	if (try_get_child(this->tabs_query, tab_zero))
	{
		this->item_content_offset.x = tab_zero.get_x() + SCROLL_REGION_OFFSET_X;
		this->item_content_offset.y = tab_zero.get_y() + SCROLL_REGION_OFFSET_Y;
	}
}

void autonomaus::BankWindowCortex::update_tabs()
{
	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		return;
	}

	std::vector<chomsky::GUIContent> tabs;
	if (!this->tabs_query.collect(root, tabs) || tabs.empty())
	{
		return;
	}

	int active_index = 0;
	int current_index = 0;
	for (auto& tab: tabs)
	{
		if (tab.get_tags().has("active"))
		{
			active_index = current_index;
			break;
		}

		++current_index;
	}

	auto tab = get("tab");
	tab->set_integer("active", active_index);
	tab->set_integer("count", (int)tabs.size());

	remember("tab", tab);
}

int autonomaus::BankWindowCortex::update_items()
{
	auto item_cortex = get_autonomaus().get_userdata<ItemCortex>();
	if (item_cortex == nullptr)
	{
		return 0;
	}

	if (is_searching() || is_equipment_panel_open())
	{
		return 0;
	}

	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		return 0;
	}

	std::vector<chomsky::GUIContent> item_contents;
	if (!this->items_query.collect(root, item_contents) || item_contents.empty())
	{
		return 0;
	}

	std::vector<chomsky::GUIContent> separators;
	this->tab_content_separator_query.collect(root, separators);

	float unsorted_offset = 0.0f;
	if (separators.size() == 0)
	{
		if (this->tab_2_separator == HUGE_VALF && this->top_row != 0 && get_active_tab() == 0)
		{
			// We only want unsorted items. If we don't know where the tab
			// separator is, and we're not at the beginning, it could
			// potentially be before the current scroll region.
			return (int)item_contents.size();
		}
		else
		{
			unsorted_offset = SCROLL_REGION_HEIGHT;
		}
	}
	else
	{
		if (separators[0].get_text().to_string() == "Tab 2")
		{
			float s = separators[0].get_y() + separators[0].get_height();
			this->tab_2_separator = this->current_scroll_offset + (this->item_content_offset.y - s);
		}
		else
		{
			if (this->tab_2_separator == HUGE_VALF)
			{
				return item_contents.size();
			}
		}

		unsorted_offset = this->tab_2_separator;
	}

	Memory tab;
	std::string tab_key;
	{
		tab_key = "tab::";
		tab_key += std::to_string(get_active_tab());

		if (!has_memory(tab_key))
		{
			tab->set_integer("index", get_active_tab());
		}
		else
		{
			tab = get(tab_key);
		}
	}

	for (auto& item_content: item_contents)
	{
		if (get_active_tab() == 0)
		{
			float y = item_content.get_y() + item_content.get_height();
			y = this->current_scroll_offset + (this->item_content_offset.y - y);
			if (y - unsorted_offset > 0.0f)
			{
				continue;
			}
		}

		int index;
		if (!try_get_item_index(item_content, index))
		{
			continue;
		}

		Memory item;
		if (!try_get_item(item_content, item))
		{
			continue;
		}

		std::string key = item->get_string("key");
		this->item_key_to_runtime_key[key] = item->get_string("runtime-key");

		Memory current;
		if (!has_memory(key))
		{
			current = item;
			current->unset("quantity");
			current->unset("runtime-key");
		}
		else
		{
			current = get(key);
		}
		current->set_boolean("tab", get_active_tab(), true);

		remember(key, current);

		if (tab->get_string("key", index) != item->get_string("key"))
		{
			tab->set_string("name", std::string(), index);
		}

		if (tab->get_string("name", index).empty() &&
			!item_cortex->is_ambiguous(item->get_string("key")))
		{
			tab->set_string("name", item_cortex->get_name(item->get_string("key")), index);
		}

		tab->set_string("key", item->get_string("key"), index);
		tab->set_integer("quantity", item->get_integer("quantity"), index);
	}

	std::size_t max_items = std::min(
		(int)tab->length("key"),
		this->max_num_rows * (SCROLL_REGION_WIDTH / ITEM_HEIGHT));
	tab->length("key", max_items);
	tab->length("name", max_items);
	tab->length("quantity", max_items);

	remember(tab_key, tab);

	return (int)item_contents.size();
}

void autonomaus::BankWindowCortex::update_usage(const std::string& type)
{
	auto usage_query = x_usage_query;
	usage_query.set_input(0, type);

	chomsky::GUIContent usage_content;
	if (!try_get_child(usage_query, usage_content))
	{
		return;
	}

	std::string text;
	if (!try_get_text(usage_content, text))
	{
		return;
	}

	const char* start = text.c_str();
	const char* end = nullptr;
	int used = std::strtol(start, (char**)(&end), 10);
	int total = std::strtol(end, nullptr, 10);
	
	Memory usage;
	usage->set_integer("used", used);
	usage->set_integer("total", total);

	std::string key = "usage::";
	key += type;
	remember(key, usage);
}

#include "rosalind/common/re.hpp"

// XXX: This code is fragile. Don't touch it without thorough testing.
bool autonomaus::BankWindowCortex::try_get_item_index(
	const chomsky::GUIContent& item,
	int& index)
{
	// TODO (maybe): Get item icon height from somewhere else?
	if (item.get_height() < rosalind::re::ITEM_ICON_HEIGHT)
	{
		return false;
	}

	float x = item.get_x() - this->item_content_offset.x;
	float y = this->item_content_offset.y - (item.get_y() + item.get_height());
	y += this->current_scroll_offset;

	int i = x / ITEM_WIDTH;
	int j = std::floor(y / ITEM_HEIGHT + 0.5f);
	int row_width = SCROLL_REGION_WIDTH / ITEM_WIDTH;

	if (i < 0 || i > row_width)
	{
		return false;
	}

	if (j < 0 || j > this->max_num_rows)
	{
		return false;
	}

	index = j * row_width + i;
	return true;
}

bool autonomaus::BankWindowCortex::try_get_item_locations(
	const std::string& item,
	std::vector<ItemLocation>& result,
	int match_flags)
{
	auto item_cortex = get_autonomaus().get_userdata<ItemCortex>();
	if (item_cortex == nullptr)
	{
		return false;
	}

	auto tabs = get_all("tab::(.*)");
	std::set<ItemLocation> named_candidates;
	std::set<ItemLocation> potential_candidates;
	for (auto& i: tabs)
	{
		auto& tab = i.second;

		std::size_t num_items = tab->length("name");
		for (std::size_t i = 0; i < num_items; ++i)
		{
			if (fuzzy_string_match(tab->get_string("name", i), item, match_flags))
			{
				named_candidates.emplace(tab->get_integer("index"), (int)i);
			}
			else if (item_cortex != nullptr)
			{
				auto key = tab->get_string("key", i);
				if (item_cortex->is_ambiguous(key))
				{
					for (auto& name: item_cortex->get_names(key))
					{
						if (fuzzy_string_match(name, item, match_flags))
						{
							potential_candidates.emplace(tab->get_integer("index"), (int)i);
						}
					}
				}
			}
		}
	}

	if (named_candidates.empty() && potential_candidates.empty())
	{
		return false;
	}

	// First, we insert candidates who are in the same tab as us.
	for (auto& i: named_candidates)
	{
		if (std::get<0>(i) == get_active_tab())
		{
			result.push_back(i);
		}
	}

	for (auto& i: potential_candidates)
	{
		if (std::get<0>(i) == get_active_tab())
		{
			result.push_back(i);
		}
	}

	// Then we insert the candidates in other tabs.
	for (auto& i: named_candidates)
	{
		if (std::get<0>(i) != get_active_tab())
		{
			result.push_back(i);
		}
	}

	for (auto& i: potential_candidates)
	{
		if (std::get<0>(i) != get_active_tab())
		{
			result.push_back(i);
		}
	}

	return true;
}

autonomaus::BankWindowCortex::IterateAction::IterateAction(BankWindowCortex* cortex) :
	status(std::make_shared<Cortex::Result::Status>(Cortex::Result::Status::working)),
	final_result(status, [this] { this->cancel(); })
{
	this->cortex = cortex;
}

autonomaus::BankWindowCortex::IterateAction::~IterateAction()
{
	if (*this->status == Result::Status::working)
	{
		*this->status = Result::Status::failure;
	}

	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	this->final_result.terminate();
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::IterateAction::get_result()
{
	return this->final_result;
}

void autonomaus::BankWindowCortex::IterateAction::update()
{
	if (!this->cortex->is_visible())
	{
		if (this->state != state_error && this->state != state_done)
		{
			this->previous_state = this->state;
			this->state = state_error;

			const float wait_time = Config::get<float>("common", "gui", "GUI_GUARD_WAIT_TIME");
			this->exception_time = get_current_time() + wait_time;
		}
	}

	switch (this->state)
	{
		case state_start:
			if (this->cortex->is_equipment_panel_open())
			{
				chomsky::GUIContent button;
				if (!this->cortex->try_get_child(this->cortex->equipment_button_query, button))
				{
					Log::warn("(BankWindowCortex::IterateAction) Couldn't close equipment panel: button not found.");
					cancel();
				}

				MotionSequence sequence(*this->cortex);
				sequence.left_click(button, [this] { return this->cortex->is_visible(); });
				sequence.wait(
					Config::get<float>("common", "gui", "ACTION_WAIT_TIME"),
					[this] { return this->cortex->is_visible() && !this->cortex->is_equipment_panel_open(); });

				this->current_result = this->cortex->perform(sequence);
				this->state = state_close_equipment;
			}
			else
			{
				this->state = state_on_start;
			}
			break;
		case state_close_equipment:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->state = state_on_start;
				}
				else
				{
					Log::warn("(BankWindowCortex::IterateAction) Couldn't close equipment panel.");
					cancel();
				}
			}
			break;
		case state_on_start:
			{
				auto result = on_start();
				if (result == Result::Status::success)
				{
					this->state = state_next;
				}
				else if (result == Result::Status::failure)
				{
					cancel();
				}
			}
			break;
		case state_next:
			if (done())
			{
				*this->status = Result::Status::success;
				this->state = state_done;
				break;
			}
			else
			{
				auto iter = next();
				int tab_index = std::get<0>(iter);
				if (tab_index != this->cortex->get_active_tab())
				{
					chomsky::GUIContent root;
					std::vector<chomsky::GUIContent> tabs;
					if (!this->cortex->try_get_root(root) ||
						!this->cortex->tabs_query.collect(root, tabs) ||
						tabs.empty())
					{
						Log::warn("(BankWindowCortex::IterateAction) Couldn't get tabs. Bad logic!");
						cancel();
						break;
					}

					if (tab_index < 0 || tab_index >= (int)tabs.size())
					{
						Log::warn("(BankWindowCortex::IterateAction) Tab index {} out of range.", tab_index);
						cancel();
						break;
					}

					MotionSequence sequence(*this->cortex);
					sequence.left_click(tabs[tab_index], [this] { return this->cortex->is_visible(); });
					sequence.wait(
						Config::get<float>("common", "gui", "ACTION_WAIT_TIME"),
						[this, tab_index] { return this->cortex->get_active_tab() == tab_index; });
					sequence.wait(
						Config::get<float>("gui", "BankWindowCortex", "MAX_LOAD_TIME"),
						[this] { return this->cortex->is_ready(); });

					this->current_result = this->cortex->perform(sequence);
					this->state = state_tab;
				}
				else
				{
					if (this->current_tab != tab_index)
					{
						this->current_tab = tab_index;
						this->state = state_on_tab;
					}
					else
					{
						this->state = state_after_tab;
					}
				}

				this->current_iter = iter;
			}
			break;
		case state_tab:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() != Result::Status::success)
				{
					Log::warn("Switching to tab {} failed.", std::get<0>(this->current_iter));
					cancel();
					break;
				}

				this->state = state_on_tab;
			}
			break;
		case state_on_tab:
			{
				auto result = on_tab();
				if (result == Result::Status::success)
				{
					this->state = state_after_tab;
				}
				else if (result == Result::Status::failure)
				{
					cancel();
				}
			}
			break;
		case state_after_tab:
			{
				int item_index = std::get<1>(current_iter);
				if (item_index < 0)
				{
					this->state = state_next;
					break;
				}

				int item_row = item_index / (SCROLL_REGION_WIDTH / ITEM_WIDTH);

				int num_rows;
				if (this->cortex->get_active_tab() == 0)
				{
					num_rows = this->cortex->max_num_rows - (this->cortex->get_num_tabs() - 1);
				}
				else
				{
					num_rows = this->cortex->max_num_rows;
				}

				if (item_row > num_rows)
				{
					this->state = state_next;
					break;
				}

				float num_visible_rows = SCROLL_REGION_HEIGHT / ITEM_HEIGHT;
				float num_invisible_rows = num_rows * ITEM_HEIGHT;
				num_invisible_rows -= SCROLL_REGION_HEIGHT;
				num_invisible_rows /= ITEM_HEIGHT;

				if (item_row > this->cortex->top_row &&
					item_row < this->cortex->top_row + num_visible_rows)
				{
					this->state = state_find;
					break;
				}
				else if (item_row == 0 && this->cortex->current_scroll < 1.0f / this->cortex->max_num_rows)
				{
					this->state = state_find;
					break;
				}
				else if (item_row == this->cortex->top_row + num_visible_rows &&
					this->cortex->current_scroll > (this->cortex->max_num_rows - 1.0f) / this->cortex->max_num_rows)
				{
					this->state = state_find;
					break;
				}

				int desired_row;
				if (item_row < num_visible_rows)
				{
					desired_row = 0;
				}
				else if (item_row < num_rows - num_visible_rows)
				{
					desired_row = item_row - num_visible_rows / 2.0f;
					desired_row = std::max(desired_row, 0);
					desired_row = std::min(desired_row, num_rows);
				}
				else
				{
					desired_row = num_rows;
				}
				float desired_scroll = desired_row / (float)num_rows;

				chomsky::GUIContent scroll_bar;
				if (!this->cortex->try_get_child(this->cortex->scroll_bar_query, scroll_bar))
				{
					this->state = state_find;
					break;
				}

				chomsky::GUIContent scroll_slider = scroll_bar.get("scroll-slider-button");
				chomsky::GUIContent scroll_down = scroll_bar.get("scroll-down-button");
				chomsky::GUIContent scroll_up = scroll_bar.get("scroll-up-button");

				int scroll_area = scroll_up.get_y();
				scroll_area -= scroll_down.get_y();
				scroll_area -= scroll_down.get_height();
				scroll_area -= scroll_slider.get_height();
				int desired_scroll_offset = scroll_area * desired_scroll;
				int desired_scroll_bar_y = scroll_up.get_y() - desired_scroll_offset;
				int distance = desired_scroll_bar_y - (scroll_slider.get_y() + scroll_slider.get_height());

				MotionSequence sequence(*this->cortex);
				sequence.drag(
					scroll_slider,
					0, distance,
					1.0f,
					[this] { return this->cortex->is_visible(); });
				sequence.wait(
					Config::get<float>("gui", "BankWindowCortex", "MAX_LOAD_TIME"),
					[this] { return this->cortex->is_ready(); });

				this->current_result = this->cortex->perform(sequence);
				this->state = state_scroll;
			}
			break;
		case state_scroll:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					// In-case we mis-scrolled.
					this->state = state_after_tab;
				}
				else
				{
					Log::warn("(BankWindowCortex::IterateAction) Scrolling failed.");
					cancel();
				}
			}
			break;
		case state_find:
			{
				chomsky::GUIContent root;
				if (!this->cortex->try_get_root(root))
				{
					Log::warn("(BankWindowCortex::IterateAction) Couldn't get root. Bad logic!");
					cancel();
					break;
				}

				std::vector<chomsky::GUIContent> items;
				if (!this->cortex->items_query.collect(root, items) ||
					items.empty())
				{
					Log::warn("(BankWindowCortex::IterateAction) No items. Bad logic!");
					cancel();
					break;
				}

				chomsky::GUIContent target;
				bool found_item = false;
				for (auto& item: items)
				{
					if (this->cortex->get_active_tab() == 0)
					{
						float y = item.get_y() + item.get_height();
						y = this->cortex->current_scroll_offset + (this->cortex->item_content_offset.y - y);
						if (y - this->cortex->tab_2_separator > 0.0f)
						{
							continue;
						}
					}

					int index;
					if (!this->cortex->try_get_item_index(item, index))
					{
						continue;
					}

					if (index == std::get<1>(this->current_iter))
					{
						target = item;
						found_item = true;
					}
				}

				if (!found_item)
				{
					this->state = state_next;
					break;
				}

				MotionSequence sequence(*this->cortex);
				sequence.hover(target, [this] { return this->cortex->is_visible(); });
				sequence.wait<ToolTipCortex>(
					Config::get<float>("common", "gui", "GUI_GUARD_WAIT_TIME"));

				this->current_result = this->cortex->perform(sequence);
				this->state = state_hover;
			}
			break;
		case state_hover:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->state = state_on_hover;
				}
				else
				{
					Log::warn("(BankWindowCortex::IterateAction) Hovering item failed.");
					cancel();
				}
			}
			break;
		case state_on_hover:
			{
				auto result = on_item();
				if (result == Result::Status::success)
				{
					this->state = state_next;
				}
				else if (result == Result::Status::failure)
				{
					cancel();
				}
			}
			break;
		case state_error:
			if (this->cortex->is_visible())
			{
				this->state = this->previous_state;
			}
			else
			{
				auto& game_state = this->cortex->get_autonomaus().get_game_state();
				if (game_state.get_last_update_timestamp() > this->exception_time)
				{
					Log::warn("(BankWindowCortex::IterateAction) Bank no longer visible.");
					cancel();
				}
			}
			break;
		case state_done:
			// Nothing.
			break;
		default:
			Log::warn("(BankWindowCortex::IterateAction) Unhandled state {}. Bad logic!", this->state);
			cancel();
			break;
	}
}

void autonomaus::BankWindowCortex::IterateAction::cancel()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	if (this->state != state_done)
	{
		*this->status = Result::Status::failure;
		this->state = state_done;
	}
}

autonomaus::BankWindowCortex::ExamineAction::ExamineAction(
	BankWindowCortex* cortex, int mode) :
		IterateAction(cortex)
{
	this->mode = mode;
}

autonomaus::BankWindowCortex::ExamineAction::~ExamineAction()
{
	if (this->current_motion.pending())
	{
		this->current_motion.cancel();
	}
}

void autonomaus::BankWindowCortex::ExamineAction::cancel()
{
	if (this->current_motion.pending())
	{
		this->current_motion.cancel();
	}

	IterateAction::cancel();
}

bool autonomaus::BankWindowCortex::ExamineAction::done()
{
	if (this->state == state_start)
	{
		this->num_tabs = this->cortex->get_num_tabs();
		this->current_tab = 0;

		return false;
	}

	if (this->current_tab >= this->num_tabs && this->pending_items.empty())
	{
		return true;
	}
	
	return false;
}

autonomaus::BankWindowCortex::ItemLocation
autonomaus::BankWindowCortex::ExamineAction::next()
{
	if (this->pending_items.empty())
	{
		auto result = std::make_tuple(this->current_tab, -1);
		++this->current_tab;

		return result;
	}
	else
	{
		this->current_index = *this->pending_items.rbegin();
		this->pending_items.erase(this->current_index);
		this->visited_items.insert(this->current_index);

		return std::make_tuple(this->cortex->get_active_tab(), this->current_index);
	}
}

autonomaus::Cortex::Result::Status
autonomaus::BankWindowCortex::ExamineAction::on_tab()
{
	if (this->state != state_tab)
	{
		this->pending_items.clear();
		this->visited_items.clear();

		update_pending_items();

		chomsky::GUIContent scroll_bar;
		if (!this->cortex->try_get_child(this->cortex->scroll_bar_query, scroll_bar))
		{
			return Result::Status::success;
		}

		MotionSequence sequence(*this->cortex);
		sequence.drag(
			scroll_bar.get("scroll-slider-button"),
			scroll_bar.get("scroll-down-button"),
			1.0f,
			[this] { return this->cortex->is_visible(); });
		sequence.wait(
			Config::get<float>("gui", "BankWindowCortex", "MAX_LOAD_TIME"),
			[this] { return this->cortex->is_ready(); });

		this->current_motion = this->cortex->perform(sequence);
		this->state = state_tab;
	}

	if (this->current_motion.get() == Result::Status::success)
	{
		if ((int)(this->cortex->current_scroll_offset * 100) < 99)
		{
			this->state = state_tab_again;
			return Result::Status::working;
		}

		if (!update_pending_items())
		{
			return Result::Status::failure;
		}
	}

	return this->current_motion.get();
}

autonomaus::Cortex::Result::Status
autonomaus::BankWindowCortex::ExamineAction::on_item()
{
	update_pending_items();

	// On failure, we just go to the next item unless it's critical.
	switch (this->state)
	{
		case state_item_start:
			{
				std::string key;
				{
					Memory tab;
					{
						std::string key = "tab::";
						key += std::to_string(this->cortex->get_active_tab());
						tab = this->cortex->get(key);
					}

					key = tab->get_string("key", this->current_index);

					if (key.empty())
					{
						return Result::Status::success;
					}
				}

				std::string runtime_key;
				{
					auto iter = this->cortex->item_key_to_runtime_key.find(key);
					if (iter == this->cortex->item_key_to_runtime_key.end())
					{
						return Result::Status::success;
					}

					if (iter->second.empty())
					{
						return Result::Status::success;
					}

					runtime_key = iter->second;
				}

				this->current_item_key = key;
				this->current_runtime_key = runtime_key;

				static const float MIN_WAIT = Config::get<float>("gui", "BankWindowCortex", "EXAMINE_WAIT_MIN");
				static const float MAX_WAIT = Config::get<float>("gui", "BankWindowCortex", "EXAMINE_WAIT_MAX");
				this->wait_time = get_current_time() + RNG::nextf(MIN_WAIT, MAX_WAIT);
				this->state = state_item_wait;
			}
			break;
		case state_item_wait:
			if (get_current_time() > this->wait_time)
			{
				this->state = state_item_examine;
			}
			break;
		case state_item_examine:
			{
				auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
				auto items = this->cortex->get_autonomaus().get_userdata<ItemCortex>();
				if (tool_tip == nullptr)
				{
					Log::warn("(BankWindowCortex::ExamineAction) Tool tip cortex not valid. Bad logic!");
					return Result::Status::failure;
				}
				else if (items == nullptr)
				{
					Log::warn("(BankWindowCortex::ExamineAction) Item cortex not valid. Bad logic!");
					return Result::Status::failure;
				}
				else
				{
					auto text = tool_tip->get_text();

					// See state_examine in InventoryPanel::ExamineAction::update
					if (text.size() < 2)
					{
						Log::warn(
							"(BankWindowCortex::ExamineAction) Tool tip malformed."
							" Expected at least two values, got {}.",
							text.size());
						return Result::Status::success;
					}

					auto name = trim_string(text[1]);

					Log::info("(BankWindowCortex::ExamineAction) Got item '{0}'.", name);
					items->remember_item(this->current_runtime_key);
					items->add_name(this->current_item_key, name);

					Memory tab;
					std::string tab_key;
					{
						tab_key = "tab::";
						tab_key += std::to_string(this->cortex->get_active_tab());

						tab = this->cortex->get(tab_key);
					}
					tab->set_string("name", name, this->current_index);
					this->cortex->remember(tab_key, tab);
				}

				this->state = state_item_done;

				return Result::Status::success;
			}
			break;
		default:
			this->state = state_item_start;
			break;
	}

	return Result::Status::working;
}

bool autonomaus::BankWindowCortex::ExamineAction::update_pending_items()
{
	Memory tab;
	std::string tab_key;
	{
		tab_key = "tab::";
		tab_key += std::to_string(this->cortex->get_active_tab());

		tab = this->cortex->get(tab_key);
	}

	auto item_cortex = this->cortex->get_autonomaus().get_userdata<ItemCortex>();
	if (item_cortex == nullptr)
	{
		Log::warn("(BankWindowCortex::ExamineAction) Item cortex is not valid. Bad logic!");
		return false;
	}
	
	std::size_t item_count = tab->length("key");
	for (std::size_t i = 0; i < item_count; ++i)
	{
		if (this->visited_items.count(i) != 0)
		{
			continue;
		}

		auto key = tab->get_string("key", i);
		if (key.empty())
		{
			continue;
		}

		auto name = tab->get_string("name", i);
		if (name.empty())
		{
			if (!item_cortex->has_name(key))
			{
				this->pending_items.insert(i);
			}
			else if (item_cortex->is_ambiguous(key))
			{
				this->pending_items.insert(i);
			}
			else
			{
				tab->set_string("name", item_cortex->get_name(key), i);
			}
		}

		if (item_cortex->is_ambiguous(key) && mode >= examine_ambiguous)
		{
			this->pending_items.insert(i);
		}

		if (mode == examine_everything)
		{
			this->pending_items.insert(i);
		}
	}

	this->cortex->remember(tab_key, tab);

	return true;
}

autonomaus::BankWindowCortex::ItemAction::ItemAction(
	BankWindowCortex* cortex,
	const std::string& item,
	int match_flags,
	std::vector<ItemLocation>& candidates) :
		IterateAction(cortex),
		candidates(std::move(candidates))
{
	this->item = item;
	this->match_flags = match_flags;
}

autonomaus::BankWindowCortex::ItemLocation
autonomaus::BankWindowCortex::ItemAction::next()
{
	if (this->candidates.empty())
	{
		on_empty();
		return this->current_location;
	}

	this->current_location = this->candidates.front();
	this->candidates.erase(this->candidates.begin());
	return this->current_location;
}

autonomaus::Cortex::Result::Status
autonomaus::BankWindowCortex::ItemAction::on_tab()
{
	return Result::Status::success;
}

autonomaus::Cortex::Result::Status
autonomaus::BankWindowCortex::ItemAction::on_item()
{
	auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
	if (tool_tip == nullptr)
	{
		Log::warn("(BankWindowCortex::WithdrawAction) Tool tip cortex not valid. Bad logic!");
		return Result::Status::failure;
	}

	switch (this->state)
	{
		case state_start:
			this->wait_time = get_current_time() + Config::get<float>("gui", "BankWindowCortex", "EXAMINE_WAIT_MIN");
			this->state = state_wait;
			break;
		case state_wait:
			if (tool_tip->is_visible())
			{
				auto text = tool_tip->get_text();
				if (text.size() >= 2)
				{
					auto name = trim_string(text[1]);
					if (fuzzy_string_match(name, this->item, this->match_flags))
					{
						this->state = state_done;
					}
					else
					{
						Memory tab;
						std::string tab_key;
						{
							tab_key = "tab::";
							tab_key += std::to_string(this->cortex->get_active_tab());

							tab = this->cortex->get(tab_key);
						}
						tab->set_string("name", name, std::get<1>(this->current_location));
						this->cortex->remember(tab_key, tab);

						this->state = state_skip;
					}
					break;
				}
			}

			if (this->wait_time < this->cortex->get_autonomaus().get_game_state().get_last_update_timestamp())
			{
				this->state = state_skip;
			}
			break;
		case state_skip:
			this->state = state_start;
			return Result::Status::success;
		case state_empty:
			{
				auto result = on_empty();
				if (result != Result::Status::working)
				{
					this->state = state_start;
				}

				return result;
			}
		case state_done:
			{
				auto result = on_match(
					std::get<0>(this->current_location),
					std::get<1>(this->current_location));
				if (result != Result::Status::working)
				{
					this->state = state_start;
				}

				return result;
			}
		default:
			Log::warn("(BankWindowCortex::ItemAction) Unhandled state {}. Bad logic!", this->state);
			cancel();
			break;
	}

	return Result::Status::working;
}

autonomaus::BankWindowCortex::WithdrawAction::WithdrawAction(
	BankWindowCortex* cortex,
	const std::string& item,
	int match_flags,
	int quantity,
	bool noted,
	std::vector<ItemLocation>& candidates) :
		ItemAction(cortex, item, match_flags, candidates)
{
	this->quantity = quantity;
	this->noted = noted;
}

autonomaus::BankWindowCortex::WithdrawAction::~WithdrawAction()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}
}

bool autonomaus::BankWindowCortex::WithdrawAction::done()
{
	return this->state == state_success;
}

autonomaus::Cortex::Result::Status
autonomaus::BankWindowCortex::WithdrawAction::on_start()
{
	if (this->current_result.get() != Result::Status::empty)
	{
		return this->current_result.get();
	}

	if (this->noted != this->cortex->is_noting_enabled())
	{
		chomsky::GUIContent button;
		if (!this->cortex->try_get_child(this->cortex->note_items_button_query, button))
		{
			Log::warn("(BankWindowCortex::WithdrawAction) Couldn't get note button. Bad logic!");
			return Result::Status::failure;
		}

		MotionSequence sequence(*this->cortex);
		sequence.left_click(button, [this] { return this->cortex->is_visible(); });

		this->current_result = this->cortex->perform(sequence);
		return Result::Status::working;
	}

	return Result::Status::success;
}

autonomaus::Cortex::Result::Status
autonomaus::BankWindowCortex::WithdrawAction::on_match(int tab_index, int item_index)
{
	auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
	if (tool_tip == nullptr)
	{
		Log::warn("(BankWindowCortex::WithdrawAction) Tool tip cortex not valid. Bad logic!");
		return Result::Status::failure;
	}

	auto right_click_menu = this->cortex->get_autonomaus().get_userdata<RightClickMenuCortex>();
	if (right_click_menu == nullptr)
	{
		Log::warn("(BankWindowCortex::WithdrawAction) Right click menu cortex not valid. Bad logic!");
		return Result::Status::failure;
	}

	switch (this->state)
	{
		case state_start:
			{
				MotionSequence sequence(*this->cortex);

				if (this->quantity == 1)
				{
					this->state = state_wait;
					sequence.left_click();
				}
				else
				{
					this->state = state_right_click;
					sequence.right_click();
					sequence.wait<RightClickMenuCortex>(
						Config::get<float>("common", "gui", "RIGHT_CLICK_WAIT_TIME"));
				}

				this->current_result = this->cortex->perform(sequence);
			}
			break;
		case state_wait:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->state = state_success;
				}
				else
				{
					this->state = state_failure;
				}
			}
			break;
		case state_right_click:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->state = state_select;
				}
				else
				{
					this->state = state_failure;
				}
			}
			break;
		case state_select:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() != Result::Status::success)
				{
					this->state = state_failure;
					break;
				}

				int option_index = -1;
				for (int i = 0; i < right_click_menu->get_num_options(); ++i)
				{
					auto text = right_click_menu->get_option_text(i);
					if (text.size() < 2)
					{
						// <color> <action> <color> <item-name>
						continue;
					}
					auto option = trim_string(text[0]);

					bool match = false;
					if (this->quantity == transfer_all && option == "Withdraw-All")
					{
						match = true;
					}
					else if (this->quantity == transfer_all_but_one && option == "Withdraw-All but one")
					{
						match = true;
					}
					else
					{
						std::string withdraw_x = "Withdraw-";
						withdraw_x += std::to_string(this->quantity);

						if (option == withdraw_x)
						{
							match = true;
						}
					}

					if (match)
					{
						option_index = i;
						break;
					}
				}

				MotionSequence sequence(*this->cortex);
				if (option_index >= 0)
				{
					Message message;
					message.set_integer("option", option_index);

					sequence.poke<RightClickMenuCortex>(
						"select",
						message,
						[this] { return this->cortex->is_visible(); });
				}
				else if (this->quantity > 0)
				{
					Message message;
					message.set_string("option", "%Withdraw-X.*");

					sequence.poke<RightClickMenuCortex>(
						"select",
						message,
						[this] { return this->cortex->is_visible(); });

					sequence.wait<DialogInputPromptCortex>(
						Config::get<float>("common", "gui", "WINDOW_WAIT_TIME"));

					sequence.type(
						std::to_string(quantity) + "\n",
						true,
						[this]
						{
							auto prompt = this->cortex->get_autonomaus().get_userdata<DialogInputPromptCortex>();

							return prompt != nullptr &&
								prompt->is_visible() &&
								this->cortex->is_visible();
						});
				}
				else
				{
					this->state = state_failure;
					break;
				}

				this->current_result = this->cortex->perform(sequence);
				this->state = state_wait;
			}
			break;
		case state_success:
			this->cortex->invalidate_tab(tab_index, item_index);
			return Result::Status::success;
		default:
			Log::warn("(BankWindowCortex::WithdrawAction) Unhandled state {}. Bad logic!", this->state);
		case state_failure:
			return Result::Status::failure;
	}

	return Result::Status::working;
}

autonomaus::Cortex::Result::Status
autonomaus::BankWindowCortex::WithdrawAction::on_empty()
{
	this->state = state_failure;
	return Result::Status::failure;
}

void autonomaus::BankWindowCortex::WithdrawAction::cancel()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	ItemAction::cancel();
}

autonomaus::BankWindowCortex::PokeBankAction::PokeBankAction(
	BankWindowCortex* cortex,
	const std::string& item,
	int match_flags,
	const std::string& action,
	std::vector<ItemLocation>& candidates):
		ItemAction(cortex, item, match_flags, candidates)
{
	this->action = action;
}

autonomaus::BankWindowCortex::PokeBankAction::~PokeBankAction()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}
}

bool autonomaus::BankWindowCortex::PokeBankAction::done()
{
	return this->state == state_success;
}

autonomaus::Cortex::Result::Status
autonomaus::BankWindowCortex::PokeBankAction::on_match(int tab_index, int item_index)
{
	switch (this->state)
	{
		case state_start:
			{
				MotionSequence sequence(*this->cortex);
				sequence.right_click();
				sequence.wait<RightClickMenuCortex>(
					Config::get<float>("common", "gui", "RIGHT_CLICK_WAIT_TIME"));

				this->current_result = this->cortex->perform(sequence);
				this->state = state_right_click;
			}
			break;
		case state_right_click:
			{
				if (!this->current_result.pending())
				{
					if (this->current_result.get() == Result::Status::success)
					{
						this->state = state_select;
					}
					else
					{
						this->state = state_failure;
					}
				}
			}
			break;
		case state_select:
			{
				auto right_click_menu = this->cortex->get_autonomaus().get_userdata<RightClickMenuCortex>();
				if (right_click_menu == nullptr)
				{
					Log::warn("(BankWindowCortex::PokeBankAction) Right click menu cortex not valid. Bad logic!");
					return Result::Status::failure;
				}

				int option_index = -1;
				for (auto i = 0; i < right_click_menu->get_num_options(); ++i)
				{
					auto text = right_click_menu->get_option_text(i);
					if (text.size() < 2)
					{
						continue;
					}

					auto option = trim_string(text[0]);
					if (fuzzy_string_match(option, this->action))
					{
						option_index = i;
						break;
					}
				}

				if (option_index >= 0)
				{
					this->current_result = right_click_menu->select(option_index);
					this->state = state_success;
				}
				else
				{
					this->state = state_failure;
				}
			}
			break;
		case state_failure:
			return Result::Status::failure;
		case state_success:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->cortex->invalidate_tab(tab_index, item_index);
				}

				return this->current_result.get();
			}
			break;
		default:
			Log::warn("(BankWindowCortex::PokeBankAction) Unhandled state {}. Bad logic!", this->state);
			return Result::Status::failure;
	}

	return Result::Status::working;
}

autonomaus::Cortex::Result::Status
autonomaus::BankWindowCortex::PokeBankAction::on_empty()
{
	return Result::Status::failure;
}

void autonomaus::BankWindowCortex::PokeBankAction::cancel()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	ItemAction::cancel();
}

autonomaus::BankWindowCortex::InventoryAction::InventoryAction(
	BankWindowCortex* cortex,
	const std::string& item,
	int match_flags) :
		status(std::make_shared<Cortex::Result::Status>(Cortex::Result::Status::working)),
		final_result(status, [this] { this->cancel(); })
{
	this->cortex = cortex;
	this->item = item;
	this->match_flags = match_flags;
}

autonomaus::BankWindowCortex::InventoryAction::~InventoryAction()
{
	if (*this->status == Result::Status::working)
	{
		*this->status = Result::Status::failure;
	}

	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	this->final_result.terminate();
}

autonomaus::Cortex::Result autonomaus::BankWindowCortex::InventoryAction::get_result()
{
	return this->final_result;
}

void autonomaus::BankWindowCortex::InventoryAction::update()
{
	auto item_cortex = this->cortex->get_autonomaus().get_userdata<ItemCortex>();
	if (item_cortex == nullptr)
	{
		Log::warn("(BankWindowCortex::InventoryAction) Item cortex is not valid. Bad logic!");
		cancel();
		return;
	}

	if (!this->cortex->is_visible())
	{
		if (this->state != state_error && this->state != state_done)
		{
			this->previous_state = this->state;
			this->state = state_error;

			const float wait_time = Config::get<float>("common", "gui", "GUI_GUARD_WAIT_TIME");
			this->exception_time = get_current_time() + wait_time;
		}
	}

	switch (this->state)
	{
		case state_start:
			{
				chomsky::GUIContent root;
				std::vector<chomsky::GUIContent> items;
				if (!this->cortex->try_get_root(root) ||
					!this->cortex->inventory_items_query.collect(root, items) ||
					items.empty())
				{
					Log::warn("(BankWindowCortex::InventoryAction) Couldn't get inventory items. Is the inventory empty?");
					cancel();
					break;
				}

				for (auto& i: items)
				{
					Memory item;
					if (!this->cortex->try_get_item(i, item))
					{
						continue;
					}

					bool match = false;
					auto names = item_cortex->get_names(item->get_string("key"));
					for (auto& name: names)
					{
						if (fuzzy_string_match(name, this->item))
						{
							match = true;
							break;
						}
					}

					if (match)
					{
						this->slots.push_back(i);
					}
				}

				if (this->slots.empty())
				{
					Log::warn("(BankWindowCortex::InventoryAction) No matching items found in inventory.");
					cancel();
					break;
				}

				this->state = state_next;
			}
			break;
		case state_next:
			{
				if (this->current_slot >= slots.size())
				{
					Log::warn("(BankWindowCortex::ItemAction) Item not found in inventory.");
					cancel();
					break;
				}

				auto& slot = slots[this->current_slot];
				++this->current_slot;

				MotionSequence sequence(*this->cortex);
				sequence.hover(slot, [this] { return this->cortex->is_visible(); });
				sequence.wait(
					Config::get<float>("gui", "BankWindowCortex", "EXAMINE_WAIT_MIN"),
					[this]
					{
						auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
						if (tool_tip == nullptr)
						{
							return false;
						}

						return tool_tip->is_visible() && tool_tip->get_text().size() >= 1;
					});

				this->current_result = this->cortex->perform(sequence);
				this->state = state_hover;
			}
			break;
		case state_hover:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->state = state_examine;
				}
				else
				{
					Log::warn("(BankWindowCortex::ExamineItem) Hovering item failed.");
					cancel();
				}
			}
			break;
		case state_examine:
			{
				auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
				if (tool_tip == nullptr)
				{
					Log::warn("(BankWindowCortex::ExamineItem) Tool tip cortex not valid. Bad logic!");
					cancel();
					break;
				}

				auto text = tool_tip->get_text();
				if (text.size() == 0)
				{
					Log::warn(
						"(BankWindowCortex::ExamineItem) Tool tip malformed."
						" Expected at least two values, got {}.",
						text.size());
					this->state = state_next;
				}

				// "Deposit-1" <space> <item-name>
				auto name = trim_string(text[0].substr(text[0].find(" ")));
				if (fuzzy_string_match(name, this->item, this->match_flags))
				{
					this->state = state_match;
				}
				else
				{
					this->state = state_next;
				}
			}
			break;
		case state_match:
			{
				auto result = on_match(this->slots[this->current_slot - 1]);
				if (result != Result::Status::working)
				{
					*this->status = result;
					this->state = state_done;
				}
			}
			break;
		case state_done:
			// Nothing.
			break;
		case state_error:
			if (this->cortex->is_visible())
			{
				this->state = this->previous_state;
			}
			else
			{
				auto& game_state = this->cortex->get_autonomaus().get_game_state();
				if (game_state.get_last_update_timestamp() > this->exception_time)
				{
					Log::warn("(BankWindowCortex::InventoryAction) Bank no longer visible.");
					cancel();
				}
			}
			break;
		default:
			Log::warn("(BankWindowCortex::InventoryAction) Unhandled state {}. Bad logic!", this->state);
			cancel();
			break;
	}
}

void autonomaus::BankWindowCortex::InventoryAction::cancel()
{
	if (*this->status == Result::Status::working)
	{
		*this->status = Result::Status::failure;
	}

	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}
}

autonomaus::BankWindowCortex::DepositAction::DepositAction(
	BankWindowCortex* cortex,
	const std::string& item,
	int match_flags,
	int quantity) :
		InventoryAction(cortex, item, match_flags)
{
	this->quantity = quantity;
}

autonomaus::BankWindowCortex::DepositAction::~DepositAction()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}
}

void autonomaus::BankWindowCortex::DepositAction::cancel()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	InventoryAction::cancel();
}

autonomaus::Cortex::Result::Status
autonomaus::BankWindowCortex::DepositAction::on_match(const chomsky::GUIContent& content)
{
	auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
	if (tool_tip == nullptr)
	{
		Log::warn("(BankWindowCortex::WithdrawAction) Tool tip cortex not valid. Bad logic!");
		return Result::Status::failure;
	}

	auto right_click_menu = this->cortex->get_autonomaus().get_userdata<RightClickMenuCortex>();
	if (right_click_menu == nullptr)
	{
		Log::warn("(BankWindowCortex::WithdrawAction) Right click menu cortex not valid. Bad logic!");
		return Result::Status::failure;
	}

	switch (this->state)
	{
		case state_start:
			{
				MotionSequence sequence(*this->cortex);

				if (this->quantity == 1)
				{
					this->state = state_wait;
					sequence.left_click();
				}
				else
				{
					this->state = state_right_click;
					sequence.right_click();
					sequence.wait<RightClickMenuCortex>(
						Config::get<float>("common", "gui", "RIGHT_CLICK_WAIT_TIME"));
				}

				this->current_result = this->cortex->perform(sequence);
			}
			break;
		case state_wait:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->state = state_success;
				}
				else
				{
					this->state = state_failure;
				}
			}
			break;
		case state_right_click:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->state = state_select;
				}
				else
				{
					this->state = state_failure;
				}
			}
			break;
		case state_select:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() != Result::Status::success)
				{
					this->state = state_failure;
					break;
				}

				int option_index = -1;
				for (int i = 0; i < right_click_menu->get_num_options(); ++i)
				{
					auto text = right_click_menu->get_option_text(i);
					if (text.size() == 0)
					{
						continue;
					}

					// "Deposit-*" <space> <item-name>
					auto option = trim_string(text[0].substr(0, text[0].find(" ")));

					bool match = false;
					if (this->quantity == transfer_all && option == "Deposit-All")
					{
						match = true;
					}
					else if (option == "Deposit")
					{
						// There's only a single item in the inventory.
						match = true;
					}
					else
					{
						std::string withdraw_x = "Deposit-";
						withdraw_x += std::to_string(this->quantity);

						if (option == withdraw_x)
						{
							match = true;
						}
					}

					if (match)
					{
						option_index = i;
						break;
					}
				}

				MotionSequence sequence(*this->cortex);
				if (option_index >= 0)
				{
					Message message;
					message.set_integer("option", option_index);

					sequence.poke<RightClickMenuCortex>(
						"select",
						message,
						[this] { return this->cortex->is_visible(); });
				}
				else if (this->quantity > 0)
				{
					Message message;
					message.set_string("option", "%Deposit-X.*");

					sequence.poke<RightClickMenuCortex>(
						"select",
						message,
						[this] { return this->cortex->is_visible(); });

					sequence.wait<DialogInputPromptCortex>(
						Config::get<float>("common", "gui", "WINDOW_WAIT_TIME"));

					sequence.type(
						std::to_string(quantity) + "\n",
						true,
						[this]
						{
							auto prompt = this->cortex->get_autonomaus().get_userdata<DialogInputPromptCortex>();

							return prompt != nullptr &&
								prompt->is_visible() &&
								this->cortex->is_visible();
						});
				}
				else
				{
					this->state = state_failure;
					break;
				}

				this->current_result = this->cortex->perform(sequence);
				this->state = state_wait;
			}
			break;
		case state_success:
			return Result::Status::success;
		default:
			Log::warn("(BankWindowCortex::DepositAction) Unhandled state {}. Bad logic!", this->state);
		case state_failure:
			return Result::Status::failure;
	}

	return Result::Status::working;
}

autonomaus::BankWindowCortex::PokeInventoryAction::PokeInventoryAction(
	BankWindowCortex* cortex,
	const std::string& item,
	int match_flags,
	const std::string& action) :
		InventoryAction(cortex, item, match_flags)
{
	this->action = action;
}

autonomaus::BankWindowCortex::PokeInventoryAction::~PokeInventoryAction()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}
}

void autonomaus::BankWindowCortex::PokeInventoryAction::cancel()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	InventoryAction::cancel();
}

autonomaus::Cortex::Result::Status
autonomaus::BankWindowCortex::PokeInventoryAction::on_match(const chomsky::GUIContent& content)
{
	switch (this->state)
	{
		case state_start:
			{
				MotionSequence sequence(*this->cortex);

				sequence.right_click();
				sequence.wait<RightClickMenuCortex>(
					Config::get<float>("common", "gui", "RIGHT_CLICK_WAIT_TIME"));

				this->current_result = this->cortex->perform(sequence);
				this->state = state_right_click;
			}
			break;
		case state_right_click:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->state = state_select;
				}
				else
				{
					return Result::Status::failure;
				}
			}
			break;
		case state_select:
			{
				auto right_click_menu = this->cortex->get_autonomaus().get_userdata<RightClickMenuCortex>();
				if (right_click_menu == nullptr)
				{
					Log::warn("(BankWindowCortex::WithdrawAction) Right click menu cortex not valid. Bad logic!");
					return Result::Status::failure;
				}

				this->current_result = right_click_menu->select(this->action);
				this->state = state_done;
			}
			break;
		case state_done:
			return this->current_result.get();
		default:
			Log::warn("(BankWindowCortex::PokeInventoryAction) Unhandled state {}. Bad logic!", this->state);
			return Result::Status::failure;
	}

	return Result::Status::working;
}
