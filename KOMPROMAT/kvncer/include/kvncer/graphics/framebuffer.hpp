// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVNCER_GRAPHICS_FRAMEBUFFER_HPP
#define KVNCER_GRAPHICS_FRAMEBUFFER_HPP

#include <cstddef>
#include <cstdint>
#include "kvncer/render/gl.hpp"

namespace kvncer
{
	class Texture;

	class Framebuffer
	{
	public:
		Framebuffer(Texture* color_attachment, bool depth);
		~Framebuffer();

		void bind();

		GLuint get_framebuffer() const;

		GLuint get_depth_attachment() const;

	private:
		GLuint framebuffer;
		GLuint depth_attachment = 0;
		Texture* color_attachment;
	};
}

#endif
