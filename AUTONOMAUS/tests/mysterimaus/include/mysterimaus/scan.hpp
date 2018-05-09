// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_MYSTERIMAUS_SCAN_HPP
#define AUTONOMAUS_MYSTERIMAUS_SCAN_HPP

#include <cstdint>
#include <deque>
#include "mysterimaus/mysterimaus.hpp"
#include "mysterimaus/record.hpp"

namespace autonomaus
{
	class Scan
	{
	public:
		Scan() = default;
		~Scan() = default;

		enum
		{
			scan_less,
			scan_less_equal,
			scan_greater,
			scan_greater_equal,
			scan_not_equal,
			scan_equal
		};

		bool scan(std::int64_t value, int size, int operation);

		bool empty() const;
		std::size_t count() const;

		Record get(std::size_t index) const;

		void reset();

	private:
		static bool scan(
			mysterimaus::Allocation* allocation,
			std::size_t offset,
			std::size_t search_size,
			std::int64_t value,
			int value_size,
			int operation,
			Record& record);

		bool ready = false;
		std::deque<Record> allocations;
	};
}

#endif
