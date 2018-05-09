// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_INPUT_GESTURES_WORLD_MOUSE_MOVE_GESTURE_HPP
#define AUTONOMAUS_INPUT_GESTURES_WORLD_MOUSE_MOVE_GESTURE_HPP

#include <cstddef>
#include <glm/glm.hpp>
#include "autonomaus/input/gestures/gesture.hpp"
#include "autonomaus/input/inputManager.hpp"
#include "autonomaus/state/camera.hpp"

namespace autonomaus
{
	class WorldMouseMoveGesture : public Gesture
	{
	public:
		WorldMouseMoveGesture(
			InputManager& input_manager,
			const glm::vec3& position,
			const Camera& camera);
		~WorldMouseMoveGesture() = default;

		int play() override;

	private:
		static const std::size_t MIN_PATHS;
		static const float DRIFT_THRESHOLD;
		static const float LENGTH_THRESHOLD;

		int try_get_screen_space(glm::vec2& result);
		int try_find_path(
			const glm::vec2& position,
			float& result_length, float& result_direction,
			eiznebahn::MousePath& result_path);

		glm::vec3 position;
		Camera camera;
	};
}

#endif
