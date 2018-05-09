// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "eiznebahn/mouse/mouseDragProvider.hpp"

eiznebahn::MouseDragProvider::MouseDragProvider(
	const MousePath& path,
	const MouseButton& press,
	const MouseButton& release) :
		movement(path), press(press), release(release)
{
	if (path.empty() || press.button != release.button)
	{
		this->pending = false;
	}
	else
	{
		this->pending = true;
		this->current_state = state_start;
	}
}

void eiznebahn::MouseDragProvider::update(float delta, InputSpoofer& spoofer)
{
	assert(!done());

	switch (this->current_state)
	{
		case state_start:
			spoofer.mouse_button_press(this->press.button);
			this->current_state = state_press;
			this->target_time = this->press.delay;
			break;
		case state_press:
			this->current_time += delta;
			if (this->current_time > this->target_time)
			{
				this->current_time -= this->target_time;

				if (this->canceled)
				{
					this->current_state = state_release;
				}
				else
				{
					this->current_state = state_movement;
				}
			}
			break;
		case state_movement:
			assert(!movement.done());

			movement.update(delta, spoofer);
			if (movement.done())
			{
				this->current_time = 0.0f;
				this->target_time = this->release.delay;
				this->current_state = state_release;
			}
			break;
		case state_release:
			this->current_time += delta;
			if (this->current_time > this->target_time || this->canceled)
			{
				spoofer.mouse_button_release(this->release.button);
				this->pending = false;
			}
			break;
		default:
			assert(false);
			break;
	}
}

void eiznebahn::MouseDragProvider::cancel()
{
	this->canceled = true;
	switch (this->current_state)
	{
		case state_start:
			// The button has yet to be pressed. We can just bail out early.
			this->pending = false;
			break;
		case state_movement:
			movement.cancel();
			break;
		case state_press:
		case state_release:
			// Nothing. The update logic will handle releasing the button.
			break;
		default:
			assert(false);
			break;
	}
}

bool eiznebahn::MouseDragProvider::done() const
{
	return !this->pending;
}
