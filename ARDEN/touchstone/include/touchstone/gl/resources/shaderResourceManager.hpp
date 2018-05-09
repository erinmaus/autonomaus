// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_SHADER_RESOURCE_MANAGER_HPP
#define TOUCHSTONE_GL_RESOURCES_SHADER_RESOURCE_MANAGER_HPP

#include "touchstone/gl/resources/resourceManager.hpp"
#include "touchstone/gl/resources/shaderResource.hpp"
#include "touchstone/gl/resource.hpp"

namespace touchstone
{
	class Context;

	class ShaderResourceManager : public ResourceManager<ShaderResource>
	{
	public:
		ShaderResourceManager(Context* context);
		~ShaderResourceManager() = default;

	protected:
		GLuint get_name(const ShaderResource& resource) const;

	private:
		Context* context;
	};
}

inline touchstone::ShaderResourceManager::ShaderResourceManager(Context* context)
{
	this->context = context;
}

inline GLuint touchstone::ShaderResourceManager::get_name(
	const ShaderResource& resource) const
{
	return resource.get_name();
}

#endif
