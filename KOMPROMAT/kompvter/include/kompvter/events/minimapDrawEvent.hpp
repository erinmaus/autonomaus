// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_EVENTS_MINIMAP_DRAW_EVENT_HPP
#define KOMPVTER_EVENTS_MINIMAP_DRAW_EVENT_HPP

#include "kompvter/events/drawEvent.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/texture.hpp"
#include "kompvter/graphics/textureManager.hpp"

namespace kompvter
{
	class MinimapDrawEvent : public DrawEvent
	{
	public:
		MinimapDrawEvent(const DrawEvent& base_draw_event);
		MinimapDrawEvent() = default;
		~MinimapDrawEvent() = default;

		int get_framebuffer() const;
		void set_framebuffer(int value);

		const Texture& get_color_attachment() const;
		void set_color_attachment(const Texture& value);

		int get_color_attachment_name() const;
		void set_color_attachment_name(int value);

	private:
		int framebuffer = 0;
		Texture color_attachment;
		int color_attachment_name = 0;
	};
}

#endif
