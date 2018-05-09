// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <boost/regex.hpp>
#include "autonomaus/core/log.hpp"
#include "autonomaus/state/memory/dialogInputPromptCortex.hpp"
#include "autonomaus/state/memory/motionSequence.hpp"
#include "chomsky/gui/guiText.hpp"

autonomaus::DialogInputPromptCortex::DialogInputPromptCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::DialogInputPrompt", autonomaus, "*.main.windows[*]:dialog-prompt-window"),
	prompt_query("*.prompt"),
	input_query("*.input")
{
	on_pull(
		"get_prompt",
		[this](auto& message)
		{
			Message result;
			result.set_string("result", this->get_prompt());
			return result;
		});

	on_pull(
		"get_input",
		[this](auto& message)
		{
			Message result;
			result.set_string("result", this->get_input());
			return result;
		});

	on_poke(
		"type",
		[this](auto& message)
		{
			auto text = message.get_string("text");
			if (message.has("ignore_case"))
			{
				return this->type(text, message.get_boolean("ignore_case"));
			}
			else
			{
				return this->type(text);
			}
		});

	on_poke(
		"clear",
		[this](auto)
		{
			return this->clear();
		});

	on_poke(
		"submit",
		[this](auto)
		{
			return this->submit();
		});
}

std::string autonomaus::DialogInputPromptCortex::get_prompt() const
{
	std::string result;
	chomsky::GUIContent text;
	if (!try_get_child(this->prompt_query, text) ||
		!try_get_text(text, result))
	{
		return std::string();
	}

	return result;
}

std::string autonomaus::DialogInputPromptCortex::get_input() const
{
	std::string result;
	chomsky::GUIContent text;
	if (!try_get_child(this->input_query, text) ||
		!try_get_text(text, result))
	{
		return std::string();
	}

	return result;
}

autonomaus::Cortex::Result autonomaus::DialogInputPromptCortex::type(
	const std::string& text, bool ignore_case)
{
	Log::info("(DialogInputPromptCortex) Typing '{}'...", text);

	MotionSequence sequence(*this);
	sequence.type(text, ignore_case, [this] { return this->is_visible(); });

	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::DialogInputPromptCortex::clear()
{
	Log::info("(DialogInputPromptCortex) Clearing input...");

	auto input = get_input();
	if (input.empty())
	{
		Log::info("(DialogInputPromptCortex) Input empty. Easy!");
		return Result::Status::success;
	}

	std::string text;
	text.resize(input.length(), '\b');

	MotionSequence sequence(*this);
	sequence.type(text, false, [this] { return this->is_visible(); });

	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::DialogInputPromptCortex::submit()
{
	Log::info("(DialogInputPromptCortex) Submitting input...");

	MotionSequence sequence(*this);
	sequence.type("\n", false, [this] { return this->is_visible(); });

	return perform(sequence);
}
