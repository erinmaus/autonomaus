// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstring>
#include "kompvter/graphics/bufferData.hpp"

kompvter::BufferData::BufferData(std::size_t size)
{
	this->data = pool.allocate(size);
	this->data_size = size;

#ifndef NDEBUG
	std::memset(this->data, 0xff, this->data_size);
#endif
}

kompvter::BufferData::~BufferData()
{
	pool.deallocate(this->data, this->data_size);
}

kompvter::BufferData::BufferData(const BufferData& other)
{
	*this = other;
}

void kompvter::BufferData::update(
	std::size_t offset, const std::uint8_t* data, std::size_t size)
{
	assert(data != nullptr);
	assert(offset + size <= this->data_size);
	assert(this->data != nullptr);

	std::memcpy(this->data + offset, data, size);
}

void kompvter::BufferData::clear()
{
#ifndef NDEBUG
	std::memset(this->data, 0xff, this->data_size);
#else
	std::memset(this->data, 0x00, this->data_size);
#endif
}

const std::uint8_t* kompvter::BufferData::get_data() const
{
	return this->data;
}

std::uint8_t* kompvter::BufferData::get_data()
{
	return this->data;
}

std::size_t kompvter::BufferData::get_data_size() const
{
	return this->data_size;
}

kompvter::BufferData& kompvter::BufferData::operator =(
	const kompvter::BufferData& other)
{
	if (this->data_size != other.data_size)
	{
		pool.deallocate(this->data, this->data_size);
		this->data = pool.allocate(other.data_size);
		this->data_size = other.data_size;
	}
	assert(this->data != nullptr);

	update(0, other.data, other.data_size);

	return *this;
}

std::uint8_t* kompvter::BufferData::Pool::allocate(std::size_t size)
{
	std::lock_guard<std::mutex> lock(this->mutex);

	auto block_size = next_power_of_two(size);
	auto& queue = this->bins[block_size];
	if (queue.empty())
	{
		return new std::uint8_t[block_size];
	}
	else
	{
		auto result = queue.front();
		queue.pop();

		return result;
	}
}

void kompvter::BufferData::Pool::deallocate(std::uint8_t* data, std::size_t size)
{
	if (data != nullptr && size != 0)
	{
		std::lock_guard<std::mutex> lock(this->mutex);

		auto block_size = next_power_of_two(size);
		auto& queue = this->bins[block_size];
		queue.push(data);
	}
}

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(_BitScanForward64)  
#endif

std::size_t kompvter::BufferData::Pool::next_power_of_two(std::size_t value)
{
#ifdef _MSC_VER
	unsigned long index;
	if (_BitScanForward64(&index, value))
	{
		return 1ULL << (sizeof(uint64_t) * 8 - index);
	}
	else
	{
		assert(false);
	}
#else
	return 1ULL << (sizeof(uint64_t) * 8 - __builtin_clzll(value));
#endif
}

kompvter::BufferData::Pool kompvter::BufferData::pool;
