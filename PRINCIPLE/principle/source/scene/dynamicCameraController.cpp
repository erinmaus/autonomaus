// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <allegro5/allegro.h>
#include "principle/common/keyboardEvent.hpp"
#include "principle/common/mouseEvent.hpp"
#include "principle/scene/dynamicCamera.hpp"
#include "principle/scene/dynamicCameraController.hpp"

principle::DynamicCameraController::DynamicCameraController(
	DynamicCamera* camera)
{
	this->camera = camera;
}

float principle::DynamicCameraController::get_zoom_speed() const
{
	return this->zoom_speed;
}

void principle::DynamicCameraController::set_zoom_speed(float value)
{
	this->zoom_speed = value;
}

float principle::DynamicCameraController::get_movement_speed() const
{
	return this->movement_speed;
}

void principle::DynamicCameraController::set_movement_speed(float value)
{
	this->movement_speed = value;
}

float principle::DynamicCameraController::get_speed_modifier() const
{
	return this->speed_modifier;
}

void principle::DynamicCameraController::set_speed_modifier(float value)
{
	this->speed_modifier = value;
}

float principle::DynamicCameraController::get_pixel_angle_ratio() const
{
	return this->pixel_angle_ratio;
}

void principle::DynamicCameraController::set_pixel_angle_ratio(float value)
{
	this->pixel_angle_ratio = value;
}

void principle::DynamicCameraController::on_mouse_button_down(
	const MouseEvent& event)
{
	if (event.get_button() == 2)
	{
		this->is_in_mouse_gesture = true;
	}
}

void principle::DynamicCameraController::on_mouse_button_up(
	const MouseEvent& event)
{
	if (event.get_button() == 2)
	{
		this->is_in_mouse_gesture = false;
	}
}

void principle::DynamicCameraController::on_mouse_scroll(const MouseEvent& event)
{
	float difference = event.get_wheel() - this->previous_mouse_wheel;
	this->camera->zoom(-(difference * this->zoom_speed * get_speed_multiplier()));

	this->previous_mouse_wheel = event.get_wheel();
}

void principle::DynamicCameraController::on_mouse_move(const MouseEvent& event)
{
	if (this->is_in_mouse_gesture)
	{
		this->camera->rotate_vertically(
			event.get_difference().x * this->pixel_angle_ratio);
		this->camera->rotate_horizontally(
			-(event.get_difference().y * this->pixel_angle_ratio));
	}
}

void principle::DynamicCameraController::on_key_down(const KeyboardEvent& event)
{
	switch (event.get_key_code())
	{
		case ALLEGRO_KEY_A:
			this->movement_axes.x -= 1.0f;
			break;
		case ALLEGRO_KEY_D:
			this->movement_axes.x += 1.0f;
			break;
		case ALLEGRO_KEY_W:
			this->movement_axes.z += 1.0f;
			break;
		case ALLEGRO_KEY_S:
			this->movement_axes.z -= 1.0f;
			break;
		case ALLEGRO_KEY_Q:
			this->movement_axes.y += 1.0f;
			break;
		case ALLEGRO_KEY_E:
			this->movement_axes.y -= 1.0f;
			break;
		case ALLEGRO_KEY_LSHIFT:
			this->use_speed_modifier = true;
			break;
	}
}

void principle::DynamicCameraController::on_key_up(const KeyboardEvent& event)
{
	switch (event.get_key_code())
	{
		case ALLEGRO_KEY_A:
			this->movement_axes.x += 1.0f;
			break;
		case ALLEGRO_KEY_D:
			this->movement_axes.x -= 1.0f;
			break;
		case ALLEGRO_KEY_W:
			this->movement_axes.z -= 1.0f;
			break;
		case ALLEGRO_KEY_S:
			this->movement_axes.z += 1.0f;
			break;
		case ALLEGRO_KEY_Q:
			this->movement_axes.y -= 1.0f;
			break;
		case ALLEGRO_KEY_E:
			this->movement_axes.y += 1.0f;
			break;
		case ALLEGRO_KEY_LSHIFT:
			this->use_speed_modifier = false;
			break;
		case ALLEGRO_KEY_SPACE:
			this->movement_axes = glm::vec3(0.0f);
			this->use_speed_modifier = false;
			break;
	}
}

void principle::DynamicCameraController::update(float delta)
{
	this->camera->move_horizontally(
		this->movement_axes.x * this->movement_speed * get_speed_multiplier(),
		this->movement_axes.z * this->movement_speed * get_speed_multiplier());
	this->camera->move_vertically(
		this->movement_axes.y * this->movement_speed * get_speed_multiplier());
}

float principle::DynamicCameraController::get_speed_multiplier() const
{
	if (this->use_speed_modifier)
	{
		return this->speed_modifier;
	}
	
	return 1.0f;
}
