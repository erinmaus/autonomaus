// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cctype>
#include "autonomaus/core/config.hpp"
#include "autonomaus/core/utility.hpp"
#include "autonomaus/input/gestures/typeGesture.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/memory/actionBarCortex.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"
#include "autonomaus/state/memory/rightClickMenuCortex.hpp"

autonomaus::ActionBarCortex::ActionBarCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::ActionBar", autonomaus, "*.main.action-bar.primary:action-bar"),
	stat_query("*.stats.?:action-bar-stat.value"),
	slots_query("*.slots[*]"),
	index_query("*.index-button:button.value"),
	minimize_button_query("*.minimize-button:button"),
	lock_button_query("*.lock-button:button"),
	auxiliary_action_bars_query("*.main.action-bar.auxiliary[*]:action-bar"),
	revolution_query("*.revolution-enabled"),
	keybind_query("*.slots[?].keybind")
{
	on_pull(
		"current_life_points",
		[this](auto&)
		{
			Message result;
			result.set_integer("result", this->get_current_life_points());
			return result;
		});

	on_pull(
		"maximum_life_points",
		[this](auto&)
		{
			Message result;
			result.set_integer("result", this->get_maximum_life_points());
			return result;
		});

	on_pull(
		"current_prayer_points",
		[this](auto&)
		{
			Message result;
			result.set_integer("result", this->get_current_prayer_points());
			return result;
		});

	on_pull(
		"maximum_prayer_points",
		[this](auto&)
		{
			Message result;
			result.set_integer("result", this->get_maximum_prayer_points());
			return result;
		});

	on_pull(
		"adrenaline",
		[this](auto&)
		{
			Message result;
			result.set_integer("result", this->get_adrenaline());
			return result;
		});

	on_pull(
		"current_summoning_points",
		[this](auto&)
		{
			Message result;
			result.set_integer("result", this->get_current_summoning_points());
			return result;
		});

	on_pull(
		"maximum_summoning_points",
		[this](auto&)
		{
			Message result;
			result.set_integer("result", this->get_maximum_summoning_points());
			return result;
		});

	on_poke(
		"switch_action_bar",
		[this](auto& message)
		{
			return this->switch_action_bar(message.get_integer("action_bar"), message.get_integer("index"));
		});

	on_pull(
		"action_bar_index",
		[this](auto& message)
		{
			Message result;
			result.set_integer("result", this->get_action_bar_index(message.get_integer("action_bar")));
			return result;
		});

	on_pull(
		"is_locked",
		[this](auto& message)
		{
			Message result;
			result.set_boolean("result", this->is_locked());
			return result;
		});

	on_poke(
		"lock",
		[this](auto&)
		{
			return this->lock();
		});

	on_poke(
		"unlock",
		[this](auto&)
		{
			return this->unlock();
		});

	on_pull(
		"is_unlocked",
		[this](auto& message)
		{
			Message result;
			result.set_boolean("result", this->is_unlocked());
			return result;
		});

	on_pull(
		"is_maximized",
		[this](auto& message)
		{
			Message result;
			result.set_boolean("result", this->is_maximized());
			return result;
		});

	on_pull(
		"is_minimized",
		[this](auto& message)
		{
			Message result;
			result.set_boolean("result", this->is_minimized());
			return result;
		});

	on_poke(
		"maximize",
		[this](auto&)
		{
			return this->maximize();
		});

	on_poke(
		"minimize",
		[this](auto&)
		{
			return this->minimize();
		});

	on_poke(
		"activate",
		[this](auto& message)
		{
			auto count = message.length("type");
			std::vector<std::pair<int, std::string>> actions;
			for (std::size_t i = 0; i < count; ++i)
			{
				actions.emplace_back(message.get_integer("type", i), message.get_string("value", i));
			}

			return this->activate(actions);
		});

	on_poke(
		"activate_slot",
		[this](auto& message)
		{
			auto count = message.length("action_bar");
			std::vector<std::pair<int, int>> slots;
			for (std::size_t i = 0; i < count; ++i)
			{
				slots.emplace_back(message.get_integer("action_bar", i), message.get_integer("slot", i));
			}

			return this->activate(slots);
		});

	on_pull(
		"find",
		[this](auto& message)
		{
			auto slots = this->get_slots(message.get_integer("type"), message.get_string("value"));

			Message result;
			for (std::size_t i = 0; i < slots.size(); ++i)
			{
				result.set_integer("action_bar", slots[i].first, i);
				result.set_integer("slot", slots[i].second, i);
			}
			return result;
		});

	on_pull(
		"slot",
		[this](auto& message)
		{
			int action_bar = message.get_integer("action_bar");
			int slot = message.get_integer("slot");

			Message result;
			result.set_integer("type", this->get_slot_type(action_bar, slot));
			result.set_boolean("enabled", this->get_slot_enabled(action_bar, slot));

			auto values = this->get_slot_values(action_bar, slot);
			for (std::size_t i = 0; i < values.size(); ++i)
			{
				result.set_string("value", values[i], i);
			}
			return result;
		});

	on_pull(
		"cooldown",
		[this](auto& message)
		{
			Message result;
			result.set_integer("result", this->get_cooldown(message.get_string("ability")));
			return result;
		});

	on_pull(
		"is_ready",
		[this](auto& message)
		{
			Message result;
			bool ready;
			if (message.has("cooldown"))
			{
				ready = this->is_ready(
					message.get_string("ability"),
					message.get_integer("cooldown"));
			}
			else
			{
				ready = this->is_ready(message.get_string("ability"));
			}
			result.set_boolean("result", ready);
			return result;
		});
}

