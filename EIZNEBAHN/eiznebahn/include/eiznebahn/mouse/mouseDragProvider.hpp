// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_MOUSE_MOUSE_CLICK_PROVIDER_HPP
#define EIZNEBAHN_MOUSE_MOUSE_CLICK_PROVIDER_HPP

#include "eiznebahn/input/inputProvider.hpp"
#include "eiznebahn/input/inputQueue.hpp"
#include "eiznebahn/mouse/mouseButton.hpp"
#include "eiznebahn/mouse/mousePath.hpp"
#include "eiznebahn/mouse/mouseMovementProvider.hpp"

namespace eiznebahn
{
	class MouseDragProvider : public InputProvider<InputQueue::channel_mouse>
	{
	public:
		explicit MouseDragProvider(
			const MousePath& path,
			const MouseButton& press,
			const MouseButton& release);
		~MouseDragProvider() = default;

		void update(float delta, InputSpoofer& spoofer) override;
		void cancel() override;
		bool done() const override;

	private:
		enum
		{
			state_start,
			state_press,
			state_movement,
			state_release
		};
		int current_state;

		float current_time = 0.0f;
		float target_time;

		bool canceled = false;
		bool pending;

		MouseMovementProvider movement;
		MouseButton press;
		MouseButton release;
	};
}

#endif
