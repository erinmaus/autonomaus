// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/input/gestures/gesture.hpp"

autonomaus::Gesture::Gesture(InputManager& input_manager)
{
	this->input_manager = &input_manager;
}

autonomaus::Gesture::~Gesture()
{
	cancel();
}

bool autonomaus::Gesture::pending() const
{
	if (this->is_group_set)
	{
		return input_manager->get_queue().pending(this->group);
	}

	return false;
}

void autonomaus::Gesture::cancel()
{
	if (this->is_group_set && pending())
	{
		input_manager->get_queue().cancel(this->group);
	}
}

autonomaus::InputManager& autonomaus::Gesture::get_input_manager() const
{
	return *this->input_manager;
}

eiznebahn::InputQueue::Group autonomaus::Gesture::get_group() const
{
	return this->group;
}

void autonomaus::Gesture::set_group(eiznebahn::InputQueue::Group value)
{
	this->group = value;
	this->is_group_set = true;
}
