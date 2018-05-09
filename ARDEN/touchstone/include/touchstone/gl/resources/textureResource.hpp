// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_TEXTURE_RESOURCE_HPP
#define TOUCHSTONE_GL_RESOURCES_TEXTURE_RESOURCE_HPP

#include <map>
#include <unordered_map>
#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/resource.hpp"

namespace touchstone
{
	class TextureResource : public Resource
	{
	public:
		TextureResource(Context* context, GLuint name);
		~TextureResource() = default;

		void create_level(
			GLenum target, GLuint level,
			GLenum internal_format,
			GLsizei width, GLsizei height, GLsizei depth);

		GLenum get_internal_format(GLuint level = 0) const;
		GLenum get_internal_format(GLuint level, GLenum target) const;
		GLsizei get_width(GLuint level = 0) const;
		GLsizei get_width(GLuint level, GLenum target) const;
		GLsizei get_height(GLuint level = 0) const;
		GLsizei get_height(GLuint level, GLenum target) const;
		GLsizei get_depth(GLuint level = 0) const;
		GLsizei get_depth(GLuint level, GLenum target) const;

	private:
		GLenum last_target = GL_NONE;

		typedef std::tuple<GLenum, GLuint> TargetLevelTuple;
		struct Level
		{
			GLenum internal_format;
			GLsizei width, height, depth;
		};
		std::map<TargetLevelTuple, Level> levels;
	};
}

#endif
