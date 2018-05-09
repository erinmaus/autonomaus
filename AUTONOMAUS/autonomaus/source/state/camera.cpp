// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "autonomaus/state/camera.hpp"
#include "autonomaus/graphics/drawState.hpp"

autonomaus::Camera::Camera(
	const glm::mat4& view,
	const glm::mat4& projection,
	const glm::ivec4& viewport,
	int window_width, int window_height) :
		view(view), projection(projection), viewport(viewport),
		window_width(window_width), window_height(window_height)
{
	// Nothing.
}

autonomaus::Camera::Camera(const DrawState& draw_state) :
	view(draw_state.get_camera_view_matrix()),
	projection(draw_state.get_camera_projection_matrix()),
	viewport(draw_state.get_world_viewport()),
	window_width(draw_state.get_gui_viewport().z),
	window_height(draw_state.get_gui_viewport().w)
{
	this->adjusted_viewport = viewport;
	this->adjusted_viewport.y = window_height - (this->adjusted_viewport.y + this->adjusted_viewport.w);
}

const glm::mat4& autonomaus::Camera::get_view() const
{
	return this->view;
}

const glm::mat4& autonomaus::Camera::get_projection() const
{
	return this->projection;
}

const glm::ivec4& autonomaus::Camera::get_viewport() const
{
	return this->adjusted_viewport;
}

glm::vec3 autonomaus::Camera::world_to_screen(const glm::vec3& point) const
{
	auto screen_space = glm::project(
		point,
		this->view, this->projection,
		this->viewport);
	screen_space.y = this->window_height - screen_space.y;

	return screen_space;
}

glm::vec3 autonomaus::Camera::screen_to_world(const glm::vec3& point) const
{
	glm::vec3 flipped_point = point;
	flipped_point.y = this->window_height - point.y;

	return glm::unProject(flipped_point, this->view, this->projection, this->viewport);
}

bool autonomaus::Camera::is_visible(const glm::vec3& point) const
{
	auto screen_space = glm::project(
		point,
		this->view, this->projection,
		this->viewport);

	if (screen_space.x < this->viewport.x ||
		screen_space.x > this->viewport.x + this->viewport.z ||
		screen_space.y < this->viewport.y ||
		screen_space.y > this->viewport.y + this->viewport.w ||
		screen_space.z < 0.0f || screen_space.z > 1.0f)
	{
		return false;
	}

	return true;
}

bool autonomaus::Camera::is_visible(const glm::vec2& point) const
{
	auto flipped_point = point;
	flipped_point.y = this->window_height - point.y;

	if (flipped_point.x < this->viewport.x ||
		flipped_point.x > this->viewport.x + this->viewport.z ||
		flipped_point.y < this->viewport.y ||
		flipped_point.y > this->viewport.y + this->viewport.w)
	{
		return false;
	}

	return true;
}
