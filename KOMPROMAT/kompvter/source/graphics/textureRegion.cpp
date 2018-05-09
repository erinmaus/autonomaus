// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cmath>
#include <limits>
#include <algorithm>
#include <glm/glm.hpp>
#include "kompvter/deps/squish/squish.h"
#include "kompvter/events/textureEvent.hpp"
#include "kompvter/graphics/textureRegion.hpp"

kompvter::TextureRegion::TextureRegion()
{
	this->name = 0;
	this->pixel_format = 0;
	this->pixel_type = 0;
	this->is_compressed = false;
	this->x = 0;
	this->y = 0;
	this->width = 0;
	this->height = 0;
}

kompvter::TextureRegion::TextureRegion(
	int name,
	int x, int y,
	int width, int height,
	int pixel_format, int pixel_type, bool is_compressed,
	const std::uint8_t* pixels_data, std::size_t pixels_size)
{
	this->name = name;

	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;

	this->pixel_format = pixel_format;
	this->pixel_type = pixel_type;
	this->is_compressed = is_compressed;

	if (pixels_data != nullptr && pixels_size > 0)
	{
		this->pixels.allocate(pixels_size);
		this->pixels.update(0, pixels_data, pixels_size);
	}
}

int kompvter::TextureRegion::get_name() const
{
	return this->name;
}

int kompvter::TextureRegion::get_pixel_format() const
{
	return this->pixel_format;
}

int kompvter::TextureRegion::get_pixel_type() const
{
	return this->pixel_type;
}

bool kompvter::TextureRegion::get_is_compressed() const
{
	return this->is_compressed;
}

int kompvter::TextureRegion::get_x() const
{
	return this->x;
}

int kompvter::TextureRegion::get_y() const
{
	return this->y;
}

int kompvter::TextureRegion::get_width() const
{
	return this->width;
}

int kompvter::TextureRegion::get_height() const
{
	return this->height;
}

const std::uint8_t* kompvter::TextureRegion::get_pixels_data() const
{
	return this->pixels.get_data();
}

std::size_t kompvter::TextureRegion::get_pixels_size() const
{
	return this->pixels.get_data_size();
}

void kompvter::TextureRegion::get_pixels(
	int x, int y, int width, int height,
	std::uint8_t* buffer) const
{
	get_pixels(x, y, width, height, buffer, width * NUM_BUFFER_COMPONENTS);
}

void kompvter::TextureRegion::get_pixels(
	int x, int y, int width, int height,
	std::uint8_t* buffer, std::size_t buffer_row_stride) const
{
	if (this->is_compressed)
	{
		get_pixels_compressed(
			x, y, width, height,
			buffer, buffer_row_stride);
	}
	else
	{
		get_pixels_uncompressed(
			x, y, width, height,
			buffer, buffer_row_stride);
	}
}

void kompvter::TextureRegion::get_pixels_compressed(
	int x, int y, int width, int height,
	std::uint8_t* buffer, std::size_t buffer_row_stride) const
{
	static_assert(
		NUM_BUFFER_COMPONENTS == DXT_BLOCK_COMPONENTS,
		"component mismatch");

	assert(this->is_compressed);
	assert(this->width % DXT_BLOCK_WIDTH == 0);
	assert(this->height % DXT_BLOCK_HEIGHT == 0);
	assert(x >= 0);
	assert(y >= 0);
	assert(x + width <= this->width);
	assert(y + height <= this->height);

	auto compressed_pixels = get_pixels_data();
	assert(compressed_pixels != nullptr);

	int right = x + width;
	int bottom = y + height;

	int j = y;
	int t = 0;
	int current_row = y / DXT_BLOCK_HEIGHT;
	int current_block_y = current_row * DXT_BLOCK_HEIGHT;
	while (j < bottom)
	{
		int local_y = j - current_block_y;
		int local_height = std::min(DXT_BLOCK_HEIGHT - local_y, bottom - j);

		int i = x;
		int s = 0;
		int current_column = x / DXT_BLOCK_WIDTH;
		int current_block_x = current_column * DXT_BLOCK_WIDTH;

		while (i < right)
		{
			int local_x = i - current_block_x;
			int local_width = std::min(DXT_BLOCK_WIDTH - local_x, right - i);

			auto output_pixel = buffer + t * buffer_row_stride + s * NUM_BUFFER_COMPONENTS;
			uncompress_block(
				current_column, current_row,
				local_x, local_y, local_width, local_height,
				output_pixel, buffer_row_stride);

			i += local_width;
			s += local_width;
			current_block_x += DXT_BLOCK_WIDTH;
			++current_column;
		}

		j += local_height;
		t += local_height;
		current_block_y += DXT_BLOCK_HEIGHT;
		++current_row;
	}
}


