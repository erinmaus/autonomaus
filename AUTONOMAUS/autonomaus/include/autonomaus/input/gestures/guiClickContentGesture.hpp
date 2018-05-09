// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_INPUT_GESTURES_GUI_CLICK_CONTENT_GESTURE_HPP
#define AUTONOMAUS_INPUT_GESTURES_GUI_CLICK_CONTENT_GESTURE_HPP

#include <cstddef>
#include <glm/glm.hpp>
#include "autonomaus/input/gestures/gesture.hpp"
#include "autonomaus/input/inputManager.hpp"
#include "autonomaus/state/camera.hpp"
#include "eiznebahn/mouse/mouseButton.hpp"
#include "chomsky/gui/guiContent.hpp"

namespace autonomaus
{
	class GUIClickContentGesture : public Gesture
	{
	public:
		GUIClickContentGesture(
			InputManager& input_manager,
			const chomsky::GUIContent& content,
			int window_width, int window_height,
			int button = eiznebahn::MouseButton::button_left);
		GUIClickContentGesture(
			InputManager& input_manager,
			int x, int y,
			int width, int height,
			int window_width, int window_height,
			int button = eiznebahn::MouseButton::button_left);
		~GUIClickContentGesture() = default;

		int play() override;

	private:
		// TODO: Samples.
		static const int MIN;
		static const int MAX;

		static const std::size_t MIN_PATHS;
		static const float DRIFT_THRESHOLD;
		static const float LENGTH_THRESHOLD;

		int try_find_path(
			const glm::vec2& position,
			float& result_length, float& result_direction,
			eiznebahn::MousePath& result_path);

		int content_width, content_height, content_x, content_y;
		int window_width, window_height;
		int button;
	};
}

#endif
