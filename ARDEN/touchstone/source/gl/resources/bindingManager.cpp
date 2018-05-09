// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/gl/bindingManager.hpp"

void touchstone::BindingManager<touchstone::SingletonBindingTarget>::bind(GLuint name)
{
	this->current_name = name;
}

void touchstone::BindingManager<touchstone::SingletonBindingTarget>::unbind()
{
	this->current_name = 0;
}

void touchstone::BindingManager<touchstone::SingletonBindingTarget>::unbind_all(
	GLuint name)
{
	if (this->current_name == name)
	{
		this->current_name = 0;
	}
}

bool touchstone::BindingManager<touchstone::SingletonBindingTarget>::is_resource_bound() const
{
	return this->current_name != 0;
}

GLuint touchstone::BindingManager<touchstone::SingletonBindingTarget>::get_current_name() const
{
	return this->current_name;
}
