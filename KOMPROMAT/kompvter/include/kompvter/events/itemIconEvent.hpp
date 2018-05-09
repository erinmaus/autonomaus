// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_EVENTS_ITEM_ICON_EVENT_HPP
#define KOMPVTER_EVENTS_ITEM_ICON_EVENT_HPP

#include "kompvter/graphics/texture.hpp"

namespace kompvter
{
	class ItemIconEvent
	{
	public:
		ItemIconEvent() = default;
		~ItemIconEvent() = default;

		const Texture& get_texture() const;
		void set_texture(const Texture& value);

		int get_texture_name() const;
		void set_texture_name(int value);

		const Texture& get_source_texture() const;
		void set_source_texture(const Texture& value);

		int get_source_texture_name() const;
		void set_source_texture_name(int value);

		const Texture& get_destination_texture() const;
		void set_destination_texture(const Texture& value);

		int get_destination_texture_name() const;
		void set_destination_texture_name(int value);

	private:
		Texture source;
		Texture destination;
		int source_name = 0, destination_name = 0;
	};
}

#endif
