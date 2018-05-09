// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompromat/events/itemIconEventBuilder.hpp"

kompromat::ItemIconEventBuilder::ItemIconEventBuilder(
	kompvter::TextureManager& texture_manager)
	: texture_manager(texture_manager)
{
	// Nada.
}

const kompvter::ItemIconEvent& kompromat::ItemIconEventBuilder::get_event() const
{
	return this->event;
}

void kompromat::ItemIconEventBuilder::from_transfer_item_icon_texture(
	const paley::Event& e)
{
	int source_name;
	{
		paley::EventField s;
		if (!e.get_root().try_get_field("source", s) ||
			!s.try_get_integer(source_name))
		{
			assert(false);
			source_name = 0;
		}
	}
	this->event.set_source_texture_name(source_name);
	this->event.set_source_texture(texture_manager.get_texture(source_name));

	int destination_name;
	{
		paley::EventField d;
		if (!e.get_root().try_get_field("destination", d) ||
			!d.try_get_integer(destination_name))
		{
			assert(false);
			destination_name = 0;
		}
	}
	this->event.set_destination_texture_name(destination_name);
	this->event.set_destination_texture(texture_manager.get_texture(destination_name));
}

void kompromat::ItemIconEventBuilder::from_clear_item_icon_texture(
	const paley::Event& e)
{
	int destination_name;
	{
		paley::EventField t;
		if (!e.get_root().try_get_field("texture", t) ||
			!t.try_get_integer(destination_name))
		{
			assert(false);
			destination_name = 0;
		}
	}
	this->event.set_destination_texture_name(destination_name);
	this->event.set_destination_texture(texture_manager.get_texture(destination_name));
}
