// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <tuple>
#include "eiznebahn/keyboard/typingProvider.hpp"

eiznebahn::TypingProvider::TypingProvider(const KeySequence& sequence) :
	sequence(sequence)
{
	if (sequence.empty() || !sequence.get_is_safe())
	{
		this->pending = false;
	}
	else
	{
		this->target_time = std::get<KeySequence::DELAY>(sequence.get(0));
		this->pending = true;
	}
}

void eiznebahn::TypingProvider::update(float delta, InputSpoofer& spoofer)
{
	assert(!done());

	if (this->canceled)
	{
		for (auto key: this->keys)
		{
			spoofer.key_release(key);
		}

		for (auto key: this->modifiers)
		{
			spoofer.key_release(key);
		}

		this->pending = false;
	}
	else
	{
		this->current_time += delta;
		while (this->current_time > this->target_time)
		{
			auto current = this->sequence.get(this->current_index);
			int key = std::get<KeySequence::KEY>(current);
			int type = std::get<KeySequence::TYPE>(current);

			if (type == KeySequence::key_press)
			{
				if (is_modifier(key))
				{
					this->modifiers.insert(key);
				}
				else
				{
					this->keys.insert(key);
				}

				spoofer.key_press(key);
			}
			else
			{
				if (is_modifier(key))
				{
					this->modifiers.erase(key);
				}
				else
				{
					this->keys.erase(key);
				}

				spoofer.key_release(key);
			}

			++this->current_index;
			if (this->current_index >= this->sequence.count())
			{
				assert(this->current_index == this->sequence.count());
				this->pending = false;
				break;
			}

			auto next = this->sequence.get(this->current_index);
			this->target_time += std::get<KeySequence::DELAY>(next);
		}
	}
}

void eiznebahn::TypingProvider::cancel()
{
	this->canceled = true;
}

bool eiznebahn::TypingProvider::done() const
{
	return !this->pending;
}

bool eiznebahn::TypingProvider::is_modifier(int key_code)
{
	switch (key_code)
	{
		case KeySequence::key_code_left_ctrl:
		case KeySequence::key_code_right_ctrl:
		case KeySequence::key_code_left_alt:
		case KeySequence::key_code_right_alt:
		case KeySequence::key_code_left_shift:
		case KeySequence::key_code_right_shift:
			return true;
		default:
			return false;
	}
}
