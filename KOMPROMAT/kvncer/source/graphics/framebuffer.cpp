// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kvncer/graphics/framebuffer.hpp"
#include "kvncer/graphics/texture.hpp"

kvncer::Framebuffer::Framebuffer(Texture* color_attachment, bool depth)
{
	assert(color_attachment != nullptr);

	glGenFramebuffers(1, &this->framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
	glFramebufferTexture2D(
		GL_FRAMEBUFFER,
		GL_COLOR_ATTACHMENT0,
		GL_TEXTURE_2D, color_attachment->get_texture(), 0);
	this->color_attachment = color_attachment;

	if (depth)
	{
		glGenTextures(1, &this->depth_attachment);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, this->depth_attachment);
		glTexImage2D(
			GL_TEXTURE_2D, 0,
			GL_DEPTH_COMPONENT32,
			color_attachment->get_width(), color_attachment->get_height(), 0,
			GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
		glFramebufferTexture2D(
			GL_FRAMEBUFFER,
			GL_DEPTH_ATTACHMENT,
			GL_TEXTURE_2D, this->depth_attachment, 0);
	}
}

kvncer::Framebuffer::~Framebuffer()
{
	glDeleteFramebuffers(1, &this->framebuffer);
	if (this->depth_attachment != 0)
	{
		glDeleteTextures(1, &this->depth_attachment);
	}
}

void kvncer::Framebuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->framebuffer);
}

GLuint kvncer::Framebuffer::get_framebuffer() const
{
	return this->framebuffer;
}

GLuint kvncer::Framebuffer::get_depth_attachment() const
{
	return this->depth_attachment;
}
