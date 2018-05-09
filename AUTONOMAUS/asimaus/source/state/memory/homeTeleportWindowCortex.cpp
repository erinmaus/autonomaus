// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/config.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/state/memory/homeTeleportWindowCortex.hpp"
#include "autonomaus/state/memory/minimapPanelCortex.hpp"
#include "autonomaus/state/memory/motionSequence.hpp"

autonomaus::HomeTeleportWindowCortex::HomeTeleportWindowCortex(Autonomaus& autonomaus) :
	GUICortex("GUI::HomeTeleportWindow", autonomaus, "*.main.windows[*]:home-teleport-window"),
	icons_query("*.icons[*].unlocked"),
	close_button_query("*.close-button:button"),
	quick_charge_toggle_query("*.quick-charges.toggle:button"),
	quick_charge_value_query("*.quick-charges.charges")
{
	on_poke(
		"teleport",
		[this](auto& message)
		{
			int mode = teleport_whatever;
			if (message.has("mode"))
			{
				mode = message.get_integer("mode");
			}

			return this->teleport(message.get_string("destination"), mode);
		});

	on_pull(
		"last_destination",
		[this](auto)
		{
			Message result;
			result.set_string("result", this->get_last_destination());
			return result;
		});

	on_pull(
		"is_quick_teleport_enabled",
		[this](auto)
		{
			Message result;
			result.set_boolean("result", this->is_quick_teleport_enabled());
			return result;
		});

	on_pull(
		"num_quick_charges",
		[this](auto)
		{
			Message result;
			result.set_integer("result", this->get_num_quick_charges());
			return result;
		});

	on_pull(
		"status",
		[this](auto& message)
		{
			Message result;
			result.set_integer("result", this->status(message.get_string("destination")));
			return result;
		});

	on_poke(
		"activated_lodestone",
		[this](auto& message)
		{
			return this->activated_lodestone(message.get_string("destination"));
		});

	on_poke(
		"close",
		[this](auto& message)
		{
			return this->close();
		});

	remember("current-destinations", get("destinations"));
}

autonomaus::Cortex::Result autonomaus::HomeTeleportWindowCortex::teleport(
	const std::string& destination, int mode)
{
	if (status(destination) == status_locked)
	{
		Log::info("(HomeTeleportWindowCortex): Destination '{0}' locked!", destination);
		return Result::Status::failure;
	}

	if (is_visible())
	{
		chomsky::GUIContent root;
		if (!try_get_root(root))
		{
			Log::info("(HomeTeleportWindowCortex): Bad logic! Failed to get root, yet visible.");

			return Result::Status::failure;
		}

		chomsky::GUIContent quick_charge_toggle_button;
		if (!try_get_child(this->quick_charge_toggle_query, quick_charge_toggle_button))
		{
			Log::info("(HomeTeleportWindowCortex): Bad logic! Couldn't get quick charge toggle button.");
			return Result::Status::failure;
		}

		std::vector<chomsky::GUIContent> destinations;
		if (!icons_query.collect(root, destinations) || destinations.empty())
		{
			Log::info("(HomeTeleportWindowCortex): No destination icons, or icon query failed!");
			return Result::Status::failure;
		}

		for (auto& i: destinations)
		{
			std::string d;
			if (try_get_data(i, "destination", d) && d == destination)
			{
				MotionSequence sequence(*this);
				if ((mode == teleport_quick && !is_quick_teleport_enabled()) ||
					(mode == teleport_slow && is_quick_teleport_enabled()))
				{
					sequence.left_click(
						quick_charge_toggle_button,
						[this] { return this->is_visible(); });
				}

				sequence.left_click(i, [this] { return this->is_visible(); });

				const float wait_time = Config::get<float>("common", "gui", "WINDOW_WAIT_TIME");
				sequence.wait(wait_time, [this] { return !this->is_visible(); });

				sequence.invoke(
					[this, destination]
					{
						Memory last_destination;
						last_destination->set_string("value", destination);
						this->remember("last-destination", last_destination);
					});


				return perform(sequence);
			}
		}

		Log::info("(HomeTeleportWindowCortex): Couldn't find teleport to '{0}'.", destination);
		return Result::Status::failure;
	}
	else
	{
		if (get_last_destination() == destination)
		{
			MotionSequence sequence(*this);

			Message message;
			message.set_string("destination", destination);
			sequence.poke<MinimapPanelCortex>("teleport_last_destination", message);

			return perform(sequence);
		}
		else
		{
			MotionSequence sequence(*this);
			sequence.poke<MinimapPanelCortex>("open_home_teleport_window", Message());

			const float wait_time = Config::get<float>("common", "gui", "WINDOW_WAIT_TIME");
			sequence.wait<HomeTeleportWindowCortex>(wait_time);

			Message message;
			message.set_string("destination", destination);
			message.set_integer("mode", mode);
			sequence.poke<HomeTeleportWindowCortex>(
				"teleport", message,
				[this] { return this->is_visible(); });

			return perform(sequence);
		}
	}
}

