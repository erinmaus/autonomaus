// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_CAMERA_HPP
#define PRINCIPLE_SCENE_CAMERA_HPP

#include <glm/glm.hpp>
#include "kvncer/graphics/shader.hpp"

namespace principle
{
	class Camera
	{
	public:
		Camera() = default;
		virtual ~Camera() = default;

		virtual glm::mat4 get_projection_matrix() const = 0;
		virtual glm::mat4 get_view_matrix() const = 0;

		virtual void apply(kvncer::Shader* shader) const;
	};
}

#endif
