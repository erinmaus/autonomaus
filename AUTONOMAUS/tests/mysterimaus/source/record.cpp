// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdlib>
#include "mysterimaus/record.hpp"

autonomaus::Record::Record(const Record& other)
{
	*this = other;
}

autonomaus::Record::Record(
	mysterimaus::Allocation* allocation,
	std::size_t offset) :
	allocation(allocation),
	offset(offset)
{
	++allocation->ref_count;
}

autonomaus::Record::~Record()
{
	if (allocation != nullptr)
	{
		if  (allocation->ref_count == 1)
		{
			std::free(allocation->pointer);
		}
		else
		{
			--allocation->ref_count;
		}
	}
}

mysterimaus::Allocation* autonomaus::Record::get_allocation() const
{
	return allocation;
}

std::size_t autonomaus::Record::get_offset() const
{
	return offset;
}

const void* autonomaus::Record::get() const
{
	if (offset < allocation->size)
	{
		return ((const char*)allocation->pointer) + offset;
	}

	return nullptr;
}

autonomaus::Record& autonomaus::Record::operator =(const Record& other)
{
	if (allocation != nullptr)
	{
		if  (allocation->ref_count == 1)
		{
			std::free(allocation->pointer);
		}
		else
		{
			--allocation->ref_count;
		}
	}

	allocation = other.allocation;
	if (allocation != nullptr)
	{
		++allocation->ref_count;
	}

	offset = other.offset;

	return *this;
}
