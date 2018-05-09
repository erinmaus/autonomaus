// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_CAMERA_HPP
#define AUTONOMAUS_STATE_CAMERA_HPP

#include <glm/glm.hpp>

namespace autonomaus
{
	class DrawState;

	class Camera
	{
	public:
		Camera() = default;
		Camera(
			const glm::mat4& view,
			const glm::mat4& projection,
			const glm::ivec4& viewport,
			int window_width, int window_height);
		explicit Camera(const DrawState& draw_state);

		const glm::mat4& get_view() const;
		const glm::mat4& get_projection() const;
		const glm::ivec4& get_viewport() const;

		glm::vec3 world_to_screen(const glm::vec3& point) const;
		glm::vec3 screen_to_world(const glm::vec3& point) const;
		bool is_visible(const glm::vec3& point) const;
		bool is_visible(const glm::vec2& point) const;

	private:
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);
		glm::ivec4 viewport = glm::ivec4(0);
		glm::ivec4 adjusted_viewport = glm::ivec4(0);
		int window_width = 0;
		int window_height = 0;
	};
}

#endif
