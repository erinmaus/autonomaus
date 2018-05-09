// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/gl/resources/vertexArrayResource.hpp"

touchstone::VertexArrayResource::VertexArrayResource(
	Context* context, GLuint name) :
		Resource(context, name)
{
	// Nope.
}

void touchstone::VertexArrayResource::enable(GLuint index)
{
	this->enabled.insert(index);
}

void touchstone::VertexArrayResource::disable(GLuint index)
{
	this->enabled.erase(index);
}

bool touchstone::VertexArrayResource::is_vertex_attribute_enabled(
	GLuint index) const
{
	return this->enabled.count(index) != 0;
}

void touchstone::VertexArrayResource::set_vertex_pointer(
	GLuint index,
	GLsizei num_components, GLenum format, GLboolean normalized, bool is_integral,
	GLsizei stride, std::size_t offset)
{
	auto& attribute = this->attributes[index];
	attribute.buffer = get_context()->get_current_resource<BufferResource>(GL_ARRAY_BUFFER).get_name();
	attribute.num_components = num_components;
	attribute.format = format;
	attribute.normalized = normalized;
	attribute.is_integral = is_integral;
	attribute.has_value = false;
	attribute.stride = stride;
	attribute.offset = offset;
}

void touchstone::VertexArrayResource::set_vertex_value(
	GLuint index,
	GLsizei num_components,
	GLenum format, GLboolean normalized, bool is_integral,
	GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
	auto& attribute = this->attributes[index];
	attribute.buffer = 0;
	attribute.num_components = num_components;
	attribute.format = format;
	attribute.normalized = normalized;
	attribute.is_integral = is_integral;
	attribute.value.single = glm::vec4(x, y, z, w);
	attribute.has_value = true;
	attribute.stride = 0;
	attribute.offset = 0;
}

void touchstone::VertexArrayResource::set_vertex_value(
	GLuint index,
	GLsizei num_components,
	GLenum format, GLboolean normalized, bool is_integral,
	GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	auto& attribute = this->attributes[index];
	attribute.buffer = 0;
	attribute.num_components = num_components;
	attribute.format = format;
	attribute.normalized = normalized;
	attribute.is_integral = is_integral;
	attribute.value.single = glm::vec4(x, y, z, w);
	attribute.has_value = true;
	attribute.stride = 0;
	attribute.offset = 0;
}

void touchstone::VertexArrayResource::set_vertex_value(
	GLuint index,
	GLsizei num_components,
	GLenum format, GLboolean normalized, bool is_integral,
	GLint x, GLint y, GLint z, GLint w)
{
	auto& attribute = this->attributes[index];
	attribute.buffer = 0;
	attribute.num_components = num_components;
	attribute.format = format;
	attribute.normalized = normalized;
	attribute.is_integral = is_integral;
	attribute.value.integer = glm::ivec4(x, y, z, w);
	attribute.has_value = true;
	attribute.stride = 0;
	attribute.offset = 0;
}

void touchstone::VertexArrayResource::set_vertex_value(
	GLuint index,
	GLsizei num_components,
	GLenum format, GLboolean normalized, bool is_integral,
	GLuint x, GLuint y, GLuint z, GLuint w)
{
	auto& attribute = this->attributes[index];
	attribute.buffer = 0;
	attribute.num_components = num_components;
	attribute.format = format;
	attribute.normalized = normalized;
	attribute.is_integral = is_integral;
	attribute.value.unsigned_integer = glm::uvec4(x, y, z, w);
	attribute.has_value = true;
	attribute.stride = 0;
	attribute.offset = 0;
}

void touchstone::VertexArrayResource::set_vertex_value(
	GLuint index,
	GLsizei num_components, GLenum format,
	GLuint value)
{
	auto& attribute = this->attributes[index];
	attribute.buffer = 0;
	attribute.num_components = num_components;
	attribute.format = format;
	attribute.normalized = GL_FALSE;
	attribute.is_integral = false;
	attribute.value.packed = value;
	attribute.has_value = true;
	attribute.stride = 0;
	attribute.offset = 0;
}

bool touchstone::VertexArrayResource::has_vertex_attribute(GLuint index) const
{
	return this->attributes.count(index) != 0;
}

touchstone::VertexAttribute touchstone::VertexArrayResource::get_vertex_attribute(
	GLuint index) const
{
	assert(has_vertex_attribute(index));
	return this->attributes.find(index)->second;
}

void touchstone::VertexArrayResource::set_element_array_buffer( GLuint name)
{
	this->element_array_buffer = name;
}

bool touchstone::VertexArrayResource::has_element_array_buffer() const
{
	return this->element_array_buffer != 0;
}

GLuint touchstone::VertexArrayResource::get_element_array_buffer() const
{
	return this->element_array_buffer;
}
