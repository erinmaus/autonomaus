// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "celia/common/buffer.hpp"

celia::Buffer::Buffer(Buffer&& other)
{
	*this = std::move(other);
}

const std::uint8_t* celia::Buffer::get_data() const
{
	return &this->data[0];
}

std::uint8_t* celia::Buffer::get_data()
{
	return &this->data[0];
}

std::size_t celia::Buffer::get_data_size() const
{
	return this->data.size();
}

std::size_t celia::Buffer::get_offset() const
{
	return this->offset;
}

void celia::Buffer::reset_offset()
{
	this->offset = 0;
}

celia::Buffer& celia::Buffer::operator =(Buffer&& other)
{
	this->data = std::move(other.data);
	this->offset = other.offset;

	other.offset = 0;

	return *this;
}
