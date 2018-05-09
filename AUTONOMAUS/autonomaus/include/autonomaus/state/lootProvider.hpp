// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_LOOT_PROVIDER_HPP
#define AUTONOMAUS_STATE_LOOT_PROVIDER_HPP

#include <memory>
#include "autonomaus/state/gameStateProvider.hpp"
#include "autonomaus/state/worldProvider.hpp"
#include "glooper/item/loot.hpp"
#include "glooper/item/lootManager.hpp"

namespace autonomaus
{
	class LootProvider :
		public GameStateProvider<glooper::Loot, LootProvider, GameStateDependentChannel>
	{
	public:
		typedef GameStateProvider<glooper::Loot, LootProvider, GameStateDependentChannel> Base;
		using Base::Slot;

		LootProvider(const WorldProvider& world_provider);
		~LootProvider() = default;

		void attach(GameStateManager& manager, Slot& slot) override;
		void detach() override;

		void update() override;

		static void add_item(
			GameStateManager& game_state,
			const View& view,
			const std::string& item,
			const chomsky::ItemIconSilhouette& icon);
		static void remove_item(
			GameStateManager& game_state,
			const View& view,
			const std::string& item);
		static void load_items(
			GameStateManager& game_state,
			const View& view,
			const std::string& path);

	private:
		const WorldProvider* world_provider;
		std::unique_ptr<glooper::LootManager> loot_manager;
	};
}

#endif
