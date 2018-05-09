// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/propsProvider.hpp"

autonomaus::PropsProvider::PropsProvider(const WorldProvider& world_provider)
{
	this->world_provider = &world_provider;
}

void autonomaus::PropsProvider::attach(GameStateManager& manager, Slot& slot)
{
	Base::attach(manager, slot);
	this->prop_manager = std::make_unique<glooper::PropManager>(
		manager.get_resource_manager(),
		slot.get_working());
}

void autonomaus::PropsProvider::detach()
{
	this->prop_manager.reset();

	Base::detach();
}

void autonomaus::PropsProvider::add_definition(
	GameStateManager& game_state,
	const View& view,
	const std::string& name)
{
	game_state.execute<PropsProvider>(
		view,
		[name](auto& provider)
		{
			auto definition = provider.prop_manager->add_definition(name);
			provider.definitions.emplace(name, definition);
		});
}

void autonomaus::PropsProvider::remove_definition(
	GameStateManager& game_state,
	const View& view,
	const std::string& name)
{
	game_state.execute<PropsProvider>(
		view,
		[name](auto& provider)
		{
			auto iter = provider.definitions.find(name);
			if (iter != provider.definitions.end())
			{
				provider.definitions.erase(iter);
			}
		});
}

void autonomaus::PropsProvider::update()
{
	this->prop_manager->update(
		*this->world_provider->get_dirty_tile_state(),
		*this->world_provider->get_static_block());
}
