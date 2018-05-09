// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/graphics/texture.hpp"

autonomaus::Texture::Texture(Renderer& renderer)
{
	this->renderer = &renderer;
	this->gl = this->renderer->dispatch;

	gl->GenTextures(1, &this->texture);
}

autonomaus::Texture::~Texture()
{
	gl->DeleteTextures(1, &this->texture);
}

int autonomaus::Texture::get_width() const
{
	return this->width;
}

int autonomaus::Texture::get_height() const
{
	return this->height;
}

void autonomaus::Texture::set_pixels(
	int width, int height, const std::uint8_t* pixels)
{
	auto previous = this->renderer->current_texture;
	this->renderer->bind_texture(this->texture);
	gl->TexImage2D(
		GL_TEXTURE_2D, 0, GL_RGBA,
		width, height, 0,
		GL_RGBA, GL_UNSIGNED_BYTE, pixels);
	gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	gl->TexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	this->renderer->bind_texture(previous);
}

void autonomaus::Texture::bind()
{
	this->renderer->bind_texture(this->texture);
}
