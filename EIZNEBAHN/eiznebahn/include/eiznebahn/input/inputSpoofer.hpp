// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_INPUT_INPUT_SPOOFER_HPP
#define EIZNEBAHN_INPUT_INPUT_SPOOFER_HPP

#include "eiznebahn/input/inputState.hpp"

namespace eiznebahn
{
	class InputSpoofer
	{
	public:
		virtual ~InputSpoofer() = default;

		virtual void mouse_move(int x, int y) = 0;
		virtual void mouse_scroll_down() = 0;
		virtual void mouse_scroll_up() = 0;
		virtual void mouse_button_press(int button) = 0;
		virtual void mouse_button_release(int button) = 0;

		virtual void key_press(int key) = 0;
		virtual void key_release(int key) = 0;

		virtual void poll() = 0;

		virtual const InputState& get_input_state() const = 0;
	};

	InputSpoofer* create_default_input_spoofer();
}

#endif
