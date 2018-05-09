// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_UNIFORM_HPP
#define TOUCHSTONE_GL_RESOURCES_UNIFORM_HPP

#include <glm/glm.hpp>
#include "touchstone/gl/types.hpp"

namespace touchstone
{
	struct Uniform
	{
		Uniform() = default;
		~Uniform() = default;

		enum
		{
			type_none = 0,
			type_single,
			type_integer,
			type_unsigned_integer,
			type_single_vec2,
			type_integer_vec2,
			type_unsigned_integer_vec2,
			type_single_vec3,
			type_integer_vec3,
			type_unsigned_integer_vec3,
			type_single_vec4,
			type_integer_vec4,
			type_unsigned_integer_vec4,
			type_matrix2,
			type_matrix2x3,
			type_matrix2x4,
			type_matrix3,
			type_matrix3x2,
			type_matrix3x4,
			type_matrix4,
			type_matrix4x2,
			type_matrix4x3,
			type_max
		};

		int type = type_none;

		enum
		{
			sampler_none = 0,
			sampler_1d,
			sampler_2d,
			sampler_3d,
			sampler_cube,
			sampler_2d_array,
			sampler_2d_rect,
			sampler_max
		};
		int sampler_type = sampler_none;

		union Value
		{
			GLfloat single;
			GLint integer;
			GLint sampler;
			GLuint unsigned_integer;
			glm::vec2 single_vec2;
			glm::ivec2 integer_vec2;
			glm::uvec2 unsigned_integer_vec2;
			glm::vec3 single_vec3;
			glm::ivec3 integer_vec3;
			glm::uvec3 unsigned_integer_vec3;
			glm::vec4 single_vec4;
			glm::ivec4 integer_vec4;
			glm::uvec4 unsigned_integer_vec4;
			glm::mat2 matrix2;
			glm::mat2x3 matrix2x3;
			glm::mat2x4 matrix2x4;
			glm::mat3 matrix3;
			glm::mat3x2 matrix3x2;
			glm::mat3x4 matrix3x4;
			glm::mat4 matrix4;
			glm::mat4x2 matrix4x2;
			glm::mat4x3 matrix4x3;
		};

		std::vector<Value> values;
	};
}

#endif
