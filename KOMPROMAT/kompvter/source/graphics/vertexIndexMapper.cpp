// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompvter/graphics/buffer.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"

kompvter::VertexIndexMapper::VertexIndexMapper(Buffer buffer, int format)
{
	this->buffer = buffer;
	this->data = buffer.get_data();
	this->format = format;
	this->index_size = get_index_size(format);
}

bool kompvter::VertexIndexMapper::lookup(
	std::size_t index, std::size_t& result) const
{
	if (index > get_num_indices())
	{
		return false;
	}

	auto d = this->data + index * index_size;
	switch(this->format)
	{
		case format_unsigned_byte:
			result = *d;
			break;
		case format_unsigned_short:
			result = *((const std::uint16_t*)d);
			break;
		case format_unsigned_integer:
			result = *((const std::uint32_t*)d);
			break;
		default:
			assert(false);
			return false;
	}

	return true;
}

std::size_t kompvter::VertexIndexMapper::get_num_indices() const
{
	if (this->buffer)
	{
		return this->buffer.get_data_size() / this->index_size;
	}

	return 0;
}

const kompvter::Buffer& kompvter::VertexIndexMapper::get_buffer() const
{
	return this->buffer;
}

int kompvter::VertexIndexMapper::get_format() const
{
	return this->format;
}

std::size_t kompvter::VertexIndexMapper::get_index_size() const
{
	return this->index_size;
}

kompvter::VertexIndexMapper::operator bool() const
{
	return this->buffer;
}

std::size_t kompvter::VertexIndexMapper::get_index_size(int format)
{
	switch(format)
	{
		case format_unsigned_byte:
			return sizeof(std::uint8_t);
			break;
		case format_unsigned_short:
			return sizeof(std::uint16_t);
			break;
		case format_unsigned_integer:
			return sizeof(std::uint32_t);
			break;
		default:
			assert(false);
			return 0;
	}
}
