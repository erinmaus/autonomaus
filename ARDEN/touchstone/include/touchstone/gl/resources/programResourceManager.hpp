// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_PROGRAM_RESOURCE_MANAGER_HPP
#define TOUCHSTONE_GL_RESOURCES_PROGRAM_RESOURCE_MANAGER_HPP

#include "touchstone/gl/resources/programResource.hpp"
#include "touchstone/gl/resources/resourceManager.hpp"
#include "touchstone/gl/resource.hpp"

namespace touchstone
{
	class State;

	class ProgramResourceManager : public ResourceManager<ProgramResource>
	{
	public:
		ProgramResourceManager(Context* state);
		~ProgramResourceManager() = default;

		Context* get_state() const;

	protected:
		GLuint get_name(const ProgramResource& resource) const;

	private:
		Context* state;
	};
}

inline touchstone::ProgramResourceManager::ProgramResourceManager(Context* state)
{
	this->state = state;
}

inline GLuint touchstone::ProgramResourceManager::get_name(
	const ProgramResource& resource) const
{
	return resource.get_name();
}

#endif
