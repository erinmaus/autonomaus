// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_DISPATCH_HPP
#define TOUCHSTONE_GL_DISPATCH_HPP

#include "touchstone/gl/types.hpp"

namespace touchstone
{
	class Dispatch
	{
	public:
		Dispatch() = default;
		~Dispatch() = default;

#include "touchstone/gl/dispatch.hpp.inl"
	};
}

#endif
