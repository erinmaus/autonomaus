// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_EVENTS_ITEM_ICON_EVENT_BUILDER_HPP
#define KOMPROMAT_EVENTS_ITEM_ICON_EVENT_BUILDER_HPP

#include "kompvter/events/itemIconEvent.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "paley/event/event.hpp"

namespace kompromat
{
	class ItemIconEventBuilder
	{
	public:
		ItemIconEventBuilder(
			kompvter::TextureManager& texture_manager);
		~ItemIconEventBuilder() = default;

		const kompvter::ItemIconEvent& get_event() const;

		void from_transfer_item_icon_texture(const paley::Event& event);
		void from_clear_item_icon_texture(const paley::Event& event);

	private:
		kompvter::ItemIconEvent event;
		kompvter::TextureManager& texture_manager;
	};
}

#endif
