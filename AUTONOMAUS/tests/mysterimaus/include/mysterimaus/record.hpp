// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_MYSTERIMAUS_RECORD_HPP
#define AUTONOMAUS_MYSTERIMAUS_RECORD_HPP

#include "mysterimaus/mysterimaus.hpp"

namespace autonomaus
{
	class Record
	{
	public:
		Record() = default;
		Record(const Record& other);
		Record(mysterimaus::Allocation* allocation, std::size_t offset);
		~Record();

		mysterimaus::Allocation* get_allocation() const;
		std::size_t get_offset() const;

		const void* get() const;

		Record& operator = (const Record& other);

	private:
		mysterimaus::Allocation* allocation = nullptr;
		std::size_t offset = 0;
	};
}

#endif
