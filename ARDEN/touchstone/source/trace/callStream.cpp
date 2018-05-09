// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "touchstone/trace/callStream.hpp"

const std::size_t touchstone::CallStream::MIN_CALL_SIZE;
const std::uint32_t touchstone::CallStream::TOKEN_NONE;

std::size_t touchstone::CallStream::Slice::data_size()
{
	auto data = get_data(alignof(std::uint32_t));
	auto size = *(const std::uint32_t*)data;

	advance(sizeof(std::uint32_t), alignof(std::uint32_t));

	return size;
}

const std::uint8_t* touchstone::CallStream::Slice::data(std::size_t size)
{
	if (size == 0)
	{
		return nullptr;
	}

	auto data = get_data(alignof(std::uint32_t));
	advance(size, alignof(std::uint8_t));

	return data;
}

std::size_t touchstone::CallStream::Slice::data_array_count()
{
	auto data = get_data(alignof(std::uint32_t));
	auto count = *(const std::uint32_t*)data;

	advance(sizeof(std::uint32_t), alignof(std::uint32_t));

	return count;
}

const std::uint8_t* touchstone::CallStream::Slice::data_array(std::size_t count)
{
	auto data = get_data(alignof(std::uint32_t));
	for (std::uint32_t i = 0; i < count; ++i)
	{
		auto length = *(const std::uint32_t*)get_data(alignof(std::uint32_t));

		advance(sizeof(std::uint32_t) + length, alignof(std::uint32_t));
	}

	if (count == 0)
	{
		return nullptr;
	}

	return data + sizeof(std::uint32_t);
}

touchstone::CallStream::Slice::Slice(
	const CallStream* stream,
	std::size_t start_offset,
	std::uint32_t call_token)
{
	this->stream = stream;
	this->current_offset = start_offset;
	this->call_token = call_token;
}

void touchstone::CallStream::Slice::advance(std::size_t count, std::size_t alignment)
{
	std::size_t aligned_offset;
	std::size_t alignment_remainder = this->current_offset % alignment;
	if (alignment_remainder != 0)
	{
		aligned_offset = (alignment - alignment_remainder);
	}
	else
	{
		aligned_offset = 0;
	}

	this->current_offset += aligned_offset + count;
	assert(this->current_offset <= this->stream->get_frame_data_size());
}

const std::uint8_t* touchstone::CallStream::Slice::get_data(std::size_t alignment) const
{
	std::size_t aligned_offset;
	std::size_t alignment_remainder = this->current_offset % alignment;
	if (alignment_remainder != 0)
	{
		aligned_offset = this->current_offset + (alignment - alignment_remainder);
	}
	else
	{
		aligned_offset = this->current_offset;
	}

	return this->stream->get_frame_data() + aligned_offset;
}

touchstone::CallStream::Slice::operator bool()
{
	return this->current_offset < this->stream->get_frame_data_size();
}

touchstone::CallStream::CallStream(
	const std::uint8_t* frame_data,
	std::size_t frame_data_size)
{
	this->frame_data = frame_data;
	this->frame_data_size = frame_data_size;

	reset();
}

const std::uint8_t* touchstone::CallStream::get_frame_data() const
{
	return this->frame_data;
}

std::size_t touchstone::CallStream::get_current_call_offset() const
{
	std::size_t aligned_offset;
	std::size_t alignment_remainder = this->offset % alignof(std::max_align_t);
	if (alignment_remainder != 0)
	{
		aligned_offset = this->offset + (alignof(std::max_align_t) - alignment_remainder);
	}
	else
	{
		aligned_offset = this->offset;
	}

	return aligned_offset;
}

std::size_t touchstone::CallStream::get_frame_data_size() const
{
	return this->frame_data_size;
}

std::uint32_t touchstone::CallStream::get_current_token() const
{
	std::size_t aligned_offset = get_current_call_offset();
	if (aligned_offset + MIN_CALL_SIZE <= get_frame_data_size())
	{
		return *((const std::uint32_t*)(get_frame_data() + aligned_offset));
	}

	return TOKEN_NONE;
}

void touchstone::CallStream::seek(std::size_t offset)
{
	assert(offset <= this->frame_data_size);
	this->offset = offset;
}

std::size_t touchstone::CallStream::tell() const
{
	return this->offset;
}

bool touchstone::CallStream::is_end_of_stream() const
{
	assert(this->offset <= this->frame_data_size);
	return this->offset == this->frame_data_size;
}

touchstone::CallStream::Slice touchstone::CallStream::begin(
	std::uint32_t call_token)
{
	assert(get_current_token() == call_token);
	assert(!is_end_of_stream());

	return Slice(this, get_current_call_offset() + sizeof(std::uint32_t), call_token);
}

std::size_t touchstone::CallStream::end(Slice& slice)
{
	assert(!is_end_of_stream());
	assert(get_current_token() == slice.call_token);

	auto result_offset = slice.current_offset + sizeof(std::uint32_t);
	auto size = result_offset - get_current_call_offset();
	this->offset = result_offset;

	assert(this->offset <= this->frame_data_size);
	auto s = size;

	return s;
}

void touchstone::CallStream::reset()
{
	this->offset = 0;
}
