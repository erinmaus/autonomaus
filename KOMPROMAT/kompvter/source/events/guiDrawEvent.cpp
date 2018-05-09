// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/events/guiDrawEvent.hpp"

kompvter::GUIDrawEvent::GUIDrawEvent(const DrawEvent& base_draw_event) :
	DrawEvent(base_draw_event)
{
	// No.
}

const kompvter::Texture& kompvter::GUIDrawEvent::get_atlas() const
{
	return this->atlas;
}

void kompvter::GUIDrawEvent::set_atlas(const Texture& value)
{
	this->atlas = value;
}

int kompvter::GUIDrawEvent::get_atlas_name() const
{
	return this->atlas_name;
}

void kompvter::GUIDrawEvent::set_atlas_name(int value)
{
	this->atlas_name = value;
}

int kompvter::GUIDrawEvent::get_framebuffer() const
{
	return this->framebuffer;
}

void kompvter::GUIDrawEvent::set_framebuffer(int value)
{
	this->framebuffer = value;
}

const kompvter::Texture& kompvter::GUIDrawEvent::get_color_attachment() const
{
	return this->color_attachment;
}

void kompvter::GUIDrawEvent::set_color_attachment(const Texture& value)
{
	this->color_attachment = value;
}

int kompvter::GUIDrawEvent::get_color_attachment_name() const
{
	return this->color_attachment_name;
}

void kompvter::GUIDrawEvent::set_color_attachment_name(int value)
{
	this->color_attachment_name = value;
}
