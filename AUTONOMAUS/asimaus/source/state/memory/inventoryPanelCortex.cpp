// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cctype>
#include <cstring>
#include <cstdlib>
#include "autonomaus/core/config.hpp"
#include "autonomaus/core/rng.hpp"
#include "autonomaus/core/time.hpp"
#include "autonomaus/core/utility.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/memory/inventoryPanelCortex.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"
#include "autonomaus/state/memory/ribbonCortex.hpp"
#include "autonomaus/state/memory/toolTipCortex.hpp"

const int autonomaus::InventoryPanelCortex::NUM_INVENTORY_SLOTS =
	Config::get<int>("common", "gameplay", "NUM_INVENTORY_SLOTS");

struct autonomaus::InventoryPanelCortex::ExamineAction : public Action
{
	ExamineAction(
		InventoryPanelCortex* cortex,
		bool everything);
	~ExamineAction();

	Cortex::Result get_result() override;
	void update() override;

	void cancel();

	InventoryPanelCortex* cortex;
	bool everything;

	enum
	{
		state_start,
		state_next,
		state_move,
		state_wait,
		state_examine,
		state_error,
		state_done
	};
	int state = state_start;
	int previous_state = state_next;

	Cortex::Result current_result;
	int current_slot = -1;
	chomsky::GUIContent current_content;
	std::string current_key;
	std::string current_runtime_key;
	float target_time;

	float error_recovery_time;

	Cortex::Result::StatusReference status;
	Cortex::Result final_result;
};

autonomaus::InventoryPanelCortex::InventoryPanelCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::InventoryPanel", autonomaus, "*.main.panels[*].content:inventory-panel-content"),
	slots_query("*.slots[*]"),
	items_query("*.items[*]"),
	coins_query("*.coins")
{
	on_poke(
		"open",
		[this](auto)
		{
			return this->open();
		});

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

	auto get_flags = [](Message message, int& flags)
	{
		bool match = false;

		if (message.get_boolean("noted"))
		{
			flags |= match_noted;
			match = true;
		}

		if (message.get_boolean("unnoted"))
		{
			flags |= match_unnoted;
			match = true;
		}

		if (message.get_boolean("variants"))
		{
			flags |= match_variants;
			match = true;
		}

		return match;
	};

	on_pull(
		"has_item",
		[this, get_flags](auto& message)
		{
			Message result;

			std::string item = message.get_string("item");
			int quantity = message.has("quantity") ? message.get_integer("quantity") : 1;
			int flags = 0;
			if (get_flags(message, flags))
			{
				result.set_boolean("result", this->has_item(item, quantity, flags));
			}
			else
			{
				result.set_boolean("result", this->has_item(item, quantity));
			}

			return result;
		});

	on_pull(
		"count_item",
		[this, get_flags](auto& message)
		{
			Message result;

			std::string item = message.get_string("item");
			int flags = 0;
			if (get_flags(message, flags))
			{
				result.set_integer("result", this->count_item(item, flags));
			}
			else
			{
				result.set_integer("result", this->count_item(item));
			}

			return result;
		});

	on_pull(
		"item_slots",
		[this, get_flags](auto& message)
		{
			std::vector<int> slots;
			std::string item = message.get_string("item");
			int flags = 0;
			if (get_flags(message, flags))
			{
				slots = this->get_item_slots(item, flags);
			}
			else
			{
				slots = this->get_item_slots(item);
			}

			Message result;
			for (std::size_t i = 0; i < slots.size(); ++i)
			{
				result.set_integer("result", slots[i], i);
			}

			return result;
		});

	on_poke(
		"poke_item",
		[this, get_flags](auto& message)
		{
			std::string item = message.get_string("item");
			std::string action = message.get_string("action");
			int flags = 0;
			if (get_flags(message, flags))
			{
				return this->poke_item(item, action, flags);
			}
			else
			{
				return this->poke_item(item, action);
			}
		});

	on_poke(
		"poke_slot",
		[this, get_flags](auto& message)
		{
			int slot = message.get_integer("slot");
			std::string action = message.get_string("action");
			return this->poke_slot(slot, action);
		});

	on_poke(
		"examine_inventory",
		[this](auto& message)
		{
			if (message.has("everything"))
			{
				return this->examine_inventory(message.get_boolean("everything"));
			}
			else
			{
				return this->examine_inventory();
			}
		});

	on_pull(
		"coins",
		[this](auto)
		{
			Message result;
			result.set_integer("result", this->get_coins());
			return result;
		});
}

autonomaus::Cortex::Result autonomaus::InventoryPanelCortex::open()
{
	auto ribbon = get_autonomaus().get_userdata<RibbonCortex>();
	if (ribbon == nullptr)
	{
		Log::warn("(InventoryPanelCortex) Ribbon cortex not valid. Bad logic!");
		return Result::Status::failure;
	}

	return ribbon->open("hero", "inventory");
}

