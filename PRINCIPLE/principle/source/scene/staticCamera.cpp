// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/scene/staticCamera.hpp"

principle::StaticCamera::StaticCamera(
	const glm::mat4& projection, const glm::mat4& view)
{
	this->projection = projection;
	this->view = view;
}

glm::mat4 principle::StaticCamera::get_projection_matrix() const
{
	return this->projection;
}

void principle::StaticCamera::set_projection_matrix(const glm::mat4& value)
{
	this->projection = value;
}

glm::mat4 principle::StaticCamera::get_view_matrix() const
{
	return this->view;
}

void principle::StaticCamera::set_view_matrix(const glm::mat4& value)
{
	this->view = value;
}
