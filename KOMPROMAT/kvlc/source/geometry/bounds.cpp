// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/type_ptr.hpp>
#include "kvlc/geometry/bounds.hpp"

kvlc::Bounds::Bounds(const glm::vec3& min, const glm::vec3& max)
	: min(min), max(max)
{
	// Nil.
}

void kvlc::Bounds::expand(const glm::vec3& point)
{
	this->min = glm::min(this->min, point);
	this->max = glm::max(this->max, point);
}

void kvlc::Bounds::expand(const Bounds& bounds)
{
	expand(bounds.min);
	expand(bounds.max);
}

bool kvlc::Bounds::contains(const glm::vec3& point) const
{
	if (point.x < this->min.x || point.x > this->max.x)
	{
		return false;
	}

	if (point.y < this->min.y || point.y > this->max.y)
	{
		return false;
	}

	if (point.z < this->min.z || point.z > this->max.z)
	{
		return false;
	}

	return true;
}

static void project(
	const glm::vec3 points[], std::size_t count,
	const glm::vec3& axis,
	float& result_min, float& result_max)
{
	result_min = HUGE_VALF;
	result_max = -HUGE_VALF;
	for (std::size_t i = 0; i < count; ++i)
	{
		float d = glm::dot(axis, points[i]);
		result_min = glm::min(result_min, d);
		result_max = glm::max(result_max, d);
	}
}

bool kvlc::Bounds::intersects_triangle(
	const glm::vec3& a, const glm::vec3& b, const glm::vec3& c)
{
	glm::vec3 triangle[3] = { a, b, c };
	glm::vec3 triangle_u = triangle[1] - triangle[0];
	glm::vec3 triangle_v = triangle[2] - triangle[0];
	glm::vec3 triangle_normal = glm::normalize(glm::cross(triangle_u, triangle_v));

	const glm::vec3 box_normals[] = {
		glm::vec3(1.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 1.0)
	};
	glm::vec3 box_points[] = {
		glm::vec3(this->min.x, this->min.y, this->min.z),
		glm::vec3(this->max.x, this->min.y, this->min.z),
		glm::vec3(this->min.x, this->max.y, this->min.z),
		glm::vec3(this->min.x, this->min.y, this->max.z),
		glm::vec3(this->max.x, this->max.y, this->min.z),
		glm::vec3(this->max.x, this->min.y, this->max.z),
		glm::vec3(this->min.x, this->max.y, this->max.z),
		glm::vec3(this->max.x, this->max.y, this->max.z)
	};

	float triangle_min, triangle_max;
	float box_min, box_max;

	for (int i = 0; i < 3; ++i)
	{
		project(triangle, 3, box_normals[i], triangle_min, triangle_max);
		if (triangle_max < this->min[i] || triangle_min > this->max[i])
		{
			return false;
		}
	}

	float triangle_d = glm::dot(triangle_normal, triangle[0]);
	project(box_points, 8, triangle_normal, box_min, box_max);
	if (box_max < triangle_d || box_min > triangle_d)
	{
		return false;
	}

	glm::vec3 edges[] = {
		a - b,
		b - c,
		c - a
	};
	for (int i = 0; i < 3; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			glm::vec3 normal = glm::normalize(glm::cross(edges[i], box_normals[j]));
			project(box_points, 8, normal, box_min, box_max);
			project(triangle, 3, normal, triangle_min, triangle_max);
			if (box_max <= triangle_min || box_min >= triangle_max)
			{
				return false;
			}
		}
	}

	return true;
}

void kvlc::Bounds::transform(const glm::mat4& transform)
{
	glm::vec4 points[] = {
		glm::vec4(this->min.x, this->min.y, this->min.z, 1.0f),
		glm::vec4(this->max.x, this->min.y, this->min.z, 1.0f),
		glm::vec4(this->min.x, this->max.y, this->min.z, 1.0f),
		glm::vec4(this->min.x, this->min.y, this->max.z, 1.0f),
		glm::vec4(this->max.x, this->max.y, this->min.z, 1.0f),
		glm::vec4(this->max.x, this->min.y, this->max.z, 1.0f),
		glm::vec4(this->min.x, this->max.y, this->max.z, 1.0f),
		glm::vec4(this->max.x, this->max.y, this->max.z, 1.0f)
	};

	for (std::size_t i = 0; i < 8; ++i)
	{
		points[i] = transform * points[i];
	}

	this->min = glm::vec3(points[0].x, points[0].y, points[0].z);
	this->max = glm::vec3(points[0].x, points[0].y, points[0].z);
	for (std::size_t i = 1; i < 8; ++i)
	{
		expand(glm::vec3(points[i].x, points[i].y, points[i].z));
	}
}

bool kvlc::Bounds::inclusive_intersection(
	const kvlc::Bounds& a, const kvlc::Bounds& b)
{
	if (a.min.x > b.max.x || a.min.y > b.max.y || a.min.z > b.max.z)
	{
		return false;
	}

	if (b.min.x > a.max.x || b.min.y > a.max.y || b.min.z > a.max.z)
	{
		return false;
	}

	return true;
}


bool kvlc::Bounds::exclusive_intersection(
	const kvlc::Bounds& a, const kvlc::Bounds& b)
{
	if (a.min.x >= b.max.x || a.min.y >= b.max.y || a.min.z >= b.max.z)
	{
		return false;
	}

	if (b.min.x >= a.max.x || b.min.y >= a.max.y || b.min.z >= a.max.z)
	{
		return false;
	}

	return true;
}

glm::vec3 kvlc::Bounds::compute_center() const
{
	return min + compute_size() / 2.0f;
}

glm::vec3 kvlc::Bounds::compute_size() const
{
	return max - min;
}
