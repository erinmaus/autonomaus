// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_TEXTURE_HPP
#define AUTONOMAUS_GRAPHICS_TEXTURE_HPP

#include <cstdint>
#include "autonomaus/graphics/renderer.hpp"

namespace autonomaus
{
	class Texture
	{
	public:
		Texture(Renderer& renderer);
		~Texture();

		int get_width() const;
		int get_height() const;

		// Assumes RGBA
		void set_pixels(int width, int height, const std::uint8_t* pixels);

		void bind();

	private:
		GLuint texture;
		int width = 0, height = 0;

		Renderer* renderer;
		touchstone::ImmediateDispatch* gl;
	};
}

#endif
