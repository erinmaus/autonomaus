// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/gl/resource.hpp"

touchstone::Resource::Resource(Context* context, GLuint name)
{
	this->context = context;
	this->name = name;
}

GLuint touchstone::Resource::get_name() const
{
	return this->name;
}

touchstone::Context* touchstone::Resource::get_context() const
{
	return this->context;
}
