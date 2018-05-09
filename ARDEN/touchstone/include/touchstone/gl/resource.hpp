// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_TRACE_RESOURCE_HPP
#define TOUCHSTONE_TRACE_RESOURCE_HPP

#include "touchstone/gl/types.hpp"

namespace touchstone
{
	class Context;

	class Resource
	{
	public:
		explicit Resource(Context* context, GLuint name);
		virtual ~Resource() = default;

		GLuint get_name() const;

	protected:
		Context* get_context() const;

	private:
		GLuint name;
		Context* context;
	};
}

#endif
