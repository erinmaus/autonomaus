// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_GRAPHICS_VERTEX_INDEX_MAPPER_HPP
#define KOMPVTER_GRAPHICS_VERTEX_INDEX_MAPPER_HPP

#include <cstddef>
#include <cstdint>
#include "kompvter/graphics/buffer.hpp"

namespace kompvter
{
	class Buffer;

	class VertexIndexMapper
	{
	public:
		enum
		{
			format_none = 0,
			format_unsigned_byte,
			format_unsigned_short,
			format_unsigned_integer
		};

		VertexIndexMapper() = default;
		VertexIndexMapper(Buffer buffer, int format);
		~VertexIndexMapper() = default;

		bool lookup(std::size_t index, std::size_t& result) const;
		std::size_t get_num_indices() const;

		const Buffer& get_buffer() const;
		int get_format() const;
		std::size_t get_index_size() const;

		operator bool() const;

	private:
		Buffer buffer;
		const std::uint8_t* data;

		int format = format_none;
		std::size_t index_size = 0;

		static std::size_t get_index_size(int format);
	};
}

#endif
