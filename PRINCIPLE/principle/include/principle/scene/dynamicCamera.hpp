// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_DYNAMIC_CAMERA_HPP
#define PRINCIPLE_SCENE_DYNAMIC_CAMERA_HPP

#include <glm/glm.hpp>
#include "principle/scene/camera.hpp"

namespace principle
{
	class DynamicCamera : public Camera
	{
	public:
		DynamicCamera() = default;
		~DynamicCamera() = default;

		int get_viewport_width() const;
		void set_viewport_width(int value);

		int get_viewport_height() const;
		void set_viewport_height(int value);

		float get_field_of_view() const;
		void set_field_of_view(float value);

		float get_near_plane() const;
		void set_near_plane(float value);

		float get_far_plane() const;
		void set_far_plane(float value);

		float get_vertical_rotation() const;
		void set_vertical_rotation(float value);

		float get_horizontal_rotation() const;
		void set_horizontal_rotation(float value);

		float get_distance() const;
		void set_distance(float value);

		float get_focus_duration() const;
		void set_focus_duration(float value);

		glm::vec3 get_target() const;
		void set_target(const glm::vec3& value);

		glm::vec3 get_forward() const;
		glm::vec3 get_strafe_forward() const;
		glm::vec3 get_left() const;
		glm::vec3 get_strafe_left() const;
		glm::vec3 get_up() const;

		void focus(const glm::vec3& target);
		void rotate_horizontally(float delta);
		void rotate_vertically(float delta);
		void zoom(float delta);
		void move_forward(float value);
		void move_left(float value);
		void move_up(float value);
		void move_horizontally(float x, float y);
		void move_vertically(float value);

		void update(float delta);

		glm::mat4 get_projection_matrix() const override;
		glm::mat4 get_view_matrix() const override;

	private:
		int viewport_width = 1, viewport_height = 1;
		float field_of_view = M_PI / 4.0f;
		float near_plane = 1.0f, far_plane = 512.0f * 128.0f;

		float vertical_rotation = 0.0f, horizontal_rotation = -(M_PI / 2.0f);
		float current_distance = 2048.0f;
		float target_distance = 2048.0f;
		float distance_delta = 1.0f;

		float focus_duration = 0.25f;

		glm::vec3 current_position = glm::vec3(0.0f);
		glm::vec3 target_position = glm::vec3(0.0f);
		float position_delta = 0.0f;
	};
}

#endif
