// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_EVENTS_BUFFER_EVENT_HPP
#define KOMPVTER_EVENTS_BUFFER_EVENT_HPP

#include <cstddef>
#include <cstdint>

namespace kompvter
{
	class BufferEvent
	{
	public:
		BufferEvent() = default;
		~BufferEvent() = default;

		int get_name() const;
		void set_name(int value);

		std::size_t get_offset() const;
		void set_offset(std::size_t value);

		const std::uint8_t* get_data() const;
		void set_data(const std::uint8_t* value);

		std::size_t get_data_size() const;
		void set_data_size(std::size_t value);

	private:
		int name = 0;
		std::size_t offset = 0;
		const std::uint8_t* data = nullptr;
		std::size_t data_size = 0;
	};
}

#endif
