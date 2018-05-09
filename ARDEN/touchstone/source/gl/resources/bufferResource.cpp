// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/gl/resources/bufferResource.hpp"

touchstone::BufferResource::BufferResource(Context* context, GLuint name) :
	Resource(context, name)
{
	// Nothing.
}

GLsizeiptr touchstone::BufferResource::get_size() const
{
	return this->size;
}

void touchstone::BufferResource::set_size(GLsizeiptr value)
{
	this->size = value;
}
