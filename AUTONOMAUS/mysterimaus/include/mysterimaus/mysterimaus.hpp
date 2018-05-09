// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MYSTERIMAUS_MYSTERIMAUS_HPP
#define MYSTERIMAUS_MYSTERIMAUS_HPP

#include <cstddef>

namespace mysterimaus
{
	std::size_t get_num_allocations();

	static const int MAX_BACKTRACE = 4;

	struct Allocation
	{
		void* pointer;
		void* allocation;
		std::size_t size;
		int ref_count;
		Allocation* next;
		Allocation* previous;
		const void* backtrace[MAX_BACKTRACE];
	};

	Allocation* get_head_allocation();
	Allocation* get_tail_allocation();
	Allocation* get_allocation(void* pointer);
}

#endif
