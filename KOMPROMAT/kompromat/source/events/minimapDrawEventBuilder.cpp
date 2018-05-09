// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompromat/events/minimapDrawEventBuilder.hpp"

kompromat::MinimapDrawEventBuilder::MinimapDrawEventBuilder(
	kompvter::BufferManager& buffer_manager,
	kompvter::TextureManager& texture_manager)
	: DrawEventBuilder(this->event),
	buffer_manager(buffer_manager),
	texture_manager(texture_manager)
{
	// Nope.
}

const kompvter::MinimapDrawEvent& kompromat::MinimapDrawEventBuilder::get_event() const
{
	return this->event;
}

void kompromat::MinimapDrawEventBuilder::from_draw_minimap(const paley::Event& e)
{
	process(e, buffer_manager);
	process_minimap_fields(e);
}

void kompromat::MinimapDrawEventBuilder::process_minimap_fields(
	const paley::Event& e)
{
	int framebuffer;
	{
		paley::EventField f;
		if (!e.get_root().try_get_field("framebuffer", f) ||
			!f.try_get_integer(framebuffer))
		{
			assert(false);
			framebuffer = 0;
		}
	}
	this->event.set_framebuffer(framebuffer);

	int color_attachment_name;
	if (framebuffer == 0)
	{
		color_attachment_name = 0;
	}
	else
	{
		paley::EventField c;
		if (!e.get_root().try_get_field("color_attachment", c) ||
			!c.try_get_integer(color_attachment_name))
		{
			assert(false);
			color_attachment_name = 0;
		}
	}
	this->event.set_color_attachment_name(color_attachment_name);
	this->event.set_color_attachment(
		this->texture_manager.get_texture(color_attachment_name));
}
