// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_EVENTS_BUFFER_EVENT_BUILDER_HPP
#define KOMPROMAT_EVENTS_BUFFER_EVENT_BUILDER_HPP

#include <cstddef>
#include <cstdint>
#include "paley/event/event.hpp"
#include "kompvter/events/bufferEvent.hpp"

namespace kompromat
{
	class BufferEventBuilder
	{
	public:
		BufferEventBuilder() = default;
		~BufferEventBuilder() = default;

		const kompvter::BufferEvent& get_event() const;

		void from_buffer_create(const paley::Event& e);
		void from_buffer_delete(const paley::Event& e);
		void from_buffer_allocate(const paley::Event& e);
		void from_buffer_clear(const paley::Event& e);
		void from_buffer_update(const paley::Event& e);

	private:
		void process_name(const paley::Event& event);

		kompvter::BufferEvent event;
		paley::EventField event_data;
	};
}

#endif
