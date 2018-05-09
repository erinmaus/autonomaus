// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_INPUT_INPUT_STATE_HPP
#define EIZNEBAHN_INPUT_INPUT_STATE_HPP

#include <string>
#include "eiznebahn/keyboard/keySequence.hpp"
#include "eiznebahn/mouse/mouseButton.hpp"

namespace eiznebahn
{
	class InputState
	{
	public:
		InputState() = default;
		~InputState() = default;

		bool is_key_pressed(int key_code) const;
		bool is_key_released(int key_code) const;
		void set_key_pressed(int key_code);
		void set_key_released(int key_code);

		int get_mouse_x() const;
		int get_mouse_y() const;
		void set_mouse_position(int x, int y);

		int get_mouse_scroll() const;
		void mouse_scroll_up();
		void mouse_scroll_down();

		bool is_mouse_button_pressed(int button) const;
		bool is_mouse_button_released(int button) const;
		void set_mouse_button_pressed(int button);
		void set_mouse_button_released(int button);

		std::string get_text_input() const;
		void set_text_input(const std::string& value);

	private:
		bool keys[KeySequence::key_code_max] = {};

		int mouse_x = 0;
		int mouse_y = 0;
		int mouse_scroll = 0;
		bool mouse_buttons[MouseButton::button_max] = {};

		std::string text;
	};
}

#endif
