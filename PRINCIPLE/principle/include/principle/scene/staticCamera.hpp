// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_STATIC_CAMERA_HPP
#define PRINCIPLE_SCENE_STATIC_CAMERA_HPP

#include <glm/glm.hpp>
#include "principle/scene/camera.hpp"

namespace principle
{
	class StaticCamera : public Camera
	{
	public:
		StaticCamera() = default;
		StaticCamera(const glm::mat4& projection, const glm::mat4& view);
		~StaticCamera() = default;

		void set_projection_matrix(const glm::mat4& value);
		glm::mat4 get_projection_matrix() const override;

		void set_view_matrix(const glm::mat4& value);
		glm::mat4 get_view_matrix() const override;

	private:
		glm::mat4 projection = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
	};
}

#endif
