// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_VERTEX_ATTRIBUTE_HPP
#define TOUCHSTONE_GL_RESOURCES_VERTEX_ATTRIBUTE_HPP

#include <glm/glm.hpp>
#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/types.hpp"

namespace touchstone
{
	struct VertexAttribute
	{
		VertexAttribute() = default;
		VertexAttribute(const VertexAttribute& other) = default;
		~VertexAttribute() = default;

		GLuint buffer = 0;

		GLint num_components = 0;
		GLenum format = GL_NONE;
		GLboolean normalized = GL_FALSE;

		union Value
		{
			glm::vec4 single;
			glm::ivec4 integer;
			glm::uvec4 unsigned_integer;
			GLuint packed;
		};
		Value value = {};
		bool has_value = false;

		bool is_integral = false;

		GLsizei stride = 0;
		std::size_t offset = 0;

		VertexAttribute& operator =(const VertexAttribute& other) = default;
	};
}

#endif
