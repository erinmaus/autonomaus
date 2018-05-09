// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_PLATFORM_NULL_INPUT_SPOOFER_HPP
#define EIZNEBAHN_PLATFORM_NULL_INPUT_SPOOFER_HPP

#include "eiznebahn/input/inputSpoofer.hpp"
#include "eiznebahn/input/inputState.hpp"

namespace eiznebahn
{
	class NullInputSpoofer : public InputSpoofer
	{
	public:
		NullInputSpoofer() = default;
		~NullInputSpoofer() = default;

		void mouse_move(int x, int y) override;
		void mouse_scroll_down() override;
		void mouse_scroll_up() override;
		void mouse_button_press(int button) override;
		void mouse_button_release(int button) override;

		void key_press(int key_code) override;
		void key_release(int key_code) override;

		void poll() override;

		const InputState& get_input_state() const override;

	private:
		InputState input_state;
	};
}

#endif