std::string autonomaus::HomeTeleportWindowCortex::get_last_destination() const
{
	if (has_memory("last-destination"))
	{
		return get("last-destination")->get_string("value");
	}

	return std::string();
}

void autonomaus::HomeTeleportWindowCortex::forget_last_destination()
{
	forget("last-destination");
}

void autonomaus::HomeTeleportWindowCortex::set_last_destination(
	const std::string& value)
{
	if (value.empty())
	{
		forget("last-destination");
	}
	else
	{
		Memory memory;
		memory->set_string("value", value);
		remember("last-destination", memory);
	}
}

bool autonomaus::HomeTeleportWindowCortex::is_quick_teleport_enabled() const
{
	return has_memory("quick-teleport");
}

int autonomaus::HomeTeleportWindowCortex::get_num_quick_charges() const
{
	if (!has_memory("quick-charges"))
	{
		return UNKNOWN_QUICK_CHARGES;
	}

	return get("quick-charges")->get_integer("value");
}

int autonomaus::HomeTeleportWindowCortex::status(const std::string& destination) const
{
	if (!has_memory("current-destinations"))
	{
		return status_unknown;
	}

	auto current_destinations = get("current-destinations");
	auto destinations = get("destinations");
	if (current_destinations->get_boolean(destination))
	{
		return status_unlocked;
	}
	else if (!destinations->get_boolean(destination))
	{
		return status_locked;
	}
	else
	{
		return status_unknown;
	}
}

autonomaus::Cortex::Result autonomaus::HomeTeleportWindowCortex::activated_lodestone(
	const std::string& destination)
{
	auto destinations = get("destinations");
	destinations->set_boolean(destination, true);
	remember("destinations", destinations);

	return Result::Status::success;
}

autonomaus::Cortex::Result autonomaus::HomeTeleportWindowCortex::close()
{
	Log::info("(HomeTeleportWindowCortex) Closing window...");

	if (!is_visible())
	{
		Log::info("(HomeTeleportWindowCortex) Window is already closed.");
		return Result::Status::success;
	}

	chomsky::GUIContent close_button;
	if (!try_get_child(this->close_button_query, close_button))
	{
		Log::info("(HomeTeleportWindowCortex) Couldn't find close button.");
		return Result::Status::failure;
	}

	MotionSequence sequence(*this);
	sequence.left_click(close_button, [this] { return this->is_visible(); });

	return perform(sequence);
}

void autonomaus::HomeTeleportWindowCortex::update()
{
	GUICortex::update();

	if (is_visible())
	{
		update_icons();
		update_charges();
	}
}

void autonomaus::HomeTeleportWindowCortex::update_icons()
{
	chomsky::GUIContent root;
	if (!try_get_root(root))
	{
		return;
	}

	std::vector<chomsky::GUIContent> icons;
	if (!this->icons_query.collect(root, icons) || icons.empty())
	{
		return;
	}

	auto destinations = get("destinations");
	auto current_destinations = get("current-destinations");
	for (auto& i: icons)
	{
		std::string destination;
		if (try_get_data(i, "destination", destination))
		{
			destinations->set_boolean(destination, true);
			current_destinations->set_boolean(destination, true);
		}
	}

	remember("destinations", destinations);
	remember("current-destinations", current_destinations);
}

void autonomaus::HomeTeleportWindowCortex::update_charges()
{
	chomsky::GUIContent content;
	if (try_get_child(quick_charge_toggle_query, content) &&
		content.get_tags().has("active"))
	{
		remember("quick-teleport", Memory());
	}
	else
	{
		forget("quick-teleport");
	}

	if (try_get_child(quick_charge_value_query, content))
	{
		Memory memory;
		memory->set_integer("value", std::atoi(content.get_text().to_string().c_str()));

		remember("quick-charges", memory);
	}
}
