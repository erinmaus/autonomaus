// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_TRACE_CALL_HPP
#define TOUCHSTONE_TRACE_CALL_HPP

#include <cstddef>
#include <cstdint>
#include "touchstone/gl/types.hpp"

namespace touchstone
{
	class CallStream;

	class Call
	{
	public:
		virtual ~Call() = default;

		virtual const char* get_name() const = 0;
		virtual std::uint32_t get_token() const = 0;
		virtual std::size_t get_call_num() const = 0;

		virtual const std::uint8_t* get_data() const = 0;
		virtual std::size_t get_size() const = 0;

		virtual void update(CallStream& stream) = 0;
		virtual Call* clone() const = 0;
	};
}

#endif
