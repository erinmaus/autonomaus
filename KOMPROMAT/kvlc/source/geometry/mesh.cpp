// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvlc/geometry/mesh.hpp"

kvlc::Mesh::Point* kvlc::Mesh::get_points()
{
	return &this->points[0];
}

const kvlc::Mesh::Point* kvlc::Mesh::get_points() const
{
	return &this->points[0];
}

std::size_t kvlc::Mesh::get_num_points() const
{
	return this->points.size();
}

void kvlc::Mesh::set_num_points(std::size_t count)
{
	this->points.resize(count, kvlc::Mesh::Point(0.0f));
}

std::uint32_t* kvlc::Mesh::get_triangles()
{
	return &this->triangles[0];
}

const std::uint32_t* kvlc::Mesh::get_triangles() const
{
	return &this->triangles[0];
}

std::size_t kvlc::Mesh::get_num_triangles() const
{
	assert(this->triangles.size() % 3 == 0);
	return this->triangles.size() / 3;
}

void kvlc::Mesh::set_num_triangles(std::size_t count)
{
	this->triangles.resize(count * 3, 0);
}
