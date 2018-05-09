// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <vector>
#include "autonomaus/core/config.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/state/memory/ribbonCortex.hpp"
#include "autonomaus/state/memory/motionSequence.hpp"

autonomaus::RibbonCortex::RibbonCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::Ribbon", autonomaus, "*.main.ribbon:ribbon"),
	ribbon_buttons_query("*.buttons[*]:button.icon"),
	ribbon_lock_button_query("*.lock-button:button.value"),
	ribbon_pop_up_query("*.main.pop-ups[*]:ribbon-pop-up"),
	panel_tabs_query("*.main.panels[*]:panel.tabs[*]")
{
	on_poke(
		"lock",
		[this] (auto)
		{
			return this->lock();
		});

	on_poke(
		"unlock",
		[this] (auto)
		{
			return this->unlock();
		});

	on_pull(
		"is_locked",
		[this] (auto)
		{
			Message result;
			result.set_boolean("result", this->is_locked());
			return result;
		});

	on_poke(
		"open",
		[this] (auto& message)
		{
			auto category = message.get_string("category");
			if (message.has("tab"))
			{
				return this->open(category, message.get_string("tab"));
			}
			else
			{
				return this->open(category);
			}
		});

	on_pull(
		"is_open",
		[this] (auto& message)
		{
			Message result;
			result.set_boolean("result", this->is_open(message.get_string("tab")));
			return result;
		});

	on_pull(
		"is_active",
		[this] (auto& message)
		{
			Message result;
			result.set_boolean("result", this->is_active(message.get_string("tab")));
			return result;
		});
}

autonomaus::Cortex::Result autonomaus::RibbonCortex::lock()
{
	Log::info("(RibbonCortex) Locking interface...");

	if (is_locked())
	{
		Log::info("(RibbonCortex) Interface already locked.");
		return Result::Status::success;
	}

	chomsky::GUIContent button;
	if (!try_get_child(this->ribbon_lock_button_query, button))
	{
		Log::info("(RibbonCortex) Couldn't get lock button. Bad logic!");
	}

	MotionSequence sequence(*this);
	sequence.left_click(button);

	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::RibbonCortex::unlock()
{
	Log::info("(RibbonCortex) Unlocking interface...");

	if (!is_locked())
	{
		Log::info("(RibbonCortex) Interface already unlocked.");
		return Result::Status::success;
	}

	chomsky::GUIContent button;
	if (!try_get_child(this->ribbon_lock_button_query, button))
	{
		Log::info("(RibbonCortex) Couldn't get lock button. Bad logic!");
	}

	MotionSequence sequence(*this);
	sequence.left_click(button);

	return perform(sequence);
}

bool autonomaus::RibbonCortex::is_locked() const
{
	chomsky::GUIContent button;
	if (!try_get_child(this->ribbon_lock_button_query, button))
	{
		return false;
	}

	return button.get_tags().has("locked");
}

autonomaus::Cortex::Result autonomaus::RibbonCortex::open(const std::string& category)
{
	Log::info("(RibbonCortex) Opening window {}...", category);

	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		Log::info("(RibbonCortex) Ribbon isn't visible.");

		return Result::Status::failure;
	}

	std::vector<chomsky::GUIContent> buttons;
	if (!this->ribbon_buttons_query.collect(root, buttons))
	{
		Log::info("(RibbonCortex) No buttons on ribbon.");

		return Result::Status::failure;
	}

	for (auto& i: buttons)
	{
		std::string name;
		if (!try_get_name(i, name))
		{
			continue;
		}

		if (name.substr(0, category.length()) == category)
		{
			MotionSequence sequence(*this);
			sequence.left_click(i);

			return perform(sequence);
		}
	}

	Log::info("(RibbonCortex) Button not found.");
	return Result::Status::failure;
}

