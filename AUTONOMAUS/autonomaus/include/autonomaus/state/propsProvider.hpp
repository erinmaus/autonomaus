// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_PROPS_PROVIDER_HPP
#define AUTONOMAUS_STATE_PROPS_PROVIDER_HPP

#include <memory>
#include "autonomaus/state/gameStateProvider.hpp"
#include "autonomaus/state/worldProvider.hpp"
#include "glooper/prop/propManager.hpp"
#include "glooper/prop/props.hpp"

namespace autonomaus
{
	class PropsProvider :
		public GameStateProvider<glooper::Props, PropsProvider, GameStateDependentChannel>
	{
	public:
		typedef GameStateProvider<glooper::Props, PropsProvider, GameStateDependentChannel> Base;
		using Base::Slot;

		PropsProvider(const WorldProvider& world_provider);
		~PropsProvider() = default;

		void attach(GameStateManager& manager, Slot& slot) override;
		void detach() override;

		void update() override;

		static void add_definition(
			GameStateManager& game_state,
			const View& view,
			const std::string& name);
		static void remove_definition(
			GameStateManager& game_state,
			const View& view,
			const std::string& name);

	private:
		const WorldProvider* world_provider;
		std::unique_ptr<glooper::PropManager> prop_manager;
		std::multimap<std::string, glooper::PropDefinition> definitions;
	};
}

#endif
