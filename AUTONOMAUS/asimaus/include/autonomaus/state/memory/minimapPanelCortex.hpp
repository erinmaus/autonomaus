// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_MINIMAP_PANEL_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_MINIMAP_PANEL_CORTEX_HPP

#include "autonomaus/state/memory/guiCortex.hpp"
#include "chomsky/gui/guiQuery.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace autonomaus
{
	class MinimapPanelCortex : public GUICortex
	{
	public:
		MinimapPanelCortex(Autonomaus& autonomaus);
		~MinimapPanelCortex() = default;

		Cortex::Result enable_run();
		Cortex::Result disable_run();
		Cortex::Result toggle_run();
		bool is_running() const;
		bool is_walking() const;
		int get_run_energy() const;

		Cortex::Result rest();
		bool is_resting() const;

		Cortex::Result click_tile(
			const thoreau::TileCoordinate& tile,
			float radius = 8.0f);
		Cortex::Result zoom_in(int ticks);
		Cortex::Result zoom_out(int ticks);

		bool is_tile_clickable(const thoreau::TileCoordinate& tile_coordinate) const;

		bool is_minimap_hidden() const;
		float get_minimap_rotation() const;
		float get_minimap_scale() const;
		thoreau::TileCoordinate get_minimap_center() const;

		Cortex::Result open_home_teleport_window();
		Cortex::Result teleport_last_destination(const std::string& destination);

		void update() override;

	private:
		chomsky::GUIQuery run_button_query;
		chomsky::GUIQuery run_button_resting_query;
		chomsky::GUIQuery run_button_energy_query;
		void update_run();

		bool try_minimap_to_panel(
			const thoreau::TileCoordinate& tile_coordinate,
			glm::ivec2& result) const;

		chomsky::GUIQuery home_teleport_button_query;

		chomsky::GUIQuery minimap_query;
		struct ClickTileAction;
	};
}

#endif