int autonomaus::InventoryPanelCortex::get_free_space() const
{
	return NUM_INVENTORY_SLOTS - get_used_space();
}

int autonomaus::InventoryPanelCortex::get_used_space() const
{
	return count("item::(.*)");
}

bool autonomaus::InventoryPanelCortex::has_item(
	const std::string& name, int quantity, int flags) const
{
	return count_item(name, flags) >= quantity;
}

int autonomaus::InventoryPanelCortex::count_item(
	const std::string& name, int flags) const
{
	auto items = get_autonomaus().get_userdata<ItemCortex>();
	if (items == nullptr)
	{
		return false;
	}

	int quantity = 0;
	auto inventory = get_all("item::(.*)");
	for (auto& i: inventory)
	{
		auto& item = i.second;

		auto key = item->get_string("key");
		if (!items->has_name(key))
		{
			continue;
		}

		if (items->is_ambiguous(key) && !(flags & match_variants))
		{
			continue;
		}

		bool match = false;
		auto names = items->get_names(key);
		for (auto& i: names)
		{
			if (fuzzy_string_match(i, name))
			{
				match = true;
				break;
			}
		}

		if (!match)
		{
			continue;
		}

		if (item->get_boolean("noted") && !(flags & match_noted))
		{
			continue;
		}

		quantity += item->get_integer("quantity");
	}

	return quantity;
}

std::vector<int> autonomaus::InventoryPanelCortex::get_item_slots(
	const std::string& name, int flags) const
{
	auto items = get_autonomaus().get_userdata<ItemCortex>();
	if (items == nullptr)
	{
		return {};
	}

	std::vector<int> slots;
	auto inventory = get_all("item::(.*)");
	for (auto& i: inventory)
	{
		auto& item = i.second;

		auto key = item->get_string("key");
		if (!items->has_name(key))
		{
			continue;
		}

		if (items->is_ambiguous(key) && !(flags & match_variants))
		{
			continue;
		}

		if (items->is_ambiguous(key))
		{
			bool match = false;
			auto names = items->get_names(key);
			for (auto& i: names)
			{
				if (fuzzy_string_match(i, name))
				{
					match = true;
					break;
				}
			}

			if (!match)
			{
				continue;
			}
		}
		else if (!fuzzy_string_match(items->get_name(key), name))
		{
			continue;
		}

		if (item->get_boolean("noted") && !(flags & match_noted))
		{
			continue;
		}

		slots.push_back(item->get_integer("slot"));
	}

	return slots;
}

autonomaus::Cortex::Result autonomaus::InventoryPanelCortex::poke_item(
	const std::string& name,
	const std::string& action,
	int flags)
{
	Log::info("(InventoryPanelCortex) Performing poke '{}' on item '{}'...", action, name);

	int slot;
	{
		auto slots = get_item_slots(name, flags);
		if (slots.empty())
		{
			Log::warn("(InventoryPanelCortex) Desired item not in inventory.");
			return Result::Status::failure;
		}

		slot = slots[0];
	}

	return poke_slot(slot, action);
}

autonomaus::Cortex::Result autonomaus::InventoryPanelCortex::poke_slot(
	int slot,
	const std::string& action)
{
	Log::info("(InventoryPanelCortex) Performing poke '{}' on slot #{}...", action, slot);

	if (!is_ready() || !is_visible())
	{
		Log::warn("(InventoryPanelCortex) Inventory panel not open or not fully visible.");
		return Result::Status::failure;
	}

	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		Log::warn("(InventoryPanelCortex) Couldn't get inventory root content. Bad logic!");
		return Result::Status::failure;
	}

	std::vector<chomsky::GUIContent> slots;
	if (!this->slots_query.collect(root, slots) || slots.empty())
	{
		Log::warn("(InventoryPanelCortex) Couldn't get inventory slot contents."
			" Is the legacy interface skin active?");
		return Result::Status::failure;
	}

	if (slot >= (int)slots.size() || slot < 0)
	{
		Log::warn("(InventoryPanelCortex) Slot index ({}) out of bounds.", slot);
		return Result::Status::failure;
	}

	auto& content = slots[slot];
	return poke(content, action, false, [this] { return this->is_visible() && this->is_ready(); });
}

autonomaus::Cortex::Result autonomaus::InventoryPanelCortex::examine_inventory(bool everything)
{
	Log::info("(InventoryPanelCortex) Examining inventory...");
	return perform(new ExamineAction(this, everything));
}

int autonomaus::InventoryPanelCortex::get_coins() const
{
	if (has_memory("coins"))
	{
		auto memory = get("coins");
		return memory->get_integer("value");
	}

	return 0;
}

