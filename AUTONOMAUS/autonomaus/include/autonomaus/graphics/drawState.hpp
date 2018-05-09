// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_DRAW_STATE_HPP
#define AUTONOMAUS_GRAPHICS_DRAW_STATE_HPP

#include <glm/glm.hpp>

namespace autonomaus
{
	class DrawState
	{
	public:
		DrawState() = default;
		~DrawState() = default;

		const glm::mat4& get_camera_view_matrix() const;
		void set_camera_view_matrix(const glm::mat4& value);
		const glm::mat4& get_camera_projection_matrix() const;
		void set_camera_projection_matrix(const glm::mat4& value);

		const glm::ivec4& get_world_viewport() const;
		void set_world_viewport(const glm::ivec4& value);
		const glm::ivec4& get_gui_viewport() const;
		void set_gui_viewport(const glm::ivec4& value);

	private:
		glm::mat4 camera_view_matrix = glm::mat4(1.0f);
		glm::mat4 camera_projection_matrix = glm::mat4(1.0f);
		glm::ivec4 world_viewport = glm::ivec4(0);
		glm::ivec4 gui_viewport = glm::ivec4(0);
	};
}

#endif
