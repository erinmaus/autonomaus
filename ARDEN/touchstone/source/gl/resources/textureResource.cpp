// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/gl/resources/textureResource.hpp"

touchstone::TextureResource::TextureResource(Context* context, GLuint name) :
	Resource(context, name)
{
	// Nil.
}

void touchstone::TextureResource::create_level(
	GLenum target, GLuint level,
	GLenum internal_format, GLsizei width, GLsizei height, GLsizei depth)
{
	auto& l = this->levels[std::make_tuple(target, level)];
	l.internal_format = internal_format;
	l.width = width;
	l.height = height;
	l.depth = depth;

	this->last_target = target;
}

GLenum touchstone::TextureResource::get_internal_format(GLuint level) const
{
	return get_internal_format(level, this->last_target);
}

GLenum touchstone::TextureResource::get_internal_format(
	GLuint level, GLenum target) const
{
	auto iter = this->levels.find(std::make_tuple(target, level));
	if (iter != this->levels.end())
	{
		return iter->second.internal_format;
	}

	return GL_NONE;
}

GLsizei touchstone::TextureResource::get_width(GLuint level) const
{
	return get_width(level, this->last_target);
}

GLsizei touchstone::TextureResource::get_width(
	GLuint level, GLenum target) const
{
	auto iter = this->levels.find(std::make_tuple(target, level));
	if (iter != this->levels.end())
	{
		return iter->second.width;
	}

	return 0;
}

GLsizei touchstone::TextureResource::get_height(GLuint level) const
{
	return get_height(level, this->last_target);
}

GLsizei touchstone::TextureResource::get_height(
	GLuint level, GLenum target) const
{
	auto iter = this->levels.find(std::make_tuple(target, level));
	if (iter != this->levels.end())
	{
		return iter->second.height;
	}

	return 0;
}

GLsizei touchstone::TextureResource::get_depth(
	GLuint level, GLenum target) const
{
	auto iter = this->levels.find(std::make_tuple(target, level));
	if (iter != this->levels.end())
	{
		return iter->second.depth;
	}

	return 0;
}

GLsizei touchstone::TextureResource::get_depth(GLuint level) const
{
	return get_depth(level, this->last_target);
}
