// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_EVENTS_MINIMAP_DRAW_EVENT_BUILDER_HPP
#define KOMPROMAT_EVENTS_MINIMAP_DRAW_EVENT_BUILDER_HPP

#include "kompromat/events/drawEventBuilder.hpp"
#include "kompvter/events/minimapDrawEvent.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"

namespace kompromat
{
	class MinimapDrawEventBuilder : public DrawEventBuilder
	{
	public:
		MinimapDrawEventBuilder(
			kompvter::BufferManager& buffer_manager,
			kompvter::TextureManager& texture_manager);
		~MinimapDrawEventBuilder() = default;

		const kompvter::MinimapDrawEvent& get_event() const;

		void from_draw_minimap(const paley::Event& e);

	private:
		void process_minimap_fields(const paley::Event& e);

		kompvter::MinimapDrawEvent event;
		kompvter::BufferManager& buffer_manager;
		kompvter::TextureManager& texture_manager;
	};
}

#endif
