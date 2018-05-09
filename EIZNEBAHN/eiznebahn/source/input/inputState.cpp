// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "eiznebahn/input/inputState.hpp"

bool eiznebahn::InputState::is_key_pressed(int key_code) const
{
	assert(key_code >= 0);
	assert(key_code < KeySequence::key_code_max);

	return keys[key_code];
}

bool eiznebahn::InputState::is_key_released(int key_code) const
{
	assert(key_code >= 0);
	assert(key_code < KeySequence::key_code_max);

	return !keys[key_code];
}

void eiznebahn::InputState::set_key_pressed(int key_code)
{
	assert(key_code >= 0);
	assert(key_code < KeySequence::key_code_max);

	keys[key_code] = true;
}

void eiznebahn::InputState::set_key_released(int key_code)
{
	assert(key_code >= 0);
	assert(key_code < KeySequence::key_code_max);

	keys[key_code] = false;
}

int eiznebahn::InputState::get_mouse_x() const
{
	return this->mouse_x;
}

int eiznebahn::InputState::get_mouse_y() const
{
	return this->mouse_y;
}

void eiznebahn::InputState::set_mouse_position(int x, int y)
{
	this->mouse_x = x;
	this->mouse_y = y;
}


int eiznebahn::InputState::get_mouse_scroll() const
{
	return this->mouse_scroll;
}

void eiznebahn::InputState::mouse_scroll_up()
{
	++this->mouse_scroll;
}

void eiznebahn::InputState::mouse_scroll_down()
{
	--this->mouse_scroll;
}

bool eiznebahn::InputState::is_mouse_button_pressed(int button) const
{
	assert(button >= MouseButton::button_min);
	assert(button < MouseButton::button_max);

	return this->mouse_buttons[button];
}

bool eiznebahn::InputState::is_mouse_button_released(int button) const
{
	assert(button >= MouseButton::button_min);
	assert(button < MouseButton::button_max);

	return !this->mouse_buttons[button];
}

void eiznebahn::InputState::set_mouse_button_pressed(int button)
{
	assert(button >= MouseButton::button_min);
	assert(button < MouseButton::button_max);

	this->mouse_buttons[button] = true;
}

void eiznebahn::InputState::set_mouse_button_released(int button)
{
	assert(button >= MouseButton::button_min);
	assert(button < MouseButton::button_max);

	this->mouse_buttons[button] = false;
}

std::string eiznebahn::InputState::get_text_input() const
{
	return this->text;
}

void eiznebahn::InputState::set_text_input(const std::string& value)
{
	this->text = value;
}