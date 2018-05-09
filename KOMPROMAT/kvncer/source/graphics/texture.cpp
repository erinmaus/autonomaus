// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/graphics/textureRegion.hpp"
#include "kvncer/graphics/texture.hpp"

kvncer::Texture::Texture(
	int format, bool is_compressed, int width, int height)
{
	glGenTextures(1, &this->texture);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	GLenum internal_format;
	switch (format)
	{
		default:
		case component_format_r:
			internal_format = GL_RED;
			break;
		case component_format_rg:
			internal_format = GL_RG;
			break;
		case component_format_rgb:
			internal_format = GL_RGB;
			break;
		case component_format_rgba:
			internal_format = GL_RGBA;
			break;
	}

	auto pixels = std::make_unique<std::uint8_t[]>(width * height * 4);
	glTexImage2D(
		GL_TEXTURE_2D, 0,
		format,
		width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());

	this->width = width;
	this->height = height;
}

kvncer::Texture::~Texture()
{
	glDeleteTextures(1, &this->texture);
}

void kvncer::Texture::update(
	int x, int y, int width, int height,
	int component_format, int component_type,
	const std::uint8_t* pixel_data)
{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);

	GLenum pixel_format;
	switch (component_format)
	{
		default:
		case component_format_r:
			pixel_format = GL_RED;
			break;
		case component_format_rg:
			pixel_format = GL_RG;
			break;
		case component_format_rgb:
			pixel_format = GL_RGB;
			break;
		case component_format_rgba:
			pixel_format = GL_RGBA;
			break;
	}

	GLenum pixel_type;
	switch (component_type)
	{
		default:
		case component_type_unsigned_byte:
			pixel_type = GL_UNSIGNED_BYTE;
			break;
		case component_type_unsigned_short:
			pixel_type = GL_UNSIGNED_SHORT;
			break;
		case component_type_unsigned_integer:
			pixel_type = GL_UNSIGNED_INT;
			break;
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexSubImage2D(
		GL_TEXTURE_2D, 0,
		x, y, width, height,
		pixel_format, pixel_type,
		pixel_data);
}

void kvncer::Texture::compressed_update(
	int x, int y, int width, int height, int compression_format,
	const std::uint8_t* pixel_data, std::size_t pixel_data_size)
{
	kompvter::TextureRegion region(
		0,
		0, 0, width, height,
		compression_format, kompvter::TextureEvent::component_format_none,
		true,
		pixel_data, pixel_data_size);
	auto pixels = std::make_unique<std::uint8_t[]>(width * height * 4);
	region.get_pixels(0, 0, width, height, pixels.get());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, this->texture);
	glTexSubImage2D(
		GL_TEXTURE_2D, 0,
		x, y, width, height,
		GL_RGBA, GL_UNSIGNED_BYTE, pixels.get());
}

void kvncer::Texture::bind(int unit)
{
	glActiveTexture(GL_TEXTURE0 + unit + 1);
	glBindTexture(GL_TEXTURE_2D, this->texture);
}

GLuint kvncer::Texture::get_texture() const
{
	return this->texture;
}

int kvncer::Texture::get_width() const
{
	return this->width;
}

int kvncer::Texture::get_height() const
{
	return this->height;
}
