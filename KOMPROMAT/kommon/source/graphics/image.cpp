// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdlib>
#include <algorithm>
#include <png.h>
#include "kommon/graphics/image.hpp"

kommon::Image::Image()
{
	this->width = 0;
	this->height = 0;
}

kommon::Image::Image(int width, int height)
{
	this->width = width;
	this->height = height;
	pixels.resize(width * height, glm::u8vec4(0));
}

int kommon::Image::get_width() const
{
	return this->width;
}

int kommon::Image::get_height() const
{
	return this->height;
}

std::uint8_t* kommon::Image::get_pixels()
{
	return &this->pixels[0].x;
}

const std::uint8_t* kommon::Image::get_pixels() const
{
	return &this->pixels[0].x;
}

std::size_t kommon::Image::get_pixels_size() const
{
	return this->pixels.size() * sizeof(glm::u8vec4);
}

glm::vec4 kommon::Image::get_pixel(int x, int y) const
{
	if (x >= 0 && y >= 0 && x < this->width && y < this->height)
	{
		auto& pixel = pixels[y * this->height + x];
		return glm::vec4(pixel) / 255.0f;
	}
	else
	{
		return glm::vec4(0.0f);
	}
}

void kommon::Image::set_pixel(int x, int y, const glm::vec4& value)
{
	if (x >= 0 && y >= 0 && x < this->width && y < this->height)
	{
		pixels[y * this->width + x] = glm::u8vec4(value * 255.0f);
	}
}

void kommon::Image::draw_line(
	int x1, int y1, int x2, int y2,
	const glm::vec4& color)
{
	if (x1 == x2)
	{
		int min_y = std::min(y1, y2);
		int max_y = std::max(y1, y2);
		for (int y = min_y; y <= max_y; ++y)
		{
			set_pixel(x1, y, color);
		}
	}
	else if (y1 == y2)
	{
		int min_x = std::min(x1, x2);
		int max_x = std::max(x1, x2);
		for (int x = min_x; x <= max_x; ++x)
		{
			set_pixel(x, y1, color);
		}
	}
	else
	{
		int delta_x = std::abs(x2 - x1);
		int sign_x = x1 < x2 ? 1 : -1;
		int delta_y = std::abs(y2 - y1);
		int sign_y = y1 < y2 ? 1 : -1;
		int delta_error = (delta_x > delta_y ? delta_x : -delta_y) / 2;

		int y = y1;
		int x = x1;
		while (x != x2 && y != y2)
		{
			set_pixel(x, y, color);

			int error = delta_error;
			if (error > -delta_x)
			{
				delta_error -= delta_y;
				x += sign_x;
			}
			if (error < delta_y)
			{
				delta_error += delta_x;
				y += sign_y;
			}
		}
	}
}

void kommon::Image::save_png(const std::string& filename) const
{
#ifdef PNG_IMAGE_VERSION
	png_image image = {};
	image.version = PNG_IMAGE_VERSION;
	image.width = this->width;
	image.height = this->height;
	image.format = PNG_FORMAT_RGBA;

	png_image_write_to_file(
		&image, filename.c_str(), false,
		&pixels[0].r, sizeof(glm::u8vec4) * this->width, nullptr);
	png_image_free(&image);
#endif
}

void kommon::Image::load_png(const std::string& filename)
{
#ifdef PNG_IMAGE_VERSION
	png_image image = {};
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image, filename.c_str()) != 0)
	{
		image.format = PNG_FORMAT_RGBA;
		this->width = image.width;
		this->height = image.height;
		this->pixels.resize(this->width * this->height);

		png_image_finish_read(
			&image, nullptr,
			&this->pixels[0].r, sizeof(glm::u8vec4) * this->width, nullptr);
		png_image_free(&image);
	}
#endif
}
