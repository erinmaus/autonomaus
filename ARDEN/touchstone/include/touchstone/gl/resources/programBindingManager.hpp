// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_PROGRAM_BINDING_MANAGER_HPP
#define TOUCHSTONE_GL_RESOURCES_PROGRAM_BINDING_MANAGER_HPP

#include "touchstone/gl/bindingManager.hpp"

namespace touchstone
{
	class ProgramBindingManager : public BindingManager<SingletonBindingTarget>
	{
	public:
		ProgramBindingManager() = default;
		~ProgramBindingManager() = default;
	};
}

#endif
