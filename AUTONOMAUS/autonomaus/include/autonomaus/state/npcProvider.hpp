// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_NPC_PROVIDER_HPP
#define AUTONOMAUS_STATE_NPC_PROVIDER_HPP

#include <memory>
#include "autonomaus/state/gameStateProvider.hpp"
#include "autonomaus/state/mapProvider.hpp"
#include "autonomaus/state/worldProvider.hpp"
#include "glooper/npc/npcCabinet.hpp"
#include "glooper/npc/npcManager.hpp"

namespace autonomaus
{
	class NPCProvider :
		public GameStateProvider<glooper::NPCCabinet, NPCProvider, GameStateDependentChannel>
	{
	public:
		typedef GameStateProvider<glooper::NPCCabinet, NPCProvider, GameStateDependentChannel> Base;

		NPCProvider(
			const MapProvider& map_provider,
			const WorldProvider& world_provider);
		~NPCProvider() = default;

		void attach(GameStateManager& manager, Slot& slot) override;
		void detach() override;

		void update() override;

		static void add_profile(
			GameStateManager& game_state,
			const View& view,
			const std::string& name);
		static void remove_profile(
			GameStateManager& game_state,
			const View& view,
			const std::string& name);

	private:
		const MapProvider* map_provider;
		const WorldProvider* world_provider;

		std::unique_ptr<glooper::NPCManager> npc_manager;
		std::multimap<std::string, glooper::NPCProfile> profiles;
	};
}

#endif
