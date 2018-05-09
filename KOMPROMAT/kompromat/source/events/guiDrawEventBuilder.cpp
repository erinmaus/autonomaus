// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompromat/events/guiDrawEventBuilder.hpp"

kompromat::GUIDrawEventBuilder::GUIDrawEventBuilder(
	kompvter::BufferManager& buffer_manager,
	kompvter::TextureManager& texture_manager)
	: DrawEventBuilder(this->event),
	buffer_manager(buffer_manager),
	texture_manager(texture_manager)
{
	// Nada.
}

const kompvter::GUIDrawEvent& kompromat::GUIDrawEventBuilder::get_event() const
{
	return this->event;
}

void kompromat::GUIDrawEventBuilder::from_draw_gui(const paley::Event& e)
{
	process(e, this->buffer_manager);
	process_gui_fields(e);
}

void kompromat::GUIDrawEventBuilder::process_gui_fields(const paley::Event& e)
{
	int atlas_name;
	{
		paley::EventField t;
		if (!e.get_root().try_get_field("texture", t) ||
			!t.try_get_integer(atlas_name))
		{
			assert(false);
			atlas_name = 0;
		}
	}
	event.set_atlas_name(atlas_name);
	event.set_atlas(texture_manager.get_texture(atlas_name));

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
	event.set_framebuffer(framebuffer);

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

	event.set_color_attachment_name(color_attachment_name);
	event.set_color_attachment(this->texture_manager.get_texture(color_attachment_name));
}
