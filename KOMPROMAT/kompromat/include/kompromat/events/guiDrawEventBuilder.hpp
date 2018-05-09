// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_EVENTS_GUI_DRAW_EVENT_BUILDER_HPP
#define KOMPROMAT_EVENTS_GUI_DRAW_EVENT_BUILDER_HPP

#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "kompromat/events/drawEventBuilder.hpp"
#include "paley/event/event.hpp"

namespace kompromat
{
	class GUIDrawEventBuilder : public DrawEventBuilder
	{
	public:
		GUIDrawEventBuilder(
			kompvter::BufferManager& buffer_manager,
			kompvter::TextureManager& texture_manager);
		~GUIDrawEventBuilder() = default;

		const kompvter::GUIDrawEvent& get_event() const;

		void from_draw_gui(const paley::Event& e);

	private:
		void process_gui_fields(const paley::Event& e);

		kompvter::GUIDrawEvent event;
		kompvter::BufferManager& buffer_manager;
		kompvter::TextureManager& texture_manager;
	};
}

#endif
