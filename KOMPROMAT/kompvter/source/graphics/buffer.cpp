// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#include <cassert>
#include <cstring>
#include "kompvter/graphics/buffer.hpp"

kompvter::Buffer::Buffer(int name)
{
	this->name = name;
}

kompvter::Buffer::Buffer(int name, const std::shared_ptr<BufferData>& data) :
	data(data)
{
	this->name = name;
}

void kompvter::Buffer::allocate(std::size_t size)
{
	this->data = DataPointer(new BufferData(size));
}

void kompvter::Buffer::update(
	std::size_t offset, const std::uint8_t* data, std::size_t size)
{
	this->data = DataPointer(new BufferData(*this->data.get()));
	this->data->update(offset, data, size);
}

void kompvter::Buffer::clear()
{
	this->data = DataPointer(new BufferData(get_data_size()));
}

int kompvter::Buffer::get_name() const
{
	return this->name;
}

const std::uint8_t* kompvter::Buffer::get_data() const
{
	if (this->data.get() == nullptr)
	{
		return nullptr;
	}

	return this->data->get_data();
}

std::size_t kompvter::Buffer::get_data_size() const
{
	if (this->data.get() == nullptr)
	{
		return 0;
	}

	return this->data->get_data_size();
}

kompvter::Buffer::operator bool() const
{
	return this->data.get() != nullptr;
}
