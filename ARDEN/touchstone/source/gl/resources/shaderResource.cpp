// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/gl/resources/shaderResource.hpp"
#include "touchstone/trace/discreteCallSet.hpp"

touchstone::ShaderResource::ShaderResource(
	Context* context, GLuint name, GLenum type) :
		Resource(context, name)
{
	this->type = type;
}

GLenum touchstone::ShaderResource::get_type() const
{
	return this->type;
}

const std::string& touchstone::ShaderResource::get_source() const
{
	return this->source;
}

void touchstone::ShaderResource::set_source(const std::string& value)
{
	this->source = value;
}
