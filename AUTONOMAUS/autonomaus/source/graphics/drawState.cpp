// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/graphics/drawState.hpp"

const glm::mat4& autonomaus::DrawState::get_camera_view_matrix() const
{
	return this->camera_view_matrix;
}

void autonomaus::DrawState::set_camera_view_matrix(const glm::mat4& value)
{
	this->camera_view_matrix = value;
}

const glm::mat4& autonomaus::DrawState::get_camera_projection_matrix() const
{
	return this->camera_projection_matrix;
}

void autonomaus::DrawState::set_camera_projection_matrix(const glm::mat4& value)
{
	this->camera_projection_matrix = value;
}

const glm::ivec4& autonomaus::DrawState::get_world_viewport() const
{
	return this->world_viewport;
}

void autonomaus::DrawState::set_world_viewport(const glm::ivec4& value)
{
	this->world_viewport = value;
}

const glm::ivec4& autonomaus::DrawState::get_gui_viewport() const
{
	return this->gui_viewport;
}

void autonomaus::DrawState::set_gui_viewport(const glm::ivec4& value)
{
	this->gui_viewport = value;
}
