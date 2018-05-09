// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/graphics/mesh.hpp"
#include "autonomaus/graphics/program.hpp"

autonomaus::Mesh::Mesh(Renderer& renderer)
{
	this->renderer = &renderer;
	this->gl = this->renderer->dispatch;

	gl->GenBuffers(1, &this->position_buffer);
	gl->GenBuffers(1, &this->texture_buffer);
	gl->GenBuffers(1, &this->color_buffer);
	gl->GenBuffers(1, &this->index_buffer);
	gl->GenVertexArrays(1, &this->vertex_array_object);
}

autonomaus::Mesh::~Mesh()
{
	gl->DeleteBuffers(1, &this->position_buffer);
	gl->DeleteBuffers(1, &this->texture_buffer);
	gl->DeleteBuffers(1, &this->color_buffer);
	gl->DeleteBuffers(1, &this->index_buffer);
	gl->DeleteVertexArrays(1, &this->vertex_array_object);
}

std::size_t autonomaus::Mesh::get_num_vertices() const
{
	return this->num_vertices;
}

void autonomaus::Mesh::set_num_vertices(std::size_t count)
{
	this->num_vertices = count;
}

void autonomaus::Mesh::set_positions(const glm::vec3 value[])
{
	this->renderer->bind_vertex_array(this->vertex_array_object);
	if (value == nullptr)
	{
		gl->DisableVertexAttribArray(Program::vertex_input_position);
	}
	else
	{
		gl->EnableVertexAttribArray(Program::vertex_input_position);
		this->renderer->bind_array_buffer(this->position_buffer);
		gl->BufferData(
			GL_ARRAY_BUFFER,
			this->num_vertices * sizeof(glm::vec3), value,
			GL_DYNAMIC_DRAW);
		gl->VertexAttribPointer(
			Program::vertex_input_position,
			3, GL_FLOAT, GL_FALSE,
			0, 0);
	}
}

void autonomaus::Mesh::set_textures(const glm::vec2 value[])
{
	this->renderer->bind_vertex_array(this->vertex_array_object);
	if (value == nullptr)
	{
		gl->DisableVertexAttribArray(Program::vertex_input_texture);
	}
	else
	{
		gl->EnableVertexAttribArray(Program::vertex_input_texture);
		this->renderer->bind_array_buffer(this->texture_buffer);
		gl->BufferData(
			GL_ARRAY_BUFFER,
			this->num_vertices * sizeof(glm::vec2), value,
			GL_DYNAMIC_DRAW);
		gl->VertexAttribPointer(
			Program::vertex_input_texture,
			2, GL_FLOAT, GL_FALSE,
			0, 0);
	}
}

void autonomaus::Mesh::set_colors(const glm::vec4 value[])
{
	this->renderer->bind_vertex_array(this->vertex_array_object);
	if (value == nullptr)
	{
		gl->DisableVertexAttribArray(Program::vertex_input_color);
	}
	else
	{
		gl->EnableVertexAttribArray(Program::vertex_input_color);
		this->renderer->bind_array_buffer(this->color_buffer);
		gl->BufferData(
			GL_ARRAY_BUFFER,
			this->num_vertices * sizeof(glm::vec4), value,
			GL_DYNAMIC_DRAW);
		gl->VertexAttribPointer(
			Program::vertex_input_color,
			4, GL_FLOAT, GL_FALSE,
			0, 0);
	}
}

std::size_t autonomaus::Mesh::get_num_indices() const
{
	return this->num_indices;
}

void autonomaus::Mesh::set_indices(
	const std::uint32_t value[], std::size_t count)
{
	this->renderer->bind_vertex_array(this->vertex_array_object);
	if (value == nullptr || count == 0)
	{
		this->renderer->bind_element_array_buffer(0);
		this->num_indices = 0;
	}
	else
	{
		this->num_indices = count;
		this->renderer->bind_element_array_buffer(this->index_buffer);
		gl->BufferData(
			GL_ELEMENT_ARRAY_BUFFER,
			sizeof(std::uint32_t) * count, value,
			GL_DYNAMIC_DRAW);
	}
}

void autonomaus::Mesh::draw(int primitive)
{
	if (this->num_indices == 0)
	{
		draw(primitive, 0, this->num_vertices);
	}
	else
	{
		draw(primitive, 0, this->num_indices);
	}
}

void autonomaus::Mesh::draw(int primitive, std::size_t start, std::size_t count)
{
	if (this->num_vertices == 0)
	{
		return;
	}

	GLenum gl_primitive;
	switch (primitive)
	{
		case primitive_triangles:
			gl_primitive = GL_TRIANGLES;
			break;
		case primitive_lines:
			gl_primitive = GL_LINES;
			break;
		case primitive_points:
			gl_primitive = GL_POINTS;
			break;
		default:
			return;
	}

	if (this->num_indices == 0)
	{
		if (start + count > this->num_vertices)
		{
			return;
		}

		this->renderer->bind_vertex_array(this->vertex_array_object);
		gl->DrawArrays(gl_primitive, start, count);
	}
	else
	{
		if (start + count > this->num_indices)
		{
			return;
		}

		this->renderer->bind_vertex_array(this->vertex_array_object);
		gl->DrawElements(
			gl_primitive, count,
			GL_UNSIGNED_INT, (void*)(start * sizeof(std::uint32_t)));
	}
}
