// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_SHAPES_HPP
#define AUTONOMAUS_GRAPHICS_SHAPES_HPP

#include "autonomaus/graphics/mesh.hpp"

namespace autonomaus
{
	void make_box(
		Mesh& mesh,
		const glm::vec3& size,
		const glm::vec4& color);
	void make_box_outline(
		Mesh& mesh,
		const glm::vec3& size,
		const glm::vec4& color);
	void make_sphere(
		Mesh& mesh,
		float radius,
		const glm::vec4& color);
}

#endif
