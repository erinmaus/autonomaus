// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompromat/events/viewportEventBuilder.hpp"

const kompvter::ViewportEvent& kompromat::ViewportEventBuilder::get_event() const
{
	return this->event;
}

void kompromat::ViewportEventBuilder::from_set_viewport(
	const paley::Event& event)
{
	int viewport_x;
	{
		paley::EventField x;
		if (!event.get_root().try_get_field("x", x) ||
			!x.try_get_integer(viewport_x))
		{
			assert(false);
			viewport_x = 0;
		}
	}
	this->event.set_x(viewport_x);

	int viewport_y;
	{
		paley::EventField y;
		if (!event.get_root().try_get_field("y", y) ||
			!y.try_get_integer(viewport_y))
		{
			assert(false);
			viewport_y = 0;
		}
	}
	this->event.set_y(viewport_y);

	int viewport_width;
	{
		paley::EventField w;
		if (!event.get_root().try_get_field("width", w) ||
			!w.try_get_integer(viewport_width))
		{
			assert(false);
			viewport_width = 0;
		}
	}
	this->event.set_width(viewport_width);

	int viewport_height;
	{
		paley::EventField h;
		if (!event.get_root().try_get_field("height", h) ||
			!h.try_get_integer(viewport_height))
		{
			assert(false);
			viewport_height = 0;
		}
	}
	this->event.set_height(viewport_height);
}
