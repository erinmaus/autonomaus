// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_IMMEDIATE_DISPATCH_HPP
#define TOUCHSTONE_GL_IMMEDIATE_DISPATCH_HPP

#include "touchstone/gl/types.hpp"
#include "touchstone/gl/dispatch.hpp"

namespace touchstone
{
	class ImmediateDispatch : public Dispatch
	{
	public:
		ImmediateDispatch() = default;
		~ImmediateDispatch() = default;

		void unload();

		static void* load_gl_function(const char* name);

#include "touchstone/gl/immediateDispatch.hpp.inl"
	};
}

#endif
