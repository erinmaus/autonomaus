// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/events/itemIconEvent.hpp"
#include "kompvter/graphics/texture.hpp"

const kompvter::Texture& kompvter::ItemIconEvent::get_texture() const
{
	return this->destination;
}

void kompvter::ItemIconEvent::set_texture(const kompvter::Texture& value)
{
	this->destination = value;
}

int kompvter::ItemIconEvent::get_texture_name() const
{
	return this->destination_name;
}

void kompvter::ItemIconEvent::set_texture_name(int value)
{
	this->destination_name = value;
}

const kompvter::Texture& kompvter::ItemIconEvent::get_source_texture() const
{
	return this->source;
}

void kompvter::ItemIconEvent::set_source_texture(const kompvter::Texture& value)
{
	this->source = value;
}

int kompvter::ItemIconEvent::get_source_texture_name() const
{
	return this->source_name;
}

void kompvter::ItemIconEvent::set_source_texture_name(int value)
{
	this->source_name = value;
}

const kompvter::Texture& kompvter::ItemIconEvent::get_destination_texture() const
{
	return this->destination;
}

void kompvter::ItemIconEvent::set_destination_texture(const kompvter::Texture& value)
{
	this->destination = value;
}

int kompvter::ItemIconEvent::get_destination_texture_name() const
{
	return this->destination_name;
}

void kompvter::ItemIconEvent::set_destination_texture_name(int value)
{
	this->destination_name = value;
}
