// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/state/mapProvider.hpp"
#include "autonomaus/state/gameStateManager.hpp"

void autonomaus::MapProvider::attach(GameStateManager& manager, Slot& slot)
{
	Base::attach(manager, slot);
	this->minimap_manager = std::make_unique<glooper::MinimapManager>(
		manager.get_resource_manager(), slot.get_working().minimap, false);
	manager.get_event_processors().get_frame_processor().add(this->minimap_manager.get());
	manager.get_event_processors().get_texture_processor().add(this->minimap_manager.get());
	manager.get_event_processors().get_gui_processor().add(this->minimap_manager.get());
	manager.get_event_processors().get_minimap_processor().add(this->minimap_manager.get());
}

void autonomaus::MapProvider::detach()
{
	get_manager()->get_event_processors().get_frame_processor().remove(this->minimap_manager.get());
	get_manager()->get_event_processors().get_texture_processor().remove(this->minimap_manager.get());
	get_manager()->get_event_processors().get_gui_processor().remove(this->minimap_manager.get());
	get_manager()->get_event_processors().get_minimap_processor().remove(this->minimap_manager.get());
	this->minimap_manager.reset();

	Base::detach();
}

void autonomaus::MapProvider::update()
{
	auto& runtime_map = get_manager()->get_resource_manager().get_runtime_map();
	get_slot()->get_working().map = runtime_map;
}
