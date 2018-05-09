// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstdlib>
#include "kvncer/graphics/mesh.hpp"
#include "kvncer/graphics/meshBuffer.hpp"

kvncer::Mesh::Mesh()
{
	glGenVertexArrays(1, &this->vertex_array_object);
}

kvncer::Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &this->vertex_array_object);
}

void kvncer::Mesh::enable_vertex_attrib(
	GLuint index,
	MeshBuffer* buffer,
	GLint num_components, GLenum type, GLboolean normalized,
	std::size_t offset, std::size_t stride)
{
	glBindVertexArray(this->vertex_array_object);
	if (buffer == nullptr)
	{
		glDisableVertexAttribArray(index);
		this->vertex_buffers.erase(index);
	}
	else
	{
		buffer->bind(GL_ARRAY_BUFFER);
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(
			index,
			num_components, type, normalized,
			stride, (const void*)offset);
		this->vertex_buffers[index] = buffer;
	}
}

void kvncer::Mesh::set_indices(kvncer::MeshBuffer* buffer)
{
	glBindVertexArray(this->vertex_array_object);
	if (buffer == nullptr)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	else
	{
		buffer->bind(GL_ELEMENT_ARRAY_BUFFER);
	}

	this->index_format = GL_NONE;
	this->index_offset_start = 0;
	this->index_offset_stop = 0;
}

void kvncer::Mesh::set_indices(
	MeshBuffer* buffer, GLenum format, std::size_t start, std::size_t stop)
{
	glBindVertexArray(this->vertex_array_object);
	if (buffer == nullptr || format == GL_NONE)
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		this->index_format = GL_NONE;
	}
	else
	{
		buffer->bind(GL_ELEMENT_ARRAY_BUFFER);
		this->index_format = format;
	}

	this->index_offset_start = start;
	this->index_offset_stop = stop;
}

void kvncer::Mesh::draw(GLenum primitive)
{
	draw(
		this->index_format, this->index_offset_start, this->index_offset_stop,
		primitive);
}

void kvncer::Mesh::draw(
	GLenum format, std::size_t start, std::size_t stop, GLenum primitive)
{
	glBindVertexArray(this->vertex_array_object);

	for (auto& vertex_buffer: vertex_buffers)
	{
		if (vertex_buffer.second->get_is_dirty())
		{
			vertex_buffer.second->update(GL_ARRAY_BUFFER);
		}
	}

	if (this->index_buffer != nullptr && this->index_buffer->get_is_dirty())
	{
		this->index_buffer->update(GL_ELEMENT_ARRAY_BUFFER);
	}

	if (format == GL_NONE)
	{
		glDrawArrays(primitive, start, stop);
	}
	else
	{
		std::size_t index_size;
		switch (format)
		{
			case GL_UNSIGNED_BYTE:
				index_size = sizeof(std::uint8_t);
				break;
			case GL_UNSIGNED_SHORT:
				index_size = sizeof(std::uint16_t);
				break;
			case GL_UNSIGNED_INT:
				index_size = sizeof(std::uint32_t);
				break;
			default:
				assert(false);
				index_size = 0;
				break;
		}

		auto count = (stop - start) / index_size;
		glDrawElements(primitive, count, format, (const void*)start);
	}
}

GLuint kvncer::Mesh::get_vertex_array_object() const
{
	return this->vertex_array_object;
}
