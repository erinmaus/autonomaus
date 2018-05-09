// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPTVER_GRAPHICS_BUFFER_STATUS_HPP
#define KOMPTVER_GRAPHICS_BUFFER_STATUS_HPP

#include <unordered_map>
#include "kompvter/listeners/bufferListener.hpp"

namespace kompvter
{
	class BufferStatus : public BufferListener
	{
	public:
		BufferStatus() = default;
		~BufferStatus() = default;

		bool has_buffer(int name) const;

		enum
		{
			status_none = 0,
			status_clean,
			status_dirty
		};
		int get_buffer_status(int name) const;

		bool touch_buffer(int name);
		void touch_all();

		void on_buffer_create(const BufferEvent& event) override;
		void on_buffer_delete(const BufferEvent& event) override;
		void on_buffer_allocate(const BufferEvent& event) override;
		void on_buffer_clear(const BufferEvent& event) override;
		void on_buffer_update(const BufferEvent& event) override;

	private:
		std::unordered_map<int, int> buffers;
	};
}

#endif
