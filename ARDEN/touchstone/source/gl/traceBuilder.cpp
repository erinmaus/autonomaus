// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/gl/traceBuilder.hpp"

touchstone::TraceBuilder::TraceBuilder(ImmediateDispatch& dispatch)
	: gl(dispatch)
{
	// Nothing.
}

void touchstone::TraceBuilder::reset()
{
	this->call_stream_builder.reset();
}

touchstone::CallStream touchstone::TraceBuilder::get_call_stream() const
{
	return CallStream(
		this->call_stream_builder.get_data(),
		this->call_stream_builder.get_size());
}

const touchstone::CallStreamBuilder&
touchstone::TraceBuilder::get_call_stream_builder() const
{
	return this->call_stream_builder;
}

#include "traceBuilder.cpp.inl"
