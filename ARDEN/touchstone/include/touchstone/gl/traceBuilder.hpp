// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_TRACE_BUILDER_HPP
#define TOUCHSTONE_GL_TRACE_BUILDER_HPP

#include "touchstone/gl/types.hpp"
#include "touchstone/gl/immediateDispatch.hpp"
#include "touchstone/trace/callStreamBuilder.hpp"

namespace touchstone
{
	class TraceBuilder
	{
	public:
		TraceBuilder(ImmediateDispatch& dispatch);
		~TraceBuilder() = default;

		void reset();

		CallStream get_call_stream() const;
		const CallStreamBuilder& get_call_stream_builder() const;

#include "touchstone/gl/traceBuilder.hpp.inl"

	private:
		ImmediateDispatch& gl;
		CallStreamBuilder call_stream_builder;
	};
}

#endif
