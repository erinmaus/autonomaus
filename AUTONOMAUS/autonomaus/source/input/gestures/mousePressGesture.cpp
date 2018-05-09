// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/rng.hpp"
#include "autonomaus/input/gestures/mousePressGesture.hpp"
#include "eiznebahn/mouse/mouseButton.hpp"
#include "eiznebahn/mouse/mouseButtonProvider.hpp"

const int autonomaus::MousePressGesture::MIN = 32;
const int autonomaus::MousePressGesture::MAX = 163;

autonomaus::MousePressGesture::MousePressGesture(
	InputManager& input_manager, int button, bool release) : Gesture(input_manager)
{
	this->button = button;
	this->release = release;
}

int autonomaus::MousePressGesture::play()
{
	eiznebahn::MouseButton button;
	button.button = this->button;
	button.delay = RNG::next(MIN, MAX);

	auto& queue = get_input_manager().get_queue();
	auto group = queue.start(eiznebahn::InputQueue::channel_mouse);
	queue.push<eiznebahn::MouseButtonProvider>(button, release);
	queue.stop();

	set_group(group);
	get_input_manager().preview(button);

	return success;
}