autonomaus::Cortex::Result autonomaus::RibbonCortex::open(
	const std::string& category,
	const std::string& tab)
{
	// XXX: This will fail if the wrong category is currently hovered.
	chomsky::GUIContent root;
	if (is_open(tab))
	{
		if (is_active(tab))
		{
			return Result::Status::success;
		}
		else
		{
			if (!try_get_global_root(root))
			{
				Log::info("(RibbonCortex) Could not get global root! Bad logic.");
				return Result::Status::failure;
			}

			std::vector<chomsky::GUIContent> tabs;
			if (!this->panel_tabs_query.collect(root, tabs))
			{
				Log::info("(RibbonCortex) No panel tabs. Bad logic!");

				return Result::Status::failure;
			}

			for (auto& i: tabs)
			{
				if (!i.has("icon"))
				{
					continue;
				}
				auto icon = i.get("icon");

				std::string name;
				if (!try_get_name(icon, name))
				{
					continue;
				}

				if (name.substr(0, tab.length()) == tab)
				{
					MotionSequence sequence(*this);
					sequence.left_click(i, [this, tab]
					{
						return this->is_open(tab);
					});

					return perform(sequence);
				}
			}

			Log::info("(RibbonCortex) Tab not found. Bad logic!");
			return Result::Status::failure;
		}
	}
	else if (try_get_root(this->ribbon_pop_up_query, root))
	{
		std::vector<chomsky::GUIContent> buttons;
		if (!this->ribbon_buttons_query.collect(root, buttons))
		{
			Log::info("(RibbonCortex) No buttons on ribbon pop-up.");

			return Result::Status::failure;
		}

		for (auto& i: buttons)
		{
			std::string name;
			if (!try_get_name(i, name))
			{
				continue;
			}

			if (name.substr(0, tab.length()) == tab)
			{
				MotionSequence sequence(*this);
				sequence.left_click(i, [this]
				{
					chomsky::GUIContent pop_up;
					return try_get_root(this->ribbon_pop_up_query, pop_up);
				});

				return perform(sequence);
			}
		}
	}
	else if (try_get_root(root))
	{
		Log::info("(RibbonCortex) Opening tab {} -> {}...", category, tab);

		std::vector<chomsky::GUIContent> buttons;
		if (!this->ribbon_buttons_query.collect(root, buttons))
		{
			Log::info("(RibbonCortex) No buttons on ribbon.");

			return Result::Status::failure;
		}

		chomsky::GUIContent button;
		for (auto& i: buttons)
		{
			std::string name;
			if (!try_get_name(i, name))
			{
				continue;
			}

			if (name.substr(0, category.length()) == category)
			{
				MotionSequence sequence(*this);
				sequence.hover(i);

				auto is_pop_up_visible = [this]
				{
					chomsky::GUIContent pop_up;
					return try_get_root(this->ribbon_pop_up_query, pop_up);
				};
				const float wait_time = Config::get<float>("common", "gui", "GUI_GUARD_WAIT_TIME");
				sequence.wait(wait_time, is_pop_up_visible);

				Message message;
				message.set_string("category", category);
				message.set_string("tab", tab);
				sequence.poke<RibbonCortex>("open", message, is_pop_up_visible);

				return perform(sequence);
				break;
			}
		}

		Log::info("(RibbonCortex) Category button not found.");
		return Result::Status::failure;
	}

	Log::info("(RibbonCortex) Couldn't open tab {} -> {}: Ribbon isn't visible.", category, tab);

	return Result::Status::failure;
}

bool autonomaus::RibbonCortex::is_open(const std::string& name) const
{
	chomsky::GUIContent gui_root;
	if (!try_get_global_root(gui_root))
	{
		return false;
	}

	std::vector<chomsky::GUIContent> tabs;
	if (!this->panel_tabs_query.collect(gui_root, tabs) || tabs.empty())
	{
		return false;
	}

	for (auto& tab: tabs)
	{
		if (!tab.has("icon"))
		{
			continue;
		}
		auto icon = tab.get("icon");

		std::string n;
		if (!try_get_name(icon, n))
		{
			continue;
		}

		if (n.substr(0, name.length()) == name)
		{
			return true;
		}
	}

	return false;
}


bool autonomaus::RibbonCortex::is_active(const std::string& name) const
{
	chomsky::GUIContent gui_root;
	if (!try_get_global_root(gui_root))
	{
		return false;
	}

	std::vector<chomsky::GUIContent> tabs;
	if (!this->panel_tabs_query.collect(gui_root, tabs) || tabs.empty())
	{
		return false;
	}

	for (auto& tab: tabs)
	{
		if (!tab.has("icon"))
		{
			continue;
		}
		auto icon = tab.get("icon");

		std::string n;
		if (!try_get_name(icon, n))
		{
			continue;
		}

		if (n.substr(0, name.length()) != name)
		{
			continue;
		}

		if (tab.get_tags().has("active"))
		{
			return true;
		}
	}

	return false;
}
