// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdio>
#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/resources/bufferBindingManager.hpp"

void touchstone::BufferBindingManager::bind(GLenum target, GLuint name)
{
	return bind(target, GLOBAL_INDEX, name);
}

void touchstone::BufferBindingManager::bind(
	GLenum target, GLint index, GLuint name)
{
	return BindingManager::bind(make_binding_target(target, index), name);
}

void touchstone::BufferBindingManager::unbind(
	GLenum target, GLint index)
{
	return BindingManager::unbind(make_binding_target(target, index));
}

bool touchstone::BufferBindingManager::is_resource_bound(
	GLenum target, GLint index) const
{
	return BindingManager::is_resource_bound(make_binding_target(target, index));
}

GLuint touchstone::BufferBindingManager::get_current_name(
	GLenum target, GLint index) const
{
	return BindingManager::get_current_name(make_binding_target(target, index));
}

touchstone::BufferBindingTarget
touchstone::BufferBindingManager::make_binding_target(GLenum target, GLint index)
{
	return std::make_tuple(target, index);
}
