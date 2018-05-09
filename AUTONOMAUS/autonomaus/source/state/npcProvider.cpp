// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/npcProvider.hpp"

autonomaus::NPCProvider::NPCProvider(
	const MapProvider& map_provider,
	const WorldProvider& world_provider)
{
	this->map_provider = &map_provider;
	this->world_provider = &world_provider;
}

void autonomaus::NPCProvider::attach(GameStateManager& manager, Slot& slot)
{
	Base::attach(manager, slot);
	this->npc_manager = std::make_unique<glooper::NPCManager>(
		manager.get_resource_manager(),
		slot.get_working(),
		this->map_provider->get_slot()->get_working().get_minimap(),
		*this->world_provider->get_animated_block());
	manager.get_event_processors().get_frame_processor().add(this->npc_manager.get());
	manager.get_event_processors().get_model_processor().add(this->npc_manager.get());
}

void autonomaus::NPCProvider::detach()
{
	get_manager()->get_event_processors().get_frame_processor().remove(this->npc_manager.get());
	get_manager()->get_event_processors().get_model_processor().remove(this->npc_manager.get());
	this->profiles.clear();
	this->npc_manager.reset();

	Base::detach();
}

void autonomaus::NPCProvider::add_profile(
	GameStateManager& game_state,
	const View& view,
	const std::string& name)
{
	game_state.execute<NPCProvider>(
		view,
		[name](auto& provider)
		{
			auto profile = provider.npc_manager->add_profile(name);
			provider.profiles.emplace(name, profile);
		});
}

void autonomaus::NPCProvider::remove_profile(
	GameStateManager& game_state,
	const View& view,
	const std::string& name)
{
	game_state.execute<NPCProvider>(
		view,
		[name](auto& provider)
		{
			auto iter = provider.profiles.find(name);
			if (iter != provider.profiles.end())
			{
				provider.profiles.erase(iter);
			}
		});
}

void autonomaus::NPCProvider::update()
{
	// Nothing.
}
