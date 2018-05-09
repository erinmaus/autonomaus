// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvncer/render/gl.hpp"
#include "principle/common/view.hpp"
#include "principle/common/keyboardEvent.hpp"
#include "principle/common/mouseEvent.hpp"

glm::vec2 principle::View::get_position() const
{
	return this->position;
}

void principle::View::set_position(const glm::vec2& value)
{
	this->position = value;
}

glm::vec2 principle::View::get_size() const
{
	return this->size;
}

void principle::View::set_size(const glm::vec2& value)
{
	this->size = value;
}

void principle::View::perform_layout()
{
	// Nothing.
}

void principle::View::update(float delta)
{
	// Nothing.
}

void principle::View::draw()
{
	apply_viewport();
}

void principle::View::on_mouse_button_down(const MouseEvent& event)
{
	// Nothing.
}

void principle::View::on_mouse_button_up(const MouseEvent& event)
{
	// Nothing.
}

void principle::View::on_mouse_move(const MouseEvent& event)
{
	// Nothing.
}

void principle::View::on_mouse_scroll(const MouseEvent& event)
{
	// Nothing.
}

void principle::View::on_key_down(const KeyboardEvent& event)
{
	// Nothing.
}

void principle::View::on_key_up(const KeyboardEvent& event)
{
	// Nothing.
}

void principle::View::apply_viewport()
{
	glEnable(GL_SCISSOR_TEST);
	glViewport(this->position.x, this->position.y, this->size.x, this->size.y);
	glScissor(this->position.x, this->position.y, this->size.x, this->size.y);
}
