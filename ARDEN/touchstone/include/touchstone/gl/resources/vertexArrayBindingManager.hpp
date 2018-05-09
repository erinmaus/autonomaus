// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_VERTEX_ARRAY_BINDING_MANAGER_HPP
#define TOUCHSTONE_GL_RESOURCES_VERTEX_ARRAY_BINDING_MANAGER_HPP

#include "touchstone/gl/bindingManager.hpp"

namespace touchstone
{
	class VertexArrayBindingManager : public BindingManager<SingletonBindingTarget>
	{
	public:
		VertexArrayBindingManager() = default;
		~VertexArrayBindingManager() = default;
	};
}

#endif
