// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVLC_GEOMETRY_MESH_HPP
#define KVLC_GEOMETRY_MESH_HPP

#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include <ode/ode.h>

namespace kvlc
{
	class Mesh
	{
	public:
		typedef glm::tvec4<dReal, glm::highp> Point;

		Mesh() = default;
		~Mesh() = default;

		Point* get_points();
		const Point* get_points() const;

		std::size_t get_num_points() const;
		void set_num_points(std::size_t count);

		std::uint32_t* get_triangles();
		const std::uint32_t* get_triangles() const;

		std::size_t get_num_triangles() const;
		void set_num_triangles(std::size_t count);

	private:
		std::vector<Point> points;
		std::vector<std::uint32_t> triangles;
	};
}

#endif
