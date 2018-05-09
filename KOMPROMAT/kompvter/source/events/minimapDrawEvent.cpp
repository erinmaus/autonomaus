// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/events/minimapDrawEvent.hpp"

kompvter::MinimapDrawEvent::MinimapDrawEvent(const DrawEvent& base_draw_event) :
	DrawEvent(base_draw_event)
{
	// No.
}

int kompvter::MinimapDrawEvent::get_framebuffer() const
{
	return this->framebuffer;
}

void kompvter::MinimapDrawEvent::set_framebuffer(int value)
{
	this->framebuffer = value;
}

const kompvter::Texture& kompvter::MinimapDrawEvent::get_color_attachment() const
{
	return this->color_attachment;
}

void kompvter::MinimapDrawEvent::set_color_attachment(const Texture& value)
{
	this->color_attachment = value;
}

int kompvter::MinimapDrawEvent::get_color_attachment_name() const
{
	return this->color_attachment_name;
}

void kompvter::MinimapDrawEvent::set_color_attachment_name(int value)
{
	this->color_attachment_name = value;
}
