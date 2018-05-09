// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompvter/events/viewportEvent.hpp"

int kompvter::ViewportEvent::get_x() const
{
	return this->x;
}

void kompvter::ViewportEvent::set_x(int value)
{
	this->x = value;
}

int kompvter::ViewportEvent::get_y() const
{
	return this->y;
}

void kompvter::ViewportEvent::set_y(int value)
{
	this->y = value;
}

int kompvter::ViewportEvent::get_width() const
{
	return this->width;
}

void kompvter::ViewportEvent::set_width(int value)
{
	this->width = value;
}

int kompvter::ViewportEvent::get_height() const
{
	return this->height;
}

void kompvter::ViewportEvent::set_height(int value)
{
	this->height = value;
}
