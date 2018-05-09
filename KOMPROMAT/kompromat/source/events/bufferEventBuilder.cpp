// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompromat/events/bufferEventBuilder.hpp"

const kompvter::BufferEvent& kompromat::BufferEventBuilder::get_event() const
{
	return this->event;
}

void kompromat::BufferEventBuilder::from_buffer_create(const paley::Event& e)
{
	process_name(e);
}

void kompromat::BufferEventBuilder::from_buffer_delete(const paley::Event &e)
{
	process_name(e);
}

void kompromat::BufferEventBuilder::from_buffer_allocate(const paley::Event& e)
{
	process_name(e);

	int buffer_size;
	{
		paley::EventField s;
		if (!e.get_root().try_get_field("size", s) ||
			!s.try_get_integer(buffer_size))
		{
			assert(false);
			buffer_size = 0;
		}
	}
	assert(buffer_size >= 0);
	this->event.set_data_size(buffer_size);
}

void kompromat::BufferEventBuilder::from_buffer_clear(const paley::Event &e)
{
	process_name(e);
}

void kompromat::BufferEventBuilder::from_buffer_update(const paley::Event& e)
{
	process_name(e);

	const std::uint8_t* data_pointer;
	std::size_t data_size;
	if (!e.get_root().try_get_field("data", this->event_data) ||
		!this->event_data.try_get_data(data_pointer, data_size))
	{
		assert(false);
		data_pointer = nullptr;
		data_size = 0;
	}

	int data_offset;
	{
		paley::EventField o;
		if (!e.get_root().try_get_field("offset", o) ||
			!o.try_get_integer(data_offset))
		{
			assert(false);
			data_offset = 0;
		}
	}
	assert(data_offset >= 0);

	this->event.set_data(data_pointer);
	this->event.set_data_size(data_size);
	this->event.set_offset(data_offset);
}

void kompromat::BufferEventBuilder::process_name(const paley::Event& e)
{
	int name;
	{
		paley::EventField n;
		if (!e.get_root().try_get_field("name", n) ||
			!n.try_get_integer(name))
		{
			assert(false);
			name = 0;
		}
	}
	event.set_name(name);
}
