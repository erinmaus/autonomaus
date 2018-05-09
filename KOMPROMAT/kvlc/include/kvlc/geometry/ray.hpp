// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVLC_GEOMETRY_RAY_HPP
#define KVLC_GEOMETRY_RAY_HPP

#include <glm/glm.hpp>

namespace kvlc
{
	struct Ray
	{
		Ray() = default;
		Ray(const glm::vec3& position, const glm::vec3& direction);

		float distance(const glm::vec3& point);

		glm::vec3 position = glm::vec3(0.0f);
		glm::vec3 direction = glm::vec3(0.0f, 1.0f, 0.0f);
	};
}

#endif
