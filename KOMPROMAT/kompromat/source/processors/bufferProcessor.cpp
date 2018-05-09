// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompvter/events/bufferEvent.hpp"
#include "kompromat/events/bufferEventBuilder.hpp"
#include "kompromat/processors/bufferProcessor.hpp"

void kompromat::BufferProcessor::process(
	const std::string& scope,
	int name,
	const paley::Event& event)
{
	if (scope == "ARDEN")
	{
		if (event.get_name() == "buffer_create")
		{
			process_buffer_create(event);
		}
		else if (event.get_name() == "buffer_delete")
		{
			process_buffer_delete(event);
		}
		else if (event.get_name() == "buffer_allocate")
		{
			process_buffer_allocate(event);
		}
		else if (event.get_name() == "buffer_clear")
		{
			process_buffer_clear(event);
		}
		else if (event.get_name() == "buffer_update")
		{
			process_buffer_update(event);
		}
	}
}

void kompromat::BufferProcessor::process_buffer_create(const paley::Event& event)
{
	BufferEventBuilder buffer_event_builder;
	buffer_event_builder.from_buffer_create(event);
	auto& e = buffer_event_builder.get_event();

	for_each(
		[&e](kompvter::BufferListener* listener)
		{
			listener->on_buffer_create(e);
		});
}

void kompromat::BufferProcessor::process_buffer_delete(const paley::Event& event)
{
	BufferEventBuilder buffer_event_builder;
	buffer_event_builder.from_buffer_delete(event);
	auto& e = buffer_event_builder.get_event();

	for_each(
		[&e](kompvter::BufferListener* listener)
		{
			listener->on_buffer_delete(e);
		});
}

void kompromat::BufferProcessor::process_buffer_allocate(const paley::Event& event)
{
	BufferEventBuilder buffer_event_builder;
	buffer_event_builder.from_buffer_allocate(event);
	auto& e = buffer_event_builder.get_event();

	for_each(
		[&e](kompvter::BufferListener* listener)
		{
			listener->on_buffer_allocate(e);
		});
}

void kompromat::BufferProcessor::process_buffer_clear(const paley::Event& event)
{
	BufferEventBuilder buffer_event_builder;
	buffer_event_builder.from_buffer_clear(event);
	auto& e = buffer_event_builder.get_event();

	for_each(
		[&e](kompvter::BufferListener* listener)
		{
			listener->on_buffer_clear(e);
		});
}

void kompromat::BufferProcessor::process_buffer_update(const paley::Event& event)
{
	BufferEventBuilder buffer_event_builder;
	buffer_event_builder.from_buffer_update(event);
	auto& e = buffer_event_builder.get_event();

	for_each(
		[&e](kompvter::BufferListener* listener)
		{
			listener->on_buffer_update(e);
		});
}
