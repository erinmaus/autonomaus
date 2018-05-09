// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/gl/resources/renderBufferResource.hpp"

touchstone::RenderBufferResource::RenderBufferResource(
	Context* context, GLuint name) :
		Resource(context, name)
{
	// Nada.
}

void touchstone::RenderBufferResource::storage(
	GLenum internal_format, GLuint samples,
	GLsizei width, GLsizei height)
{
	this->internal_format = internal_format;
	this->samples = samples;
	this->width = width;
	this->height = height;
}

GLenum touchstone::RenderBufferResource::get_internal_format() const
{
	return this->internal_format;
}

GLuint touchstone::RenderBufferResource::get_samples() const
{
	return this->samples;
}

GLsizei touchstone::RenderBufferResource::get_width() const
{
	return this->width;
}

GLsizei touchstone::RenderBufferResource::get_height() const
{
	return this->height;
}
