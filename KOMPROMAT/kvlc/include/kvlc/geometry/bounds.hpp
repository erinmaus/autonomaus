// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVLC_GEOMETRY_BOUNDS_HPP
#define KVLC_GEOMETRY_BOUNDS_HPP

#include <glm/glm.hpp>

namespace kvlc
{
	struct Bounds
	{
		Bounds() = default;
		Bounds(const glm::vec3& min, const glm::vec3& max);

		void expand(const glm::vec3& point);
		void expand(const Bounds& bounds);
		void transform(const glm::mat4& transform);

		bool contains(const glm::vec3& point) const;
		bool intersects_triangle(
			const glm::vec3& a, const glm::vec3& b, const glm::vec3& c);

		static bool inclusive_intersection(const Bounds& a, const Bounds& b);
		static bool exclusive_intersection(const Bounds& a, const Bounds& b);

		glm::vec3 compute_center() const;
		glm::vec3 compute_size() const;

		glm::vec3 min = glm::vec3(0.0f);
		glm::vec3 max = glm::vec3(0.0f);
	};
}

#endif
