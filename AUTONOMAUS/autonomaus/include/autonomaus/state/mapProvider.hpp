// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MAP_PROVIDER_HPP
#define AUTONOMAUS_STATE_MAP_PROVIDER_HPP

#include <memory>
#include "autonomaus/state/gameStateProvider.hpp"
#include "autonomaus/state/map.hpp"
#include "glooper/map/minimap.hpp"
#include "glooper/map/minimapManager.hpp"

namespace autonomaus
{
	class MapProvider :
		public GameStateProvider<Map, MapProvider, GameStateIndependentChannel>
	{
	public:
		typedef GameStateProvider<Map, MapProvider, GameStateIndependentChannel> Base;
		using Base::Slot;

		MapProvider() = default;
		~MapProvider() = default;

		void attach(GameStateManager& manager, Slot& slot) override;
		void detach() override;

		void update() override;

	private:
		std::unique_ptr<glooper::MinimapManager> minimap_manager;
	};
}

#endif
