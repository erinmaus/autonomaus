// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <boost/regex.hpp>
#include "autonomaus/core/log.hpp"
#include "autonomaus/core/config.hpp"
#include "autonomaus/core/utility.hpp"
#include "autonomaus/state/memory/rightClickMenuCortex.hpp"
#include "autonomaus/state/memory/motionSequence.hpp"

autonomaus::RightClickMenuCortex::RightClickMenuCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::RightClickMenu", autonomaus, "*.main.right-click-menu:menu"),
	options_query("*.options[*]:menu-option.text"),
	cancel_query("*.cancel")
{
	on_poke(
		"select",
		[this](auto message)
		{
			if (message.get("option").get_type() == Message::Field::type_string)
			{
				return this->select(message.get_string("option"), message.get_string("target"));
			}
			else
			{
				return this->select(message.get_integer("option"));
			}
		});

	on_poke(
		"select_any",
		[this](auto message)
		{
			std::vector<std::string> actions;
			auto options = message.get("options");
			for (std::size_t i = 0; i < options.length(); ++i)
			{
				actions.push_back(options.get_string(i));
			}

			return this->select_any(actions, message.get_string("target"));
		});
}

bool autonomaus::RightClickMenuCortex::match(
	const std::string& action, const std::string& target) const
{
	return match_any({ action }, target);
}

bool autonomaus::RightClickMenuCortex::match_any(
	const std::vector<std::string>& actions, const std::string& target) const
{
	int num_options = get_num_options();
	for (int i = 0; i < num_options; ++i)
	{
		for (auto& action: actions)
		{
			if (action.empty())
			{
				continue;
			}

			if (target.empty())
			{
				auto option = get_option_string(i);
				if (fuzzy_string_match(option, action))
				{
					return true;
				}
			}
			else
			{
				auto text = get_option_text(i);
				if (text.size() < 2)
				{
					continue;
				}

				auto option_action = trim_string(text[0]);
				auto option_target = trim_string(text[1]);
				if (fuzzy_string_match(option_action, action) &&
					fuzzy_string_match(target, option_target))
				{
					return true;
				}
			}
		}
	}

	return false;
}

int autonomaus::RightClickMenuCortex::get_num_options() const
{
	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		return 0;
	}

	std::vector<chomsky::GUIContent> elements;
	if (!this->options_query.collect(root, elements) || elements.empty())
	{
		return 0;
	}

	return elements.size();
}

std::vector<std::string> autonomaus::RightClickMenuCortex::get_option_text(
	int option) const
{
	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		return {};
	}

	std::vector<chomsky::GUIContent> elements;
	if (!this->options_query.collect(root, elements) || elements.empty())
	{
		return {};
	}

	if (option < 0 || option >= (int)elements.size())
	{
		return {};
	}

	std::vector<std::string> result;
	auto& text = elements[option].get_text();
	for (auto& i: text)
	{
		auto t = std::get<chomsky::GUIText::SEGMENT_STRING>(i);
		if (!t.empty())
		{
			result.push_back(t);
		}
	}

	return result;
}

std::string autonomaus::RightClickMenuCortex::get_option_string(
	int option) const
{
	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		return {};
	}

	std::vector<chomsky::GUIContent> elements;
	if (!this->options_query.collect(root, elements) || elements.empty())
	{
		return {};
	}

	if (option < 0 || option >= (int)elements.size())
	{
		return {};
	}

	return elements[option].get_text().to_string();
}

autonomaus::Cortex::Result autonomaus::RightClickMenuCortex::select(int option)
{
	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		Log::warn("(RightClickMenuCortex) Right click menu not visible.");
		return Result::Status::failure;
	}

	std::vector<chomsky::GUIContent> elements;
	if (!this->options_query.collect(root, elements) || elements.empty())
	{
		return Result::Status::failure;
	}

	if (option < 0 || option >= (int)elements.size())
	{
		Log::warn("(RightClickMenuCortex) Failed to selection option {}: value out of bounds.", option);
		return Result::Status::failure;
	}

	Log::info(
		"(RightClickMenuCortex) Selecting option {}.",
		elements[option].get_text().to_string());

	MotionSequence sequence(*this);
	sequence.left_click(elements[option], [this] { return is_visible(); });

	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::RightClickMenuCortex::select(
	const std::string& action,
	const std::string& target)
{
	return select_any({ action }, target);
}

autonomaus::Cortex::Result autonomaus::RightClickMenuCortex::select_any(
	const std::vector<std::string>& actions, const std::string& target)
{
	if (actions.empty())
	{
		return Result::Status::failure;
	}

	if (!is_visible())
	{
		Log::info("(RightClickMenuCortex) Opening right-click menu...");

		MotionSequence sequence(*this);

		sequence.right_click();
		sequence.wait<RightClickMenuCortex>(Config::get<float>("common", "gui", "RIGHT_CLICK_WAIT_TIME"));

		Message message;
		for (std::size_t i = 0; i < actions.size(); ++i)
		{
			message.set_string("options", actions[i], i);
		}

		sequence.poke<RightClickMenuCortex>("select_any", message);

		return perform(sequence);
	}
	else
	{
		int num_options = get_num_options();
		for (int i = 0; i < num_options; ++i)
		{
			for (auto& action: actions)
			{
				if (action.empty())
				{
					continue;
				}

				if (target.empty())
				{
					auto option = get_option_string(i);
					if (fuzzy_string_match(option, action))
					{
						return select(i);
					}
				}
				else
				{
					auto text = get_option_text(i);
					if (text.size() < 2)
					{
						continue;
					}

					auto option_action = trim_string(text[0]);
					auto option_target = trim_string(text[1]);
					if (fuzzy_string_match(option_action, action) &&
						fuzzy_string_match(target, option_target))
					{
						return select(i);
					}
				}
			}
		}
		
		Log::info("(RightClickMenuCortex) Option selection failed, closing menu.");

		chomsky::GUIContent cancel;
		if (try_get_child(this->cancel_query, cancel))
		{
			MotionSequence sequence(*this);
			sequence.left_click(cancel, [this] { return this->is_visible(); });
			sequence.wait(0.0f, [] { return false; });
			return perform(sequence);
		}
		else
		{
			Log::info("(RightClickMenuCortex) Couldn't click cancel!");
		}

		return Result::Status::failure;
	}
}