void kompvter::TextureRegion::get_pixels_uncompressed(
	int x, int y, int width, int height,
	std::uint8_t* buffer, std::size_t buffer_row_stride) const
{
	assert(!this->is_compressed);
	assert(x >= 0);
	assert(y >= 0);
	assert(x + width <= this->width);
	assert(y + height <= this->height);

	auto input_pixels = get_pixels_data();
	assert(input_pixels != nullptr);

	std::size_t num_components;
	switch (this->pixel_format)
	{
		case TextureEvent::component_format_r:
			num_components = 1;
			break;
		case TextureEvent::component_format_rg:
			num_components = 2;
			break;
		case TextureEvent::component_format_rgb:
			num_components = 3;
			break;
		case TextureEvent::component_format_rgba:
			num_components = 4;
			break;
		default:
			assert(false);
			return;
	}

	std::size_t component_size;
	switch (this->pixel_type)
	{
		case TextureEvent::component_type_unsigned_byte:
			component_size = sizeof(std::uint8_t);
			break;
		case TextureEvent::component_type_unsigned_short:
			component_size = sizeof(std::uint16_t);
			break;
		case TextureEvent::component_type_unsigned_integer:
			component_size = sizeof(std::uint32_t);
			break;
		case TextureEvent::component_type_float:
			component_size = sizeof(float);
			break;
		default:
			assert(false);
			return;
	}

	const std::size_t PIXEL_SIZE = component_size * num_components;
	const std::size_t INPUT_ROW_STRIDE = PIXEL_SIZE * this->width;
	auto current_input_row = input_pixels + INPUT_ROW_STRIDE * y;
	auto current_output_row = buffer;
	for (int j = 0; j < height; ++j)
	{
		for (int i = 0; i < width; ++i)
		{
			int current_x = x + i;
			auto input_pixel = current_input_row;
			input_pixel += PIXEL_SIZE * current_x;
			auto output_pixel = current_output_row;
			output_pixel += NUM_BUFFER_COMPONENTS * i;

			for (int k = 0; k < num_components; ++k)
			{
				switch (this->pixel_type)
				{
					case TextureEvent::component_type_unsigned_byte:
						*output_pixel = *input_pixel;
						break;
					case TextureEvent::component_type_unsigned_short:
						*output_pixel = *((const std::uint16_t*)input_pixel) / std::numeric_limits<std::uint16_t>::max();
						break;
					case TextureEvent::component_type_unsigned_integer:
						*output_pixel = *((const std::uint32_t*)input_pixel) / std::numeric_limits<std::uint16_t>::max();
						break;
					case TextureEvent::component_type_float:
						component_size = glm::clamp(*((const float*)input_pixel), 0.0f, 1.0f) * 0xff;
						break;
					default:
						// If this ever asserts we've really screwed the pooch.
						assert(false);
						return;
				}

				++output_pixel;
				++input_pixel;
			}

			for (int k = num_components; k < NUM_BUFFER_COMPONENTS; ++k)
			{
				*output_pixel++ = 0;
			}
		}

		current_output_row += buffer_row_stride;
		current_input_row += INPUT_ROW_STRIDE;
	}
}

void kompvter::TextureRegion::uncompress_block(
	int input_column, int input_row,
	int output_x, int output_y,
	int output_width, int output_height,
	std::uint8_t* output_pixels, std::size_t output_pixels_stride) const
{
	assert(this->is_compressed);
	assert(input_column < this->width / DXT_BLOCK_WIDTH);
	assert(input_row < this->height / DXT_BLOCK_HEIGHT);

	std::size_t block_size;
	switch (this->pixel_format)
	{
		case kompvter::TextureEvent::compression_rgb_dxt1:
		case kompvter::TextureEvent::compression_srgb_dxt1:
		case kompvter::TextureEvent::compression_rgba_dxt1:
		case kompvter::TextureEvent::compression_srgba_dxt1:
			block_size = DXT1_INPUT_BLOCK_SIZE;
			break;
		case kompvter::TextureEvent::compression_rgba_dxt3:
		case kompvter::TextureEvent::compression_srgba_dxt3:
			block_size = DXT3_INPUT_BLOCK_SIZE;
			break;
		case kompvter::TextureEvent::compression_rgba_dxt5:
		case kompvter::TextureEvent::compression_srgba_dxt5:
			block_size = DXT5_INPUT_BLOCK_SIZE;
			break;
		default:
			assert(false);
			break;
	}

	int num_columns = this->width / DXT_BLOCK_WIDTH;
	std::uint8_t uncompressed_block[DXT_OUTPUT_BLOCK_SIZE] = { 0 };

	std::size_t offset = (input_row * num_columns + input_column) * block_size;
	assert(offset < get_pixels_size());

	auto input_block = get_pixels_data();
	assert(input_block != nullptr);

	input_block += offset;

	switch (this->pixel_format)
	{
		case kompvter::TextureEvent::compression_srgb_dxt1:
		case kompvter::TextureEvent::compression_rgb_dxt1:
		case kompvter::TextureEvent::compression_srgba_dxt1:
		case kompvter::TextureEvent::compression_rgba_dxt1:
			squish::Decompress(uncompressed_block, input_block, squish::kDxt1);
			break;
		case kompvter::TextureEvent::compression_srgba_dxt3:
		case kompvter::TextureEvent::compression_rgba_dxt3:
			squish::Decompress(uncompressed_block, input_block, squish::kDxt3);
			break;
		case kompvter::TextureEvent::compression_srgba_dxt5:
		case kompvter::TextureEvent::compression_rgba_dxt5:
			squish::Decompress(uncompressed_block, input_block, squish::kDxt5);
			break;
		default:
			assert(false);
			break;
	}

	for (int j = output_y; j < output_height; ++j)
	{
		auto output_pixel = output_pixels + j * output_pixels_stride + output_x * NUM_BUFFER_COMPONENTS;
		auto input_pixel = uncompressed_block + j * DXT_OUTPUT_BLOCK_STRIDE + output_x * NUM_BUFFER_COMPONENTS;

		for (int i = output_x; i < output_width; ++i)
		{
			for (int k = 0; k < NUM_BUFFER_COMPONENTS; ++k)
			{
				*(output_pixel++) = *(input_pixel++);
			}
		}
	}
}
