// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_DISPATCH_HPP
#define TOUCHSTONE_GL_DISPATCH_HPP

#include "touchstone/trace/callStream.hpp"
#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/glTrace.hpp"

touchstone::Call* touchstone::GLTrace::read_call(
	std::size_t call_num, CallStream& stream)
{
	auto token = stream.get_current_token();

	switch (token)
	{
#include "glTrace.cpp.inl"
	}

	assert(false);
	return nullptr;
}

#endif
