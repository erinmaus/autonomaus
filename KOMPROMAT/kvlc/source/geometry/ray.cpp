// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvlc/geometry/ray.hpp"

kvlc::Ray::Ray(const glm::vec3& position, const glm::vec3& direction)
{
	this->position = position;
	this->direction = direction;
}

float kvlc::Ray::distance(const glm::vec3& point)
{
	auto ray_point_difference  = this->position - point;
	float dot = glm::dot(ray_point_difference, this->direction);
	if (dot <= 0.0f)
	{
		return glm::length(ray_point_difference);
	}

	auto projection = this->position + dot * this->direction;
	return glm::length(projection - point);
}
