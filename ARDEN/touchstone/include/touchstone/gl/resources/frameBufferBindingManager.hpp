// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_FRAME_BUFFER_BINDING_MANAGER_HPP
#define TOUCHSTONE_GL_RESOURCES_FRAME_BUFFER_BINDING_MANAGER_HPP

#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/bindingManager.hpp"

namespace touchstone
{
	class FrameBufferBindingManager : private BindingManager<GLenum>
	{
	public:
		FrameBufferBindingManager() = default;
		~FrameBufferBindingManager() = default;

		void bind(GLenum target, GLuint name);
		void unbind(GLenum target);

		bool is_resource_bound(GLenum target, GLenum alias) const;

		GLuint get_current_name(GLenum target, GLenum alias) const;

		using BindingManager::unbind_all;
		using BindingManager::is_resource_bound;
		using BindingManager::get_current_name;
	};
}

inline void touchstone::FrameBufferBindingManager::bind(GLenum target, GLuint name)
{
	if (target == GL_FRAMEBUFFER)
	{
		BindingManager::bind(GL_DRAW_FRAMEBUFFER, name);
		BindingManager::bind(GL_READ_FRAMEBUFFER, name);
	}
	else
	{
		BindingManager::bind(target, name);
	}
}

inline void touchstone::FrameBufferBindingManager::unbind(GLenum target)
{
	if (target == GL_FRAMEBUFFER)
	{
		BindingManager::unbind(GL_DRAW_FRAMEBUFFER);
		BindingManager::unbind(GL_READ_FRAMEBUFFER);
	}
	else
	{
		BindingManager::unbind(target);
	}
}

inline bool touchstone::FrameBufferBindingManager::is_resource_bound(
	GLenum target, GLenum alias) const
{
	assert(alias != GL_FRAMEBUFFER);
	if (target == GL_FRAMEBUFFER)
	{
		return is_resource_bound(alias);
	}

	return is_resource_bound(target);
}

inline GLuint touchstone::FrameBufferBindingManager::get_current_name(
	GLenum target, GLenum alias) const
{
	assert(alias != GL_FRAMEBUFFER);
	if (target == GL_FRAMEBUFFER)
	{
		return get_current_name(alias);
	}

	return get_current_name(target);
}

#endif
