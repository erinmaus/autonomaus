// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_RENDER_BUFFER_BINDING_MANAGER_HPP
#define TOUCHSTONE_GL_RESOURCES_RENDER_BUFFER_BINDING_MANAGER_HPP

#include "touchstone/gl/bindingManager.hpp"

namespace touchstone
{
	class RenderBufferBindingManager : public BindingManager<SingletonBindingTarget>
	{
	public:
		RenderBufferBindingManager() = default;
		~RenderBufferBindingManager() = default;
	};
}

#endif
