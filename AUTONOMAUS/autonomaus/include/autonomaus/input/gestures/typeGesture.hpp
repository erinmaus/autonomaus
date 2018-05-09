// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_INPUT_GESTURES_TYPE_GESTURE_HPP
#define AUTONOMAUS_INPUT_GESTURES_TYPE_GESTURE_HPP

#include <cstddef>
#include <glm/glm.hpp>
#include "autonomaus/input/gestures/gesture.hpp"
#include "autonomaus/input/inputManager.hpp"

namespace autonomaus
{
	class TypeGesture : public Gesture
	{
	public:
		TypeGesture(
			InputManager& input_manager,
			const std::string& text,
			bool ignore_case);
		~TypeGesture() = default;

		static std::string escape_sequence(int modifier, int key);
		static std::string escape_sequence(int modifier, char key);
		static std::string escape_sequence(int key);

		int play() override;

	private:
		// TODO: Samples.
		static const int MIN;
		static const int MAX;

		static const char* ESCAPE;

		std::string text;
		bool ignore_case;
	};
}

#endif
