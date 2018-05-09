// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include <cassert>
#include <algorithm>
#include "touchstone/trace/callStreamBuilder.hpp"

const std::size_t touchstone::CallStreamBuilder::INITIAL_SIZE;

touchstone::CallStreamBuilder::CallStreamBuilder()
{
	this->block = std::make_unique<std::uint8_t[]>(INITIAL_SIZE);
	this->block_size = INITIAL_SIZE;
	this->block_offset = 0;
}

void touchstone::CallStreamBuilder::append(
	std::size_t size, std::size_t alignment, const std::uint8_t* bytes)
{
	auto aligned_offset = get_offset(alignment);
	auto new_offset = aligned_offset + size;
	if (new_offset > this->block_size)
	{
		resize(new_offset);
	}

	std::memcpy(this->block.get() + aligned_offset, bytes, size);
	this->block_offset = new_offset;
}

void touchstone::CallStreamBuilder::reset()
{
	this->block_offset = 0;
}

const std::uint8_t* touchstone::CallStreamBuilder::get_data() const
{
	return this->block.get();
}

std::size_t touchstone::CallStreamBuilder::get_size() const
{
	return this->block_offset;
}

std::size_t touchstone::CallStreamBuilder::get_offset(
	std::size_t alignment) const
{
	std::size_t aligned_offset;
	std::size_t alignment_remainder = this->block_offset % alignment;
	if (alignment_remainder != 0)
	{
		aligned_offset = this->block_offset + (alignment - alignment_remainder);
	}
	else
	{
		aligned_offset = this->block_offset;
	}

	return aligned_offset;
}

void touchstone::CallStreamBuilder::resize(std::size_t minimum_size)
{
	std::size_t new_size = this->block_size * 2;
	while (new_size < minimum_size)
	{
		new_size *= 2;
	}

	printf("resize\n");

	auto new_block = new std::uint8_t[new_size];
	std::memcpy(new_block, get_data(), get_size());

	this->block = BlockPointer(new_block);
	this->block_size = new_size;
}
