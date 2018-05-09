// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_VERTEX_ARRAY_RESOURCE_HPP
#define TOUCHSTONE_GL_RESOURCES_VERTEX_ARRAY_RESOURCE_HPP

#include <unordered_map>
#include <unordered_set>
#include <glm/glm.hpp>
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/resource.hpp"
#include "touchstone/gl/resources/vertexAttribute.hpp"

namespace touchstone
{
	class VertexArrayResource : public Resource
	{
	public:
		VertexArrayResource(Context* context, GLuint name);
		~VertexArrayResource() = default;

		void enable(GLuint index);
		void disable(GLuint index);

		void set_vertex_pointer(
			GLuint index,
			GLsizei num_components,
			GLenum format, GLboolean normalized, bool is_integral,
			GLsizei stride, std::size_t offset);
		void set_vertex_value(
			GLuint index,
			GLsizei num_components,
			GLenum format, GLboolean normalized, bool is_integral,
			GLdouble x, GLdouble y, GLdouble z, GLdouble w);
		void set_vertex_value(
			GLuint index,
			GLsizei num_components,
			GLenum format, GLboolean normalized, bool is_integral,
			GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		void set_vertex_value(
			GLuint index,
			GLsizei num_components,
			GLenum format, GLboolean normalized, bool is_integral,
			GLint x, GLint y, GLint z, GLint w);
		void set_vertex_value(
			GLuint index,
			GLsizei num_components,
			GLenum format, GLboolean normalized, bool is_integral,
			GLuint x, GLuint y, GLuint z, GLuint w);
		void set_vertex_value(
			GLuint index,
			GLsizei num_components, GLenum format,
			GLuint value);

		bool has_vertex_attribute(GLuint index) const;
		VertexAttribute get_vertex_attribute(GLuint index) const;
		bool is_vertex_attribute_enabled(GLuint index) const;

		void set_element_array_buffer(GLuint name);
		bool has_element_array_buffer() const;
		GLuint get_element_array_buffer() const;

	private:
		std::unordered_map<GLuint, VertexAttribute> attributes;
		std::unordered_set<GLuint> enabled;

		GLuint element_array_buffer = 0;
	};
}

#endif
