// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_BUFFER_RESOURCE_HPP
#define TOUCHSTONE_GL_RESOURCES_BUFFER_RESOURCE_HPP

#include "touchstone/gl/types.hpp"
#include "touchstone/gl/resource.hpp"

namespace touchstone
{
	class BufferResource : public Resource
	{
	public:
		BufferResource(Context* context, GLuint name);
		~BufferResource() = default;

		GLsizeiptr get_size() const;
		void set_size(GLsizeiptr value);

	private:
		GLsizeiptr size = 0;
	};
}

#endif