int autonomaus::ActionBarCortex::get_current_life_points() const
{
	auto stats = get("stats");
	return stats->get_integer("current_life_points");
}

int autonomaus::ActionBarCortex::get_maximum_life_points() const
{
	auto stats = get("stats");
	return stats->get_integer("maximum_life_points");
}

int autonomaus::ActionBarCortex::get_current_prayer_points() const
{
	auto stats = get("stats");
	return stats->get_integer("current_prayer_points");
}

int autonomaus::ActionBarCortex::get_maximum_prayer_points() const
{
	auto stats = get("stats");
	return stats->get_integer("maximum_prayer_points");
}

int autonomaus::ActionBarCortex::get_adrenaline() const
{
	auto stats = get("stats");
	return stats->get_integer("adrenaline");
}

int autonomaus::ActionBarCortex::get_current_summoning_points() const
{
	auto stats = get("stats");
	return stats->get_integer("current_summoning_points");
}

int autonomaus::ActionBarCortex::get_maximum_summoning_points() const
{
	auto stats = get("stats");
	return stats->get_integer("maximum_summoning_points");
}

autonomaus::Cortex::Result autonomaus::ActionBarCortex::switch_action_bar(
	int action_bar, int index)
{
	if (get_action_bar_index(action_bar) == index)
	{
		return Cortex::Result::Status::success;
	}

	Log::info("(ActionBarCortex) Switching action bar {} to index {}...", action_bar, index);

	MotionSequence sequence(*this);
	if (action_bar == action_bar_primary)
	{
		chomsky::GUIContent primary;
		if (!try_get_root(primary))
		{
			Log::warn("(ActionBarCortex) Couldn't get action bar!");
			return Result::Status::failure;
		}

		if (!primary.has("index-button"))
		{
			Log::warn("(ActionBarCortex) Index button not visible. Is action bar minimized?");
			return Result::Status::failure;
		}

		sequence.right_click(primary.get("index-button"), [this] { return this->is_visible(); });
	}
	else
	{
		chomsky::GUIContent root;
		if (!try_get_global_root(root))
		{
			Log::warn("(ActionBarCortex) Couldn't get GUI root. Bad logic!");
			return Result::Status::failure;
		}

		std::vector<chomsky::GUIContent> auxiliary;
		if (!this->auxiliary_action_bars_query.collect(root, auxiliary) ||
			action_bar < 0 || action_bar > (int)auxiliary.size())
		{
			Log::warn("(ActionBarCortex) Action bar not visible.");
			return Result::Status::failure;
		}

		if (!auxiliary[action_bar - 1].has("index-button"))
		{
			Log::warn("(ActionBarCortex) Index button not visible. Bad logic!");
			return Result::Status::failure;
		}

		sequence.right_click(
			auxiliary[action_bar - 1].get("index-button"),
			[this] { return this->is_visible(); });
	}

	sequence.wait<RightClickMenuCortex>(
		Config::get<float>("common", "gui", "RIGHT_CLICK_WAIT_TIME"));

	Message message;
	message.set_string("option", std::to_string(index));
	sequence.poke<RightClickMenuCortex>("select", message);
	sequence.wait(
		Config::get<float>("common", "gui", "ACTION_WAIT_TIME"),
		[this, action_bar, index] { return this->get_action_bar_index(action_bar) == index; });

	return perform(sequence);
}

