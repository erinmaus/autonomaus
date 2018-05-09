// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_BUFFER_BINDING_MANAGER_HPP
#define TOUCHSTONE_GL_RESOURCES_BUFFER_BINDING_MANAGER_HPP

#include <tuple>
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/bindingManager.hpp"

namespace touchstone
{
	typedef std::tuple<GLenum /* target */, GLint /* index */> BufferBindingTarget;

	class BufferBindingManager : private BindingManager<BufferBindingTarget>
	{
	public:
		BufferBindingManager() = default;
		~BufferBindingManager() = default;

		static const std::size_t TARGET = 0;
		static const std::size_t INDEX = 0;
		static const GLint GLOBAL_INDEX = -1;

		void bind(GLenum target, GLuint name);
		void bind(GLenum target, GLint index, GLuint name);
		void unbind(GLenum target, GLint index = GLOBAL_INDEX);
		using BindingManager::unbind_all;

		bool is_resource_bound(GLenum target, GLint index = GLOBAL_INDEX) const;
		GLuint get_current_name(GLenum target, GLint index = GLOBAL_INDEX) const;

		using BindingManager::begin;
		using BindingManager::end;

	private:
		static BufferBindingTarget make_binding_target(
			GLenum target, GLint index);
	};
}

#endif
