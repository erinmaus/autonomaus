// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/common/keyboardEvent.hpp"

int principle::KeyboardEvent::get_key_code() const
{
	return this->key_code;
}

void principle::KeyboardEvent::set_key_code(int value)
{
	this->key_code = value;
}

int principle::KeyboardEvent::get_modifiers() const
{
	return this->modifiers;
}

void principle::KeyboardEvent::set_modifiers(int value)
{
	this->modifiers = value;
}

int principle::KeyboardEvent::get_character() const
{
	return this->character;
}

void principle::KeyboardEvent::set_character(int value)
{
	this->character = value;
}
