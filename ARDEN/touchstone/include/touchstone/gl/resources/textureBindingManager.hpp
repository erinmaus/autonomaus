// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_TEXTURE_BINDING_MANAGER_HPP
#define TOUCHSTONE_GL_RESOURCES_TEXTURE_BINDING_MANAGER_HPP

#include <tuple>
#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/bindingManager.hpp"

namespace touchstone
{
	typedef std::tuple<GLenum /* unit */, GLenum /* target */> TextureBindingTarget;

	class TextureBindingManager : private BindingManager<TextureBindingTarget>
	{
	public:
		TextureBindingManager() = default;
		~TextureBindingManager() = default;

		static const std::size_t UNIT = 0;
		static const std::size_t TARGET = 1;

		void set_active_unit(GLenum unit);
		GLenum get_active_unit() const;

		void bind(GLenum target, GLuint name);
		void unbind(GLenum target);
		using BindingManager::unbind_all;

		bool is_resource_bound(GLenum target) const;
		bool is_resource_bound(GLenum target, GLenum unit) const;

		GLuint get_current_name(GLenum target) const;
		GLuint get_current_name(GLenum target, GLenum unit) const;

		using BindingManager::begin;
		using BindingManager::end;

	private:
		static TextureBindingTarget make_binding_target(GLenum unit, GLenum target);

		GLenum active_unit = GL_TEXTURE0;
	};
}

#endif
