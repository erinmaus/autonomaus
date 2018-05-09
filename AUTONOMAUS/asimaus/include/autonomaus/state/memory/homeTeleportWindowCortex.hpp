// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_HOME_TELEPORT_WINDOW_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_HOME_TELEPORT_WINDOW_CORTEX_HPP

#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"

namespace autonomaus
{
	class HomeTeleportWindowCortex : public GUICortex
	{
	public:
		HomeTeleportWindowCortex(Autonomaus& autonomaus);
		~HomeTeleportWindowCortex() = default;

		enum
		{
			teleport_slow,
			teleport_quick,
			teleport_whatever
		};
		Cortex::Result teleport(const std::string& destination, int mode = teleport_whatever);

		std::string get_last_destination() const;
		void forget_last_destination();
		void set_last_destination(const std::string& value);

		bool is_quick_teleport_enabled() const;

		static const int UNKNOWN_QUICK_CHARGES = -1;
		int get_num_quick_charges() const;

		enum
		{
			status_unlocked,
			status_locked,
			status_unknown
		};
		int status(const std::string& destination) const;
		Cortex::Result activated_lodestone(const std::string& destination);

		Cortex::Result close();

		void update() override;

	private:
		chomsky::GUIQuery icons_query;
		void update_icons();

		chomsky::GUIQuery close_button_query;

		chomsky::GUIQuery quick_charge_toggle_query;
		chomsky::GUIQuery quick_charge_value_query;
		void update_charges();
	};
}

#endif
