// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "eiznebahn/platform/nullInputSpoofer.hpp"

void eiznebahn::NullInputSpoofer::mouse_move(int x, int y)
{
	this->input_state.set_mouse_position(x, y);
}

void eiznebahn::NullInputSpoofer::mouse_scroll_down()
{
	this->input_state.mouse_scroll_down();
}

void eiznebahn::NullInputSpoofer::mouse_scroll_up()
{
	this->input_state.mouse_scroll_up();
}

void eiznebahn::NullInputSpoofer::mouse_button_press(int button)
{
	this->input_state.set_mouse_button_pressed(button);
}

void eiznebahn::NullInputSpoofer::mouse_button_release(int button)
{
	this->input_state.set_mouse_button_released(button);
}

void eiznebahn::NullInputSpoofer::key_press(int key_code)
{
	this->input_state.set_key_pressed(key_code);
}

void eiznebahn::NullInputSpoofer::key_release(int key_code)
{
	this->input_state.set_key_released(key_code);
}

void eiznebahn::NullInputSpoofer::poll()
{
	// Nothing.
}

const eiznebahn::InputState& eiznebahn::NullInputSpoofer::get_input_state() const
{
	return this->input_state;
}
