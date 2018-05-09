// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompromat/processors/frameProcessor.hpp"

void kompromat::FrameProcessor::process(
	const std::string& scope, int name, const paley::Event& event)
{
	if (scope == "ARDEN")
	{
		if (event.get_name() == "draw_frame")
		{
			process_draw_frame(event);
		}
		else if (event.get_name() == "draw_enabled")
		{
			process_draw_enabled(event);
		}
		else if (event.get_name() == "draw_disabled")
		{
			process_draw_disabled(event);
		}
	}
}

void kompromat::FrameProcessor::request_frame(paley::Client& client)
{
	paley::Event event;
	event.set_name("draw_request");

	client.send(event, "ARDEN");
}

void kompromat::FrameProcessor::process_draw_frame(const paley::Event& event)
{
	float timestamp = get_timestamp(event);
	for_each(
		[timestamp](kompvter::FrameListener* listener)
		{
			listener->on_draw_frame(timestamp);
		});
}

void kompromat::FrameProcessor::process_draw_enabled(const paley::Event& event)
{
	float timestamp = get_timestamp(event);
	for_each(
		[timestamp](kompvter::FrameListener* listener)
		{
			listener->on_draw_enabled(timestamp);
		});
}

void kompromat::FrameProcessor::process_draw_disabled(const paley::Event& event)
{
	float timestamp = get_timestamp(event);
	for_each(
		[timestamp](kompvter::FrameListener* listener)
		{
			listener->on_draw_disabled(timestamp);
		});
}

float kompromat::FrameProcessor::get_timestamp(const paley::Event& event)
{
	float timestamp;
	{
		paley::EventField t;
		if (!event.get_root().try_get_field("timestamp", t) ||
			!t.try_get_single(timestamp))
		{
			assert(false);
			timestamp = 0.0f;
		}
	}
	return timestamp;
}
