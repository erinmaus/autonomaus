// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_GL_TRACE_HPP
#define TOUCHSTONE_GL_GL_TRACE_HPP

#include "touchstone/trace/trace.hpp"

namespace touchstone
{
	class GLTrace : public Trace
	{
	public:
		GLTrace() = default;
		~GLTrace() = default;

	protected:
		Call* read_call(std::size_t call_num, CallStream& stream) override;
	};
}

#endif
