// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_POINT_MATERIAL_HPP
#define PRINCIPLE_SCENE_POINT_MATERIAL_HPP

#include "principle/scene/material.hpp"

namespace principle
{
	class PointMaterial : public Material
	{
	public:
		PointMaterial() = default;
		~PointMaterial() = default;

		float get_point_size() const;
		void set_point_size(float value);

		glm::vec4 get_color() const;
		void set_color(const glm::vec4& color);

		void use() override;

	protected:
		void build() override;

	private:
		float point_size = 1.0f;
		glm::vec4 color = glm::vec4(1.0f);
	};
}

#endif
