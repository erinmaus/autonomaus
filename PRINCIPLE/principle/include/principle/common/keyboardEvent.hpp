// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_KEYBOARD_EVENT_HPP
#define PRINCIPLE_KEYBOARD_EVENT_HPP

namespace principle
{
	class KeyboardEvent
	{
	public:
		KeyboardEvent() = default;
		~KeyboardEvent() = default;

		int get_key_code() const;
		void set_key_code(int value);

		int get_modifiers() const;
		void set_modifiers(int value);

		int get_character() const;
		void set_character(int value);

	private:
		int key_code = 0, modifiers = 0;
		int character = 0;
	};
}

#endif
