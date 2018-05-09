// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/common/mouseEvent.hpp"

glm::vec2 principle::MouseEvent::get_position() const
{
	return this->position;
}

void principle::MouseEvent::set_position(const glm::vec2& value)
{
	this->position = value;
}

glm::vec2 principle::MouseEvent::get_difference() const
{
	return this->difference;
}

void principle::MouseEvent::set_difference(const glm::vec2& value)
{
	this->difference = value;
}

float principle::MouseEvent::get_wheel() const
{
	return this->wheel;
}

void principle::MouseEvent::set_wheel(float value)
{
	this->wheel = value;
}

int principle::MouseEvent::get_button() const
{
	return this->button;
}

void principle::MouseEvent::set_button(int value)
{
	this->button = value;
}
