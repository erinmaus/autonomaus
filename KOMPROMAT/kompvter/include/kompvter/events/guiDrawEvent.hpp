// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_EVENTS_GUI_DRAW_EVENT_HPP
#define KOMPVTER_EVENTS_GUI_DRAW_EVENT_HPP

#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/events/drawEvent.hpp"
#include "kompvter/graphics/texture.hpp"
#include "kompvter/graphics/textureManager.hpp"

namespace kompvter
{
	class GUIDrawEvent : public DrawEvent
	{
	public:
		GUIDrawEvent(const DrawEvent& base_draw_event);
		GUIDrawEvent() = default;
		~GUIDrawEvent() = default;

		const Texture& get_atlas() const;
		void set_atlas(const Texture& value);

		int get_atlas_name() const;
		void set_atlas_name(int value);

		int get_framebuffer() const;
		void set_framebuffer(int value);

		const Texture& get_color_attachment() const;
		void set_color_attachment(const Texture& value);

		int get_color_attachment_name() const;
		void set_color_attachment_name(int value);

	private:
		Texture atlas;
		int atlas_name = 0;

		int framebuffer = 0;
		Texture color_attachment;
		int color_attachment_name = 0;
	};
}

#endif
