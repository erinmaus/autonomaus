// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "eiznebahn/keyboard/keySequence.hpp"

void eiznebahn::KeySequence::press(int key, float delay)
{
	if (this->current_keys.count(key) != 0)
	{
		this->is_safe = false;
	}

	this->keys.emplace_back(key, key_press, delay);
	this->current_keys.insert(key);
}

void eiznebahn::KeySequence::release(int key, float delay)
{
	if (this->current_keys.count(key) == 0)
	{
		this->is_safe = false;
	}

	this->keys.emplace_back(key, key_release, delay);
	this->current_keys.erase(key);
}

eiznebahn::KeySequence::Key eiznebahn::KeySequence::get(std::size_t index) const
{
	return this->keys.at(index);
}

bool eiznebahn::KeySequence::empty() const
{
	return this->keys.empty();
}

std::size_t eiznebahn::KeySequence::count() const
{
	return this->keys.size();
}

bool eiznebahn::KeySequence::get_is_safe() const
{
	return this->is_safe && this->current_keys.empty();
}

eiznebahn::KeySequence::const_iterator
eiznebahn::KeySequence::begin() const
{
	return this->keys.begin();
}

eiznebahn::KeySequence::const_iterator
eiznebahn::KeySequence::end() const
{
	return this->keys.end();
}
