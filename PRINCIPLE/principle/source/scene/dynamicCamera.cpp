// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "principle/scene/dynamicCamera.hpp"

int principle::DynamicCamera::get_viewport_width() const
{
	return this->viewport_width;
}

void principle::DynamicCamera::set_viewport_width(int value)
{
	this->viewport_width = value;
}

int principle::DynamicCamera::get_viewport_height() const
{
	return this->viewport_height;
}

void principle::DynamicCamera::set_viewport_height(int value)
{
	this->viewport_height = value;
}

float principle::DynamicCamera::get_field_of_view() const
{
	return this->field_of_view;
}

void principle::DynamicCamera::set_field_of_view(float value)
{
	this->field_of_view = value;
}

float principle::DynamicCamera::get_near_plane() const
{
	return this->near_plane;
}

void principle::DynamicCamera::set_near_plane(float value)
{
	this->near_plane = value;
}

float principle::DynamicCamera::get_far_plane() const
{
	return this->far_plane;
}

void principle::DynamicCamera::set_far_plane(float value)
{
	this->far_plane = value;
}


float principle::DynamicCamera::get_vertical_rotation() const
{
	return this->vertical_rotation;
}

void principle::DynamicCamera::set_vertical_rotation(float value)
{
	this->vertical_rotation = value;
}

float principle::DynamicCamera::get_horizontal_rotation() const
{
	return this->horizontal_rotation;
}

void principle::DynamicCamera::set_horizontal_rotation(float value)
{
	this->horizontal_rotation = value;
}

float principle::DynamicCamera::get_distance() const
{
	return this->target_distance;
}

void principle::DynamicCamera::set_distance(float value)
{
	this->current_distance = value;
	this->target_distance = value;
	this->distance_delta = 1.0f;
}

float principle::DynamicCamera::get_focus_duration() const
{
	return this->focus_duration;
}

void principle::DynamicCamera::set_focus_duration(float value)
{
	this->focus_duration = value;
}

glm::vec3 principle::DynamicCamera::get_target() const
{
	return this->target_position;
}

void principle::DynamicCamera::set_target(const glm::vec3& value)
{
	this->target_position = value;
	this->current_position = value;
	this->position_delta = 1.0f;
}

glm::vec3 principle::DynamicCamera::get_strafe_forward() const
{
	float phi = this->vertical_rotation;
	float theta = this->horizontal_rotation;

	float x = std::cos(phi) * std::cos(theta);
	float y = 0.0f;
	float z = std::cos(phi) * std::sin(theta);

	return glm::normalize(glm::vec3(x, y, z));
}

glm::vec3 principle::DynamicCamera::get_forward() const
{
	float phi = this->vertical_rotation;
	float theta = this->horizontal_rotation;

	float x = std::cos(phi) * std::cos(theta);
	float y = std::sin(phi);
	float z = std::cos(phi) * std::sin(theta);

	return glm::normalize(glm::vec3(x, y, z));
}

glm::vec3 principle::DynamicCamera::get_left() const
{
	return glm::normalize(glm::cross(get_up(), get_forward()));
}

glm::vec3 principle::DynamicCamera::get_strafe_left() const
{
	return glm::normalize(glm::cross(get_up(), get_strafe_forward()));
}

glm::vec3 principle::DynamicCamera::get_up() const
{
	return glm::vec3(0.0f, 1.0f, 0.0f);
}

void principle::DynamicCamera::focus(const glm::vec3& target)
{
	this->current_position = glm::mix(
		this->current_position, this->target_position,
		this->position_delta);
	this->target_position = target;
	this->position_delta = 0.0f;
}

void principle::DynamicCamera::rotate_vertically(float delta)
{
	float new_horizontal_rotation = this->horizontal_rotation - delta;

	while (new_horizontal_rotation >= M_PI * 2.0f)
	{
		new_horizontal_rotation -= M_PI * 2.0f;
	}

	while (new_horizontal_rotation < 0.0f)
	{
		new_horizontal_rotation += M_PI * 2.0f;
	}

	this->horizontal_rotation = new_horizontal_rotation;
}

void principle::DynamicCamera::rotate_horizontally(float delta)
{
	float new_vertical_rotation = this->vertical_rotation + delta;

	if (new_vertical_rotation > M_PI / 2.0f)
	{
		new_vertical_rotation = M_PI / 2.0f - M_PI / 128.0f;
	}

	if (new_vertical_rotation < -(M_PI / 2.0f))
	{
		new_vertical_rotation = -M_PI / 2.0f + M_PI / 128.0f;
	}

	this->vertical_rotation = new_vertical_rotation;
}

void principle::DynamicCamera::zoom(float delta)
{
	this->current_distance = glm::mix(
		this->current_distance, this->target_distance,
		this->distance_delta);
	this->target_distance = this->target_distance + delta;
	this->distance_delta = 0.0f;
}

void principle::DynamicCamera::move_forward(float value)
{
	this->target_position += get_strafe_forward() * value;
}

void principle::DynamicCamera::move_left(float value)
{
	this->target_position += get_strafe_left() * value;
}

void principle::DynamicCamera::move_up(float value)
{
	this->target_position += get_up() * value;
}

void principle::DynamicCamera::move_horizontally(float x, float z)
{
	auto xz_plane = glm::vec3(1.0f, 0.0f, 1.0f);
	auto x_offset = get_strafe_left() * x;
	auto z_offset = get_strafe_forward() * z;

	focus(this->target_position + x_offset + z_offset);
}

void principle::DynamicCamera::move_vertically(float y)
{
	auto y_plane = glm::vec3(0.0f, 1.0f, 0.0f);
	auto y_offset = y_plane * y;

	focus(this->target_position + y_offset);
}

void principle::DynamicCamera::update(float delta)
{
	float step = delta / this->focus_duration;
	this->distance_delta = glm::min(this->distance_delta + step, 1.0f);
	this->position_delta = glm::min(this->position_delta + step, 1.0f);
}

glm::mat4 principle::DynamicCamera::get_projection_matrix() const
{
	auto perspective = glm::perspective(
		(float)this->field_of_view,
		(float)this->viewport_width / (float)this->viewport_height,
		this->near_plane, this->far_plane);
	return glm::scale(glm::mat4(1.0f), glm::vec3(-1.0f, 1.0f, 1.0f)) * perspective;
}

glm::mat4 principle::DynamicCamera::get_view_matrix() const
{
	glm::vec3 center = glm::mix(
		this->current_position, this->target_position,
		this->position_delta);
	float eye_distance = glm::mix(
		this->current_distance, this->target_distance,
		this->distance_delta);
	glm::vec3 eye = -eye_distance * get_forward() + center;
	glm::vec3 up = get_up();

	return glm::lookAt(eye, center, up);
}
