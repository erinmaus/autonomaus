// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_LISTENERS_BUFFER_LISTENER_HPP
#define KOMPVTER_LISTENERS_BUFFER_LISTENER_HPP

#include <memory>
#include <unordered_map>

namespace kompvter
{
	class BufferEvent;

	class BufferListener
	{
	public:
		BufferListener() = default;
		virtual ~BufferListener() = default;

		virtual void on_buffer_create(const BufferEvent& event);
		virtual void on_buffer_delete(const BufferEvent& event);
		virtual void on_buffer_allocate(const BufferEvent& event);
		virtual void on_buffer_clear(const BufferEvent& event);
		virtual void on_buffer_update(const BufferEvent& event);
	};
}

#endif
