// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/config.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/memory/progressWindowCortex.hpp"

const float autonomaus::ProgressWindowCortex::CLOSE_TIME =
	Config::get<float>("gui", "ProgressWindowCortex", "CLOSE_TIME");

autonomaus::ProgressWindowCortex::ProgressWindowCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::ProgressWindow", autonomaus, "*.main.windows[*]:progress-window"),
	cancel_button_query("*.cancel-button:button"),
	close_button_query("*.close-button:button")
{
	on_poke(
		"cancel",
		[this](auto&)
		{
			return this->cancel();
		});

	on_poke(
		"close",
		[this](auto&)
		{
			return this->close();
		});
}

autonomaus::Cortex::Result autonomaus::ProgressWindowCortex::cancel()
{
	Log::info("(ProgressWindowCortex) Canceling action...");

	chomsky::GUIContent button;
	if (!try_get_child(this->cancel_button_query, button))
	{
		Log::warn("(ProgressWindowCortex) Couldn't find cancel button. Is window open?");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.left_click(button, [this] { return this->is_visible(); });
	sequence.wait(CLOSE_TIME, [this] { return !this->is_visible(); });

	return perform(sequence);
}

autonomaus::Cortex::Result autonomaus::ProgressWindowCortex::close()
{
	Log::info("(ProgressWindowCortex) Closing window...");

	chomsky::GUIContent button;
	if (!try_get_child(this->close_button_query, button))
	{
		Log::warn("(ProgressWindowCortex) Couldn't find close button. Is window open?");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.left_click(button, [this] { return this->is_visible(); });
	sequence.wait(CLOSE_TIME, [this] { return !this->is_visible(); });

	return perform(sequence);
}
