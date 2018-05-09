// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompvter/events/bufferEvent.hpp"

int kompvter::BufferEvent::get_name() const
{
	return this->name;
}

void kompvter::BufferEvent::set_name(int value)
{
	this->name = value;
}

std::size_t kompvter::BufferEvent::get_offset() const
{
	return this->offset;
}

void kompvter::BufferEvent::set_offset(std::size_t value)
{
	this->offset = value;
}

const std::uint8_t* kompvter::BufferEvent::get_data() const
{
	return this->data;
}

void kompvter::BufferEvent::set_data(const std::uint8_t* value)
{
	this->data = value;
}

std::size_t kompvter::BufferEvent::get_data_size() const
{
	return this->data_size;
}

void kompvter::BufferEvent::set_data_size(std::size_t value)
{
	this->data_size = value;
}
