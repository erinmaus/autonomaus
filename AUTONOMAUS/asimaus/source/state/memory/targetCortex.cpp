// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/config.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/core/utility.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/memory/targetCortex.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"

autonomaus::TargetCortex::TargetCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::Target", autonomaus, "*.main.target:target"),
	current_health_query("*.health.value"),
	buffs_query("*.status[*].buff"),
	debuffs_query("*.status[*].debuff")
{
	on_pull(
		"current_health",
		[this](auto&)
		{
			Message result;
			result.set_integer("result", this->get_current_health());
			return result;
		});

	on_pull(
		"buffs",
		[this](auto&)
		{
			auto buffs = this->get_buffs();

			Message result;
			for (std::size_t i = 0; i < buffs.size(); ++i)
			{
				result.set_string("result", buffs[i], i);
			}

			return result;
		});

	on_pull(
		"debuffs",
		[this](auto&)
		{
			auto debuffs = this->get_debuffs();

			Message result;
			for (std::size_t i = 0; i < debuffs.size(); ++i)
			{
				result.set_string("result", debuffs[i], i);
			}

			return result;
		});

	on_pull(
		"has_buff",
		[this](auto& message)
		{
			Message result;
			result.set_boolean("result", this->has_buff(message.get_string("buff")));
			return result;
		});

	on_pull(
		"has_debuff",
		[this](auto& message)
		{
			Message result;
			result.set_boolean("result", this->has_debuff(message.get_string("debuff")));
			return result;
		});
}

int autonomaus::TargetCortex::get_current_health() const
{
	chomsky::GUIContent health;
	if (try_get_child(this->current_health_query, health))
	{
		return std::atoi(health.get_text().to_string().c_str());
	}

	return 0;
}

std::vector<std::string> autonomaus::TargetCortex::get_buffs() const
{
	std::vector<std::string> result;

	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		return result;
	}

	std::vector<chomsky::GUIContent> buffs;
	if (!this->buffs_query.collect(root, buffs))
	{
		return result;
	}

	auto items = get_autonomaus().get_userdata<ItemCortex>();

	for (auto& i: buffs)
	{
		std::string name;
		if (try_get_name(i, name))
		{
			if (name == "item" && items != nullptr)
			{
				Memory item;
				if (try_get_item(i, item))
				{
					auto names = items->get_names(item->get_string("key"));
					for (auto& name: names)
					{
						result.push_back(name);
					}
				}
			}
			else if (name == "prayer")
			{
				try_get_data(i, "prayer", name);
				result.push_back(name);
			}
			else if (name == "ability")
			{
				try_get_data(i, "ability", name);
				result.push_back(name);
			}
			else
			{
				result.push_back(name);
			}
		}
	}

	return result;
}

std::vector<std::string> autonomaus::TargetCortex::get_debuffs() const
{
	std::vector<std::string> result;

	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		return result;
	}

	std::vector<chomsky::GUIContent> debuffs;
	if (!this->debuffs_query.collect(root, debuffs))
	{
		return result;
	}

	auto items = get_autonomaus().get_userdata<ItemCortex>();

	for (auto& i: debuffs)
	{
		std::string name;
		if (try_get_name(i, name))
		{
			if (name == "item" && items != nullptr)
			{
				Memory item;
				if (try_get_item(i, item))
				{
					auto names = items->get_names(item->get_string("key"));
					for (auto& name: names)
					{
						result.push_back(name);
					}
				}
			}
			else if (name == "prayer")
			{
				try_get_data(i, "prayer", name);
				result.push_back(name);
			}
			else if (name == "ability")
			{
				try_get_data(i, "ability", name);
				result.push_back(name);
			}
			else
			{
				result.push_back(name);
			}
		}
	}

	return result;
}

bool autonomaus::TargetCortex::has_buff(const std::string& value) const
{
	auto buffs = get_buffs();
	for (auto& i: buffs)
	{
		if (fuzzy_string_match(i, value))
		{
			return true;
		}
	}

	return false;
}

bool autonomaus::TargetCortex::has_debuff(const std::string& value) const
{
	auto debuffs = get_debuffs();
	for (auto& i: debuffs)
	{
		if (fuzzy_string_match(i, value))
		{
			return true;
		}
	}

	return false;
}
