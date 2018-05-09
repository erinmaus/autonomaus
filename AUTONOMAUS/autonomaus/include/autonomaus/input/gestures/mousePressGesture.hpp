// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_INPUT_GESTURES_MOUSE_PRESS_GESTURE_HPP
#define AUTONOMAUS_INPUT_GESTURES_MOUSE_PRESS_GESTURE_HPP

#include <cstddef>
#include <glm/glm.hpp>
#include "autonomaus/input/gestures/gesture.hpp"
#include "autonomaus/input/inputManager.hpp"

namespace autonomaus
{
	class MousePressGesture : public Gesture
	{
	public:
		MousePressGesture(InputManager& input_manager, int button, bool release = true);
		~MousePressGesture() = default;

		int play() override;

	private:
		// TODO: Samples.
		static const int MIN;
		static const int MAX;

		int button;
		bool release;
	};
}

#endif
