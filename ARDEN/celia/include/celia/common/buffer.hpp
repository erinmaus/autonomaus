// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CELIA_COMMON_BUFFER_HPP
#define CELIA_COMMON_BUFFER_HPP

#include <cstdint>
#include <cstring>
#include <algorithm>
#include <vector>

namespace celia
{
	class Buffer
	{
	public:
		Buffer() = default;
		Buffer(Buffer&& other);
		~Buffer() = default;

		template <typename Value>
		Value push(const Value& value);

		template <typename Value>
		Value* push_data(const Value* value, std::size_t length);

		template <typename Value>
		Value pop();

		template <typename Value>
		Value* pop_data(std::size_t length);

		const std::uint8_t* get_data() const;
		std::uint8_t* get_data();
		std::size_t get_data_size() const;

		std::size_t get_offset() const;
		void reset_offset();

		template <typename Value>
		void copy(Value* destination, std::size_t length);

		Buffer& operator =(Buffer&& other);

	private:
		std::vector<std::uint8_t> data;
		std::size_t offset = 0;
	};
}

template <typename Value>
Value celia::Buffer::push(const Value& value)
{
	auto v = ((const std::uint8_t*)&value);
	data.insert(data.end(), v, v + sizeof(Value));

	return value;
}

template <typename Value>
Value* celia::Buffer::push_data(const Value* value, std::size_t length)
{
	std::size_t old_size = data.size();
	data.resize(old_size + length);
	if (value != nullptr)
	{
		auto v = (const std::uint8_t*)value;
		std::copy(v, v + length, data.begin() + old_size);
	}

	return (Value*)(get_data() + old_size);
}

template <typename Value>
Value celia::Buffer::pop()
{
	auto result = *((Value*)(this->get_data() + offset));
	offset += sizeof(Value);

	return result;
}

template <typename Value>
Value* celia::Buffer::pop_data(std::size_t length)
{
	auto result = (Value*)(this->get_data() + offset);
	offset += length;

	return result;
}

template <typename Value>
void celia::Buffer::copy(Value* destination, std::size_t length)
{
	std::memcpy(destination, get_data() + offset, length);
}

#endif
