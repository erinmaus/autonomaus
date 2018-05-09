// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdint>
#include <cstdlib>
#include "mysterimaus/scan.hpp"

bool autonomaus::Scan::scan(std::int64_t value, int size, int operation)
{
	if (!ready)
	{
		for (auto i = mysterimaus::get_head_allocation(); i != nullptr; i = i->next)
		{
			std::size_t offset = 0;

			Record record;
			while (scan(i, offset, i->size - offset, value, size, operation, record))
			{
				allocations.push_back(record);
				offset = record.get_offset() + 1;
			}
		}

		ready = true;
	}
	else
	{
		for (auto i = allocations.begin(); i != allocations.end(); /* Nop. */)
		{
			Record r;
			if (i->get_allocation()->ref_count == 1 || !scan(i->get_allocation(), i->get_offset(), size, value, size, operation, r))
			{
				i = allocations.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	return !allocations.empty();
}

bool autonomaus::Scan::empty() const
{
	return allocations.empty();
}

std::size_t autonomaus::Scan::count() const
{
	return allocations.size();
}

autonomaus::Record autonomaus::Scan::get(std::size_t index) const
{
	if (index < allocations.size())
	{
		return allocations[index];
	}
	
	return Record();
}

void autonomaus::Scan::reset()
{
	ready = false;
	allocations.clear();
}

bool autonomaus::Scan::scan(
	mysterimaus::Allocation* allocation,
	std::size_t offset,
	std::size_t search_size,
	std::int64_t value,
	int value_size,
	int operation,
	Record& record)
{
	if (offset + search_size > allocation->size)
	{
		return false;
	}

	if (value_size < 1)
	{
		return false;
	}

	const char* p = ((const char*)allocation->pointer) + offset;
	for (std::size_t i = 0; i + value_size <= search_size; ++i)
	{
		std::int64_t v;
		switch (value_size)
		{
			case 1:
				v = *(p + i);
				break;
			case 2:
				v = *(const std::int16_t*)(p + i);
				break;
			case 4:
				v = *(const std::int32_t*)(p + i);
				break;
			case 8:
				v = *(const std::int64_t*)(p + i);
			default:
				return false;
		}

		bool r;
		switch (operation)
		{
			case scan_less:
				r = (v < value);
				break;
			case scan_less_equal:
				r = (v <= value);
				break;
			case scan_greater:
				r = (v > value);
				break;
			case scan_greater_equal:
				r = (v >= value);
				break;
			case scan_not_equal:
				r = (v != value);
				break;
			case scan_equal:
				r = (v == value);
				break;
			default:
				return false;
		}

		if (r)
		{
			record = Record(allocation, offset + i);
			return true;
		}
	}

	return false;
}
