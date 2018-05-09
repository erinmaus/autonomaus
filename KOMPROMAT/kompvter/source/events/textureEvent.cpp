// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompvter/events/textureEvent.hpp"

int kompvter::TextureEvent::get_name() const
{
	return get_destination_name();
}

void kompvter::TextureEvent::set_name(int value)
{
	this->destination = value;
}

int kompvter::TextureEvent::get_destination_name() const
{
	return this->destination;
}

void kompvter::TextureEvent::set_destination_name(int value)
{
	this->destination = value;
}

int kompvter::TextureEvent::get_source_name() const
{
	return this->source;
}

void kompvter::TextureEvent::set_source_name(int value)
{
	this->source = value;
}

int kompvter::TextureEvent::get_component_format() const
{
	return this->component_format;
}

void kompvter::TextureEvent::set_component_format(int value)
{
	this->component_format = value;
}

int kompvter::TextureEvent::get_component_type() const
{
	return this->component_type;
}

void kompvter::TextureEvent::set_component_type(int value)
{
	this->component_type = value;
}

int kompvter::TextureEvent::get_compression() const
{
	return this->compression;
}

void kompvter::TextureEvent::set_compression(int value)
{
	this->compression = value;
}

bool kompvter::TextureEvent::get_is_compressed() const
{
	return this->compression != compression_none;
}

int kompvter::TextureEvent::get_width() const
{
	return this->width;
}

void kompvter::TextureEvent::set_width(int value)
{
	this->width = value;
}

int kompvter::TextureEvent::get_height() const
{
	return this->height;
}

void kompvter::TextureEvent::set_height(int value)
{
	this->height = value;
}

int kompvter::TextureEvent::get_x() const
{
	return get_destination_x();
}

void kompvter::TextureEvent::set_x(int value)
{
	this->destination_x = value;
}

int kompvter::TextureEvent::get_y() const
{
	return get_destination_y();
}

void kompvter::TextureEvent::set_y(int value)
{
	this->destination_y = value;
}

int kompvter::TextureEvent::get_source_x() const
{
	return this->source_x;
}

void kompvter::TextureEvent::set_source_x(int value)
{
	this->source_x = value;
}

int kompvter::TextureEvent::get_source_y() const
{
	return this->source_y;
}

void kompvter::TextureEvent::set_source_y(int value)
{
	this->source_y = value;
}

int kompvter::TextureEvent::get_destination_x() const
{
	return this->destination_x;
}

void kompvter::TextureEvent::set_destination_x(int value)
{
	this->destination_x = value;
}

int kompvter::TextureEvent::get_destination_y() const
{
	return this->destination_y;
}

void kompvter::TextureEvent::set_destination_y(int value)
{
	this->destination_y = value;
}

const std::uint8_t* kompvter::TextureEvent::get_pixels_data() const
{
	return this->pixels_data;
}

void kompvter::TextureEvent::set_pixels_data(const std::uint8_t* value)
{
	this->pixels_data = value;
}

std::size_t kompvter::TextureEvent::get_pixels_data_size() const
{
	return this->pixels_data_size;
}

void kompvter::TextureEvent::set_pixels_data_size(std::size_t value)
{
	this->pixels_data_size = value;
}
