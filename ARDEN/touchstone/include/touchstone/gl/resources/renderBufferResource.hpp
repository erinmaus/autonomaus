// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_RENDER_BUFFER_RESOURCE_HPP
#define TOUCHSTONE_GL_RESOURCES_RENDER_BUFFER_RESOURCE_HPP

#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/resource.hpp"

namespace touchstone
{
	class RenderBufferResource : public Resource
	{
	public:
		RenderBufferResource(Context* context, GLuint name);
		~RenderBufferResource() = default;

		void storage(
			GLenum internal_format, GLuint samples,
			GLsizei width, GLsizei height);

		GLenum get_internal_format() const;
		GLuint get_samples() const;
		GLsizei get_width() const;
		GLsizei get_height() const;

	private:
		GLenum internal_format = GL_NONE;
		GLuint samples = 0;
		GLsizei width = 0, height = 0;
	};
}

#endif
