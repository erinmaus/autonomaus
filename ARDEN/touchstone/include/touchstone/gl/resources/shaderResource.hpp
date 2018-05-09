// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_SHADER_RESOURCE_HPP
#define TOUCHSTONE_GL_RESOURCES_SHADER_RESOURCE_HPP

#include <string>
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/resource.hpp"

namespace touchstone
{
	class ShaderResource : public Resource
	{
	public:
		ShaderResource(Context* context, GLuint name, GLenum type);
		~ShaderResource() = default;

		GLenum get_type() const;

		const std::string& get_source() const;
		void set_source(const std::string& value);

	private:
		GLenum type;
		std::string source;
	};
}

#endif
