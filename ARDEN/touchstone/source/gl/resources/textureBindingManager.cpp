// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/gl/resources/textureBindingManager.hpp"

void touchstone::TextureBindingManager::set_active_unit(GLenum unit)
{
	this->active_unit = unit;
}

GLenum touchstone::TextureBindingManager::get_active_unit() const
{
	return this->active_unit;
}

void touchstone::TextureBindingManager::bind(GLenum target, GLuint name)
{
	BindingManager<TextureBindingTarget>::bind(
		make_binding_target(this->active_unit, target), name);
	assert(get_current_name(target) == name);
}

void touchstone::TextureBindingManager::unbind(GLenum target)
{
	BindingManager<TextureBindingTarget>::unbind(std::make_tuple(get_active_unit(), target));
}

bool touchstone::TextureBindingManager::is_resource_bound(GLenum target) const
{
	return is_resource_bound(target, get_active_unit());
}

bool touchstone::TextureBindingManager::is_resource_bound(
	GLenum target, GLenum unit) const
{
	return BindingManager<TextureBindingTarget>::is_resource_bound(
		make_binding_target(unit, target));
}

GLuint touchstone::TextureBindingManager::get_current_name(GLenum target) const
{
	return get_current_name(target, get_active_unit());
}

GLuint touchstone::TextureBindingManager::get_current_name(GLenum target, GLenum unit) const
{
	return BindingManager<TextureBindingTarget>::get_current_name(
		make_binding_target(unit, target));
}

touchstone::TextureBindingTarget touchstone::TextureBindingManager::make_binding_target(
	GLenum unit, GLenum target)
{
	GLenum real_target;
	switch (target)
	{
		case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
		case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
		case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
			real_target = GL_TEXTURE_CUBE_MAP;
			break;
		default:
			real_target = target;
	}

	return std::make_tuple(unit, real_target);
}
