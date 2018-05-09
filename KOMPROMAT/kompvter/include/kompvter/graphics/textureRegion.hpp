// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_GRAPHICS_TEXTURE_REGION_HPP
#define KOMPVTER_GRAPHICS_TEXTURE_REGION_HPP

#include <cstddef>
#include <cstdint>
#include "kompvter/graphics/buffer.hpp"

namespace kompvter
{
	class Texture;

	class TextureRegion
	{
	public:
		TextureRegion();
		TextureRegion(
			int name,
			int x, int y,
			int width, int height,
			int pixel_format, int pixel_type, bool is_compressed,
			const std::uint8_t* pixels_data, std::size_t pixels_size);
		virtual ~TextureRegion() = default;

		int get_name() const;

		int get_pixel_format() const;
		int get_pixel_type() const;
		bool get_is_compressed() const;

		int get_x() const;
		int get_y() const;
		int get_width() const;
		int get_height() const;

		const std::uint8_t* get_pixels_data() const;
		std::size_t get_pixels_size() const;

		// Requires 'buffer' to be RGBA.
		void get_pixels(
			int x, int y, int width, int height,
			std::uint8_t* buffer, std::size_t buffer_row_stride) const;

		// Assumes 'buffer_row_stride' is width * height * 4.
		void get_pixels(
			int x, int y, int width, int height,
			std::uint8_t* buffer) const;

	private:
		static const int NUM_BUFFER_COMPONENTS = 4;
		static const int DXT_BLOCK_WIDTH = 4;
		static const int DXT_BLOCK_HEIGHT = 4;
		static const int DXT_BLOCK_COMPONENTS = 4;
		static const int DXT_OUTPUT_BLOCK_SIZE =
			DXT_BLOCK_WIDTH * DXT_BLOCK_HEIGHT * DXT_BLOCK_COMPONENTS;
		static const int DXT_OUTPUT_BLOCK_STRIDE =
			DXT_BLOCK_WIDTH * DXT_BLOCK_COMPONENTS;
		static const int DXT1_INPUT_BLOCK_SIZE = 8;
		static const int DXT3_INPUT_BLOCK_SIZE = 16;
		static const int DXT5_INPUT_BLOCK_SIZE = 16;
		void get_pixels_compressed(
			int x, int y, int width, int height,
			std::uint8_t* buffer, std::size_t buffer_row_stride) const;
		void get_pixels_uncompressed(
			int x, int y, int width, int height,
			std::uint8_t* buffer, std::size_t buffer_row_stride) const;

		void uncompress_block(
			int input_column, int input_row,
			int output_x, int output_y,
			int output_width, int output_height,
			std::uint8_t* output_pixels, std::size_t output_pixels_stride) const;

		int name;

		int pixel_format, pixel_type;
		bool is_compressed;

		int x, y, width, height;
		Buffer pixels;
	};
}

#endif
