// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvncer/graphics/meshBuffer.hpp"

kvncer::MeshBuffer::MeshBuffer()
{
	glGenBuffers(1, &this->buffer);
}

kvncer::MeshBuffer::~MeshBuffer()
{
	glDeleteBuffers(1, &this->buffer);
}

const kompvter::Buffer& kvncer::MeshBuffer::get_buffer() const
{
	return this->data;
}

void kvncer::MeshBuffer::set_buffer(const kompvter::Buffer& buffer)
{
	this->data = buffer;
	this->is_dirty = true;
}

void kvncer::MeshBuffer::update(GLenum target)
{
	if (this->is_dirty)
	{
		glBindBuffer(target, this->buffer);
		if (this->data.get_data_size() == 0)
		{
			glBufferData(target, 0, nullptr, GL_STATIC_DRAW);
		}
		else
		{
			glBufferData(
				target,
				this->data.get_data_size(), this->data.get_data(),
				GL_STATIC_DRAW);
		}
		this->is_dirty = false;
	}

}

void kvncer::MeshBuffer::bind(GLenum target)
{
	if (this->is_dirty)
	{
		update(target);
	}
	else
	{
		glBindBuffer(target, this->buffer);
	}
}

bool kvncer::MeshBuffer::get_is_dirty() const
{
	return this->is_dirty;
}
