// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#ifndef KOMPVTER_GRAPHICS_BUFFER_HPP
#define KOMPVTER_GRAPHICS_BUFFER_HPP

#include <cstdint>
#include <memory>
#include "kompvter/graphics/bufferData.hpp"

namespace kompvter
{
	class Buffer
	{
	public:
		explicit Buffer(int name = 0);
		Buffer(int name, const std::shared_ptr<BufferData>& data);
		~Buffer() = default;

		void allocate(std::size_t size);
		void update(
			std::size_t offset, const std::uint8_t* data, std::size_t size);
		void clear();

		int get_name() const;

		const std::uint8_t* get_data() const;
		std::size_t get_data_size() const;

		operator bool() const;

	private:
		int name;

		typedef std::shared_ptr<BufferData> DataPointer;
		DataPointer data;
	};
}

#endif