bool autonomaus::ActionBarCortex::is_locked() const
{
	return has_memory("locked");
}

bool autonomaus::ActionBarCortex::is_unlocked() const
{
	return !is_locked();
}

autonomaus::Cortex::Result autonomaus::ActionBarCortex::lock()
{
	if (is_locked())
	{
		return Result::Status::success;
	}
	Log::info("(ActionBarCortex) Locking action bar...");

	chomsky::GUIContent lock;
	if (!try_get_child(this->lock_button_query, lock))
	{
		Log::warn("ActionBarCortex) Lock button not visible. Is action bar minimized?");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.left_click(lock, [this] { return this->is_visible(); });
	sequence.wait(
		Config::get<float>("common", "gui", "ACTION_WAIT_TIME"),
		[this] { return this->is_locked(); });

	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::ActionBarCortex::unlock()
{
	if (is_unlocked())
	{
		return Result::Status::success;
	}
	Log::info("(ActionBarCortex) Unlocking action bar...");

	chomsky::GUIContent lock;
	if (!try_get_child(this->lock_button_query, lock))
	{
		Log::warn("ActionBarCortex) Lock button not visible. Is action bar minimized?");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.left_click(lock, [this] { return this->is_visible(); });
	sequence.wait(
		Config::get<float>("common", "gui", "ACTION_WAIT_TIME"),
		[this] { return this->is_unlocked(); });

	return perform(sequence);
}

bool autonomaus::ActionBarCortex::is_minimized() const
{
	return has_memory("minimized");
}

bool autonomaus::ActionBarCortex::is_maximized() const
{
	return !is_minimized();
}

autonomaus::Cortex::Result autonomaus::ActionBarCortex::minimize()
{
	if (is_minimized())
	{
		return Result::Status::success;
	}
	Log::info("(ActionBarCortex) Minimizing action bar...");

	chomsky::GUIContent minimize;
	if (!try_get_child(this->minimize_button_query, minimize))
	{
		Log::warn("ActionBarCortex) Minimize button not visible. Is action bar minimized?");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.left_click(minimize, [this] { return this->is_visible(); });
	sequence.wait(
		Config::get<float>("common", "gui", "ACTION_WAIT_TIME"),
		[this] { return this->is_minimized(); });

	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::ActionBarCortex::maximize()
{
	if (is_maximized())
	{
		return Result::Status::success;
	}
	Log::info("(ActionBarCortex) Maximizing action bar...");

	chomsky::GUIContent minimize;
	if (!try_get_child(this->minimize_button_query, minimize))
	{
		Log::warn("ActionBarCortex) Minimize button not visible. Is action bar minimized?");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.left_click(minimize, [this] { return this->is_visible(); });
	sequence.wait(
		Config::get<float>("common", "gui", "ACTION_WAIT_TIME"),
		[this] { return this->is_maximized(); });

	return perform(sequence);
}

int autonomaus::ActionBarCortex::get_action_bar_index(int action_bar)
{
	auto memory = get_action_bar(action_bar);
	return memory->get_integer("index");
}

int autonomaus::ActionBarCortex::get_slot_type(int action_bar, int slot) const
{
	auto memory = get_action_bar(action_bar);
	return memory->get_integer("type", slot);
}

bool autonomaus::ActionBarCortex::get_slot_enabled(int action_bar, int slot) const
{
	auto memory = get_action_bar(action_bar);
	return memory->get_integer("enabled", slot);
}

std::string autonomaus::ActionBarCortex::get_slot_value(int action_bar, int slot) const
{
	return get_slot_values(action_bar, slot)[0];
}

std::vector<std::string> autonomaus::ActionBarCortex::get_slot_values(int action_bar, int slot) const
{
	auto memory = get_action_bar(action_bar);

	std::vector<std::string> result;
	if (memory->get_integer("type", slot) == type_item)
	{
		auto value = memory->get_string("value", slot);
		auto item_cortex = get_autonomaus().get_userdata<ItemCortex>();
		if (item_cortex->has_name(value))
		{
			for (auto& name: item_cortex->get_names(value))
			{
				result.push_back(name);
			}
		}
	}
	result.push_back(memory->get_string("value", slot));

	return result;
}

autonomaus::Cortex::Result autonomaus::ActionBarCortex::activate(
	int action_bar, int slot)
{
	return activate({ std::make_pair(action_bar, slot) });
}

autonomaus::Cortex::Result autonomaus::ActionBarCortex::activate(
	const std::vector<std::pair<int, int>>& slots)
{
	Log::info("(ActionBarCortex) Activating {} slots by index...", slots.size());

	MotionSequence sequence(*this);
	std::string text;
	for (auto& i: slots)
	{
		auto action_bar = i.first;
		auto slot = i.second;

		Log::info("(ActionBarCortex) Activating action bar {} slot {}.", action_bar, slot);

		chomsky::GUIContent action_bar_content;
		if (action_bar == action_bar_primary)
		{
			try_get_root(action_bar_content);
		}
		else
		{
			chomsky::GUIContent root;
			if (try_get_global_root(root))
			{
				std::vector<chomsky::GUIContent> auxiliary;
				if (this->auxiliary_action_bars_query.collect(root, auxiliary))
				{
					if (action_bar > 0 && action_bar <= (int)auxiliary.size())
					{
						action_bar_content = auxiliary[action_bar - 1];
					}
				}
			}
		}

		if (action_bar_content.empty())
		{
			Log::warn("(ActionBarCortex) Couldn't get action bar.");
			return Result::Status::failure;
		}

		chomsky::GUIContent slot_content;
		{
			std::vector<chomsky::GUIContent> slots;
			if (!this->slots_query.collect(action_bar_content, slots) ||
				slots.empty())
			{
				Log::warn("(ActionBarCortex) Couldn't get action bar slots.");
				return Result::Status::failure;
			}

			if (slot < 0 || slot > (int)slots.size())
			{
				Log::warn("(ActionBarCortex) Slot index out-of-bounds.");
				return Result::Status::failure;
			}

			slot_content = slots[slot];
		}

		if (slot_content.has("keybind"))
		{
			auto keybind = slot_content.get("keybind").get_text().to_string();
			std::string t;
			if (try_parse_keybind(keybind, t))
			{
				text += t;

				continue;
			}

		}
		
		sequence.left_click(slot_content, [this] { return this->is_visible(); });
	}

	if (!text.empty())
	{
		sequence.type(text, true, [this] { return this->is_visible(); });
	}

	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::ActionBarCortex::activate(
	int type, const std::string& value)
{
	return activate({ std::make_pair(type, value) });
}

autonomaus::Cortex::Result autonomaus::ActionBarCortex::activate(
	const std::vector<std::pair<int, std::string>>& values)
{
	Log::info("(ActionBarCortex) Activating {} slots by value...", values.size());

	std::vector<std::pair<int, int>> slots;
	for (auto& i: values)
	{
		auto s = get_slot(i.first, i.second);
		if (s.first == action_bar_none)
		{
			Log::warn("(ActionBarCortex) Couldn't find '{}' (type: {}).", i.second, i.first);
			return Result::Status::failure;
		}

		slots.push_back(s);
	}

	return activate(slots);
}

std::pair<int, int> autonomaus::ActionBarCortex::get_slot(
	int type, const std::string& value) const
{
	auto result = get_slots(type, value);
	if (result.empty())
	{
		return std::make_pair(action_bar_none, 0);
	}

	return result[0];
}

int autonomaus::ActionBarCortex::get_cooldown(const std::string& ability)
{
	auto slot = get_slot(type_ability, ability);
	if (slot.first == action_bar_none)
	{
		return 0;
	}

	auto action_bar = get_action_bar(slot.first);
	return action_bar->get_integer("cooldown", slot.second);
}

bool autonomaus::ActionBarCortex::is_ready(const std::string& ability, int cooldown)
{
	auto slot = get_slot(type_ability, ability);
	if (slot.first == action_bar_none)
	{
		return 0;
	}

	auto action_bar = get_action_bar(slot.first);
	return action_bar->get_integer("cooldown", slot.second) <= cooldown &&
		action_bar->get_boolean("enabled", slot.second);
}

std::vector<std::pair<int, int>> autonomaus::ActionBarCortex::get_slots(
	int type,
	const std::string& value) const
{
	std::vector<std::pair<int, int>> result;

	auto item_cortex = get_autonomaus().get_userdata<ItemCortex>();

	auto action_bars = get_all("action_bar::(.*)");
	for (auto& i: action_bars)
	{
		auto& action_bar = i.second;

		std::size_t slots = action_bar->length("type");
		for (std::size_t j = 0; j < slots; ++j)
		{
			auto slot_type = action_bar->get_integer("type", j);
			auto slot_value = action_bar->get_string("value", j);

			if (slot_type != type)
			{
				continue;
			}

			if (slot_type == type_item && item_cortex != nullptr)
			{
				auto names = item_cortex->get_names(slot_value);
				for (auto& name: names)
				{
					if (fuzzy_string_match(name, value))
					{
						result.emplace_back(action_bar->get_integer("action_bar"), j);
					}
				}
			}
			else if (fuzzy_string_match(slot_value, value))
			{
				result.emplace_back(action_bar->get_integer("action_bar"), j);
			}
		}
	}

	return result;
}

bool autonomaus::ActionBarCortex::is_revolution_enabled() const
{
	return has_memory("revolution");
}

void autonomaus::ActionBarCortex::update()
{
	GUICortex::update();

	if (is_visible())
	{
		update_stats();

		chomsky::GUIContent primary;
		if (try_get_root(primary))
		{
			update_action_bar(action_bar_primary, primary);

			chomsky::GUIContent root;
			if (try_get_global_root(root))
			{
				std::vector<chomsky::GUIContent> auxiliary;
				if (this->auxiliary_action_bars_query.collect(root, auxiliary))
				{
					int index = action_bar_auxiliary_1;
					for (auto& i: auxiliary)
					{
						update_action_bar(index, i);
						++index;
					}
				}
			}

			chomsky::GUIContent lock;
			if (try_get_child(this->lock_button_query, lock) && lock.has("value"))
			{
				auto& value = lock.get("value");
				if (value.get_tags().has("locked"))
				{
					remember("locked", Memory());
				}
				else
				{
					forget("locked");
				}
			}

			chomsky::GUIContent minimize;
			if (try_get_child(this->minimize_button_query, minimize) && minimize.has("value"))
			{
				auto& value = minimize.get("value");
				if (value.get_tags().has("minimized"))
				{
					remember("minimized", Memory());
				}
				else
				{
					forget("minimized");
				}
			}

			chomsky::GUIContent revolution;
			if (try_get_child(this->revolution_query, revolution))
			{
				remember("revolution", Memory());
			}
			else if (is_maximized())
			{
				forget("revolution");
			}
		}
	}
}

autonomaus::Memory autonomaus::ActionBarCortex::get_action_bar(int action_bar) const
{
	auto key = std::string("action_bar::") + std::to_string(action_bar);
	return get(key);
}

void autonomaus::ActionBarCortex::update_action_bar(
	int action_bar,
	const chomsky::GUIContent& action_bar_content)
{
	auto key = std::string("action_bar::") + std::to_string(action_bar);
	auto memory = get(key);
	memory->set_integer("action_bar", action_bar);

	std::vector<chomsky::GUIContent> index_button;
	if (this->index_query.collect(action_bar_content, index_button) &&
		!index_button.empty())
	{
		auto value = index_button[0].get_text().to_string();
		memory->set_integer("index", std::atoi(value.c_str()));
	}

	std::vector<chomsky::GUIContent> slots;
	if (!this->slots_query.collect(action_bar_content, slots) || slots.empty())
	{
		return;
	}

	std::size_t index = 0;
	for (auto& slot: slots)
	{
		bool enabled = slot.has("enabled");
		memory->set_boolean("enabled", enabled, index);

		if (slot.has("cooldown"))
		{
			auto cooldown = std::atoi(slot.get("cooldown").get_text().to_string().c_str());
			memory->set_integer("cooldown", cooldown, index);
		}
		else
		{
			memory->set_integer("cooldown", 0, index);
		}

		if (slot.get("value").get_num_elements() != 1)
		{
			continue;
		}

		auto& element = slot.get("value").get_element(0);

		Memory item;
		if (try_get_item(element, item))
		{
			memory->set_integer("type", type_item, index);
			memory->set_string("value", item->get_string("key"), index);
		}
		else if (element.get_is_textured())
		{
			auto& texture_dictionary = get_autonomaus().get_game_state().get_resource_manager().get_texture_dictionary();
			auto texture_key = element.get_key();
			if (texture_dictionary.has(texture_key))
			{
				auto& entry = texture_dictionary.get(texture_key);

				if (entry.get_value() == "spell")
				{
					memory->set_integer("type", type_spell, index);
					memory->set_string("value", entry.get_data("spell"), index);
				}
				else if (entry.get_value() == "ability")
				{
					memory->set_integer("type", type_ability, index);
					memory->set_string("value", entry.get_data("ability"), index);
				}
				else if (entry.get_value() == "prayer")
				{
					memory->set_integer("type", type_prayer, index);
					memory->set_string("value", entry.get_data("prayer"), index);
				}
				else if (entry.get_value() == "action-bar-empty-slot-background")
				{
					memory->set_integer("type", type_none, index);
					memory->set_string("value", std::string(), index);
				}
				else
				{
					memory->set_integer("type", type_unknown, index);
					memory->set_string("value", entry.get_value(), index);
				}
			}
		}

		++index;
	}

	remember(key, memory);
}

void autonomaus::ActionBarCortex::update_stats()
{
	auto stats = get("stats");
	int current_life_points = stats->get_integer("current_life_points");
	int maximum_life_points = stats->get_integer("maximum_life_points");
	{
		std::string value;
		if (try_get_stat("life-points", value))
		{
			auto values = split_string(value, "/");
			if (values.size() == 2)
			{
				current_life_points = std::atoi(values[0].c_str());
				maximum_life_points = std::atoi(values[1].c_str());
			}
		}
	}

	int current_prayer_points = stats->get_integer("current_prayer_points");
	int maximum_prayer_points = stats->get_integer("maximum_prayer_points");
	{
		std::string value;
		if (try_get_stat("prayer-points", value))
		{
			auto values = split_string(value, "/");
			if (values.size() == 2)
			{
				current_prayer_points = std::atoi(values[0].c_str());
				maximum_prayer_points = std::atoi(values[1].c_str());
			}
		}
	}

	int adrenaline = stats->get_integer("adrenaline");
	{
		std::string value;
		if (try_get_stat("adrenaline", value))
		{
			adrenaline = std::atoi(value.c_str());
		}
	}

	int current_summoning_points = stats->get_integer("current_summoning_points");
	int maximum_summoning_points = stats->get_integer("maximum_summoning_points");
	{
		std::string value;
		if (try_get_stat("summoning", value))
		{
			auto values = split_string(value, "/");
			if (values.size() == 2)
			{
				current_summoning_points = std::atoi(values[0].c_str());
				maximum_summoning_points = std::atoi(values[1].c_str());
			}
		}
	}

	stats->set_integer("current_life_points", current_life_points);
	stats->set_integer("maximum_life_points", maximum_life_points);
	stats->set_integer("current_prayer_points", current_prayer_points);
	stats->set_integer("maximum_prayer_points", maximum_prayer_points);
	stats->set_integer("adrenaline", adrenaline);
	stats->set_integer("current_summoning_points", current_summoning_points);
	stats->set_integer("maximum_summoning_points", maximum_summoning_points);

	remember("stats", stats);
}

bool autonomaus::ActionBarCortex::try_get_stat(const std::string& stat, std::string& result)
{
	auto query = stat_query;
	query.set_input(0, stat);

	chomsky::GUIContent content;
	if (try_get_child(query, content))
	{
		result = content.get_text().to_string();
		return true;
	}

	return false;
}

#include <boost/regex.hpp>

bool autonomaus::ActionBarCortex::try_parse_keybind(
	const std::string& keybind,
	std::string& result)
{
	boost::regex e("(Shift|Ctrl|Alt)?\\s*(.*)");
	boost::smatch m;
	if (boost::regex_match(keybind, m, e))
	{
		std::string modifier = m[1];
		std::string key = m[2];

		int key_modifier = eiznebahn::KeySequence::key_code_max;
		if (modifier == "Shift")
		{
			key_modifier = eiznebahn::KeySequence::key_code_left_shift;
		}
		else if (modifier == "Ctrl")
		{
			key_modifier = eiznebahn::KeySequence::key_code_left_ctrl;
		}
		else if (modifier == "Alt")
		{
			key_modifier = eiznebahn::KeySequence::key_code_left_alt;
		}

		if (key.size() != 1)
		{
			return false;
		}

		result = TypeGesture::escape_sequence(key_modifier, key[0]);
		return true;
	}

	return false;
}
