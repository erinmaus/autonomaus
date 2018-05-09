// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_MOUSE_MOUSE_BUTTON_PROVIDER_HPP
#define EIZNEBAHN_MOUSE_MOUSE_BUTTON_PROVIDER_HPP

#include "eiznebahn/input/inputProvider.hpp"
#include "eiznebahn/input/inputQueue.hpp"
#include "eiznebahn/mouse/mouseButton.hpp"

namespace eiznebahn
{
	class MouseButtonProvider : public InputProvider<InputQueue::channel_mouse>
	{
	public:
		MouseButtonProvider(const MouseButton& button, bool release);
		~MouseButtonProvider() = default;

		void update(float delta, InputSpoofer& spoofer) override;
		void cancel() override;
		bool done() const override;

	private:
		float current_time = 0.0f;
		float target_time;

		bool pending = true;

		MouseButton button;
		bool pressed = false;
		bool canceled = false;
		bool release;
	};
}

#endif