void autonomaus::InventoryPanelCortex::mark_dirty()
{
	this->dirty = true;
}

bool autonomaus::InventoryPanelCortex::is_dirty() const
{
	return this->dirty;
}

bool autonomaus::InventoryPanelCortex::is_ready() const
{
	return GUICortex::is_ready() && !this->obscured;
}

void autonomaus::InventoryPanelCortex::update()
{
	GUICortex::update();

	if (is_visible())
	{
		update_items();
		update_coins();
	}
}

void autonomaus::InventoryPanelCortex::update_items()
{
	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		return;
	}

	std::vector<chomsky::GUIContent> slots;
	if (!this->slots_query.collect(root, slots) || slots.empty())
	{
		return;
	}

	if ((int)slots.size() != NUM_INVENTORY_SLOTS)
	{
		this->obscured = true;
		return;
	}
	else
	{
		this->obscured = false;
	}

	std::vector<chomsky::GUIContent> items;
	if (!this->items_query.collect(root, items) || items.empty())
	{
		auto memories = get_all("item::(.*)");
		for (auto& i: memories)
		{
			forget(i.first);
		}
		return;
	}

	std::size_t current_slot = 0;
	std::set<int> visited_slots;
	for (auto& i: items)
	{
		Memory item;
		if (!try_get_item(i, item))
		{
			continue;
		}

		std::size_t slot_index = 0;
		for (std::size_t j = current_slot; j < slots.size(); ++j)
		{
			auto& slot = slots[j];

			int center_x = i.get_x() + i.get_width() / 2;
			int center_y = i.get_y() + i.get_height() / 2;

			if (center_x > slot.get_x() &&
				center_x < slot.get_x() + slot.get_width() &&
				center_y > slot.get_y() &&
				center_y < slot.get_y() + slot.get_height())
			{
				slot_index = j;
				item->set_integer("slot", j);
			}
		}

		if (!item->has("slot"))
		{
			continue;
		}

		visited_slots.insert(slot_index);
		current_slot = slot_index;

		std::string key = "item::";
		key += item->get_string("slot");
		remember(key, item);
	}

	for (int i = 0; i < NUM_INVENTORY_SLOTS; ++i)
	{
		if (visited_slots.count(i) == 0)
		{
			std::string key = "item::";
			key += std::to_string(i);

			forget(key);
		}
	}
}

void autonomaus::InventoryPanelCortex::update_coins()
{
	chomsky::GUIContent coins;
	std::string text;
	if (!try_get_child(this->coins_query, coins) || !try_get_text(coins, text))
	{
		return;
	}

	const char* start = text.c_str();
	const char* end = nullptr;
	int number = std::strtol(start, (char**)(&end), 10);

	int multiplier;
	switch (std::tolower(*end))
	{
		case 'k':
			multiplier = 1000;
			break;
		case 'm':
			multiplier = 1000000;
			break;
		default:
			multiplier = 1;
			break;
	};

	Memory memory;
	memory->set_integer("value", multiplier * number);
	remember("coins", memory);
}

autonomaus::InventoryPanelCortex::ExamineAction::ExamineAction(
	InventoryPanelCortex* cortex,
	bool everything) :
		status(std::make_shared<Cortex::Result::Status>(Cortex::Result::Status::working)),
		final_result(status, [this] { this->cancel(); })
{
	this->cortex = cortex;
	this->everything = everything;
}

autonomaus::InventoryPanelCortex::ExamineAction::~ExamineAction()
{
	cancel();
}

autonomaus::Cortex::Result autonomaus::InventoryPanelCortex::ExamineAction::get_result()
{
	return this->final_result;
}

