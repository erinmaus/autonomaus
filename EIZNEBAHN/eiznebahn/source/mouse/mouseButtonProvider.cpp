// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "eiznebahn/mouse/mouseButtonProvider.hpp"

eiznebahn::MouseButtonProvider::MouseButtonProvider(const MouseButton& button, bool release) :
	button(button)
{
	this->target_time = button.delay;
	this->release = release;
}

void eiznebahn::MouseButtonProvider::update(float delta, InputSpoofer& spoofer)
{
	assert(!done());

	if (!this->pressed)
	{
		bool was_pressed = true;
		if (!spoofer.get_input_state().is_mouse_button_pressed(this->button.button))
		{
			was_pressed = false;
			spoofer.mouse_button_press(this->button.button);
		}

		if (was_pressed || !this->canceled)
		{
			this->pressed = true;
		}
	}
	else
	{
		this->current_time += delta;
		if (this->current_time >= this->target_time)
		{
			if (this->release || this->canceled)
			{
				spoofer.mouse_button_release(this->button.button);
			}

			this->pending = false;
		}
	}
}

void eiznebahn::MouseButtonProvider::cancel()
{
	this->canceled = true;
}

bool eiznebahn::MouseButtonProvider::done() const
{
	return !this->pending;
}
