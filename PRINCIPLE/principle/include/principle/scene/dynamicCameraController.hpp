// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_DYNAMIC_CAMERA_CONTROLLER
#define PRINCIPLE_SCENE_DYNAMIC_CAMERA_CONTROLLER

#include <glm/glm.hpp>

namespace principle
{
	class DynamicCamera;
	class KeyboardEvent;
	class MouseEvent;

	class DynamicCameraController
	{
	public:
		DynamicCameraController(DynamicCamera* camera);
		~DynamicCameraController() = default;

		float get_zoom_speed() const;
		void set_zoom_speed(float value);

		float get_movement_speed() const;
		void set_movement_speed(float value);

		float get_speed_modifier() const;
		void set_speed_modifier(float value);

		float get_pixel_angle_ratio() const;
		void set_pixel_angle_ratio(float value);

		void on_mouse_button_down(const MouseEvent& event);
		void on_mouse_button_up(const MouseEvent& event);
		void on_mouse_scroll(const MouseEvent& event);
		void on_mouse_move(const MouseEvent& event);
		void on_key_down(const KeyboardEvent& event);
		void on_key_up(const KeyboardEvent& event);
		void update(float delta);

	private:
		float get_speed_multiplier() const;

		float previous_mouse_wheel = 0.0f;
		bool is_in_mouse_gesture = false;

		glm::vec3 movement_axes = glm::vec3(0.0f);
		bool use_speed_modifier = false;

		float zoom_speed = 128.0f;
		float movement_speed = 256.0f;
		float speed_modifier = 8.0f;
		float pixel_angle_ratio = M_PI * 2.0f / 1024.0f;

		DynamicCamera* camera;
	};
}

#endif