void autonomaus::InventoryPanelCortex::ExamineAction::update()
{
	if (!this->cortex->is_ready() || !this->cortex->is_visible())
	{
		if (this->state != state_error && this->state != state_done)
		{
			this->previous_state = this->state;
			this->state = state_error;

			const float wait_time = Config::get<float>("common", "gui", "GUI_GUARD_WAIT_TIME");
			this->error_recovery_time = get_current_time() + wait_time;
		}
	}

	switch (this->state)
	{
		case state_start:
			{
				auto items = cortex->get_autonomaus().get_userdata<ItemCortex>();
				if (items == nullptr)
				{
					Log::warn("(InventoryPanelCortex::ExamineAction) Item cortex not valid. Bad logic!");
					cancel();
				}
				else
				{
					auto slots = cortex->get_all("item::(.*)");
					for (auto& i: slots)
					{
						auto& item = i.second;
						auto key = item->get_string("key");
						auto runtime_key = item->get_string("runtime-key");
						auto slot = item->get_integer("slot");

						if ((this->everything && slot > this->current_slot) ||
							!items->has_name(key))
						{
							this->current_slot = slot;
							this->current_key = key;
							this->current_runtime_key = runtime_key;
							this->state = state_next;
							break;
						}
					}

					if (this->state == state_start)
					{
						Log::info("(InventoryPanelCortex::ExamineAction): Done examining inventory.");
						*this->status = Result::Status::success;
						this->state = state_done;
					}
				}
			}
			break;
		case state_next:
			{
				auto items = this->cortex->get_autonomaus().get_userdata<ItemCortex>();
				if (items == nullptr)
				{
					Log::warn("(InventoryPanelCortex::ExamineAction) Item cortex not valid. Bad logic!");
					cancel();
				}
				else
				{
					chomsky::GUIContent root;
					if (!this->cortex->try_get_root(root))
					{
						Log::warn("(InventoryPanelCortex::ExamineAction) Could not get root inventory content. Bad logic!");
						cancel();
						break;
					}

					std::vector<chomsky::GUIContent> slots;
					if (!this->cortex->slots_query.collect(root, slots) || slots.empty())
					{
						Log::warn("(InventoryPanelCortex::ExamineAction) Couldn't get inventory slot contents."
							" Is the legacy interface skin active?");
						cancel();
						break;
					}

					if (this->current_slot > (int)slots.size())
					{
						Log::warn("(InventoryPanelCortex::ExamineAction) Slot index out of bounds.");
						cancel();
						break;
					}

					this->current_content = slots[this->current_slot];

					MotionSequence sequence(*this->cortex);
					sequence.hover(
						current_content,
						[this]
						{
							return this->cortex->is_ready() && this->cortex->is_visible();
						});

					this->current_result = this->cortex->perform(sequence);
					this->state = state_move;
				}
			}
			break;
		case state_move:
			{
				if (this->current_result.get() == Result::Status::failure)
				{
					Log::warn("(InventoryPanelCortex::ExamineAction) Hover motion failed!");
					cancel();
				}
				else if (this->current_result.get() == Result::Status::success)
				{
					static const float min_wait = Config::get<float>("gui", "InventoryCortex", "EXAMINE_WAIT_MIN");
					static const float max_wait = Config::get<float>("gui", "InventoryCortex", "EXAMINE_WAIT_MAX");
					float wait = RNG::nextf(min_wait, max_wait);
					this->current_result = Result();
					this->target_time = get_current_time() + wait;
					this->state = state_wait;
				}
			}
			break;
		case state_wait:
			{
				auto& game_state = this->cortex->get_autonomaus().get_game_state();
				if (game_state.get_last_update_timestamp() > this->target_time)
				{
					this->state = state_examine;
				}
			}
			break;
		case state_examine:
			{
				auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
				auto items = this->cortex->get_autonomaus().get_userdata<ItemCortex>();
				if (tool_tip == nullptr)
				{
					Log::warn("(InventoryPanelCortex::ExamineAction) Tool tip cortex not valid. Bad logic!");
					cancel();
				}
				else if (items == nullptr)
				{
					Log::warn("(InventoryPanelCortex::ExamineAction) Item cortex not valid. Bad logic!");
					cancel();
				}
				else
				{
					auto text = tool_tip->get_text();

					// It goes something like this:
					//
					// <color> <action-text> <color> <item-name> <etc>
					//
					// So the item name is the second text value.
					//
					// This is only necessarily true for the main inventory panel.
					// The bank inventory panel, for example, has a single text
					// value with the item name:
					//
					// <color> <deposit-action> <item-name> <etc>
					if (text.size() < 2)
					{
						Log::warn(
							"(InventoryPanelCortex::ExamineAction) Tool tip malformed."
							" Expected at least two values, got {}.",
							text.size());
						cancel();
						break;
					}

					auto name = trim_string(text[1]);

					Log::info("(InventoryPanelCortex::ExamineAction) Got item '{0}'.", name);
					items->remember_item(this->current_runtime_key);
					items->add_name(this->current_key, name);
					this->state = state_start;
				}
			}
			break;
		case state_error:
			if (this->cortex->is_ready() && this->cortex->is_visible())
			{
				this->state = this->previous_state;
			}
			else
			{
				auto& game_state = this->cortex->get_autonomaus().get_game_state();
				if (game_state.get_last_update_timestamp() > this->error_recovery_time)
				{
					Log::warn("(InventoryPanelCortex::ExamineAction) Inventory no longer visible.");
					cancel();
				}
			}
			break;
		case state_done:
			// Nothing.
			break;
		default:
			Log::warn("(InventoryPanelCortex::ExamineAction) Bad state! Value: '{}'.", this->state);
			cancel();
			break;
	}
}

void autonomaus::InventoryPanelCortex::ExamineAction::cancel()
{
	this->current_result.cancel();
	this->final_result.terminate();

	if (*this->status == Result::Status::working)
	{
		*this->status = Result::Status::failure;
	}

	this->state = state_done;
}
