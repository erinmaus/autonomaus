// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvre/terrain/terrain.hpp"

kvre::Terrain::Terrain(
	const kompvter::VertexAttributes& attributes,
	const Profile* profile)
{
	this->vertex_attributes = attributes;
	if (profile == nullptr)
	{
		this->profile = Profile::get_default();
	}
	else
	{
		this->profile = profile;
	}

	this->positions_fetcher = this->profile->get_terrain_vertex_positions_fetcher(
		this->vertex_attributes);
	this->colors_fetcher = this->profile->get_terrain_vertex_colors_fetcher(
		this->vertex_attributes);

	assert(this->positions_fetcher != nullptr);
	assert(this->colors_fetcher != nullptr);
}

kvre::Terrain::Terrain(const kvre::Terrain& other)
{
	*this = other;
}

const kompvter::VertexAttributes& kvre::Terrain::get_vertex_attributes() const
{
	return this->vertex_attributes;
}

const kvre::Profile* kvre::Terrain::get_profile() const
{
	return this->profile;
}

std::size_t kvre::Terrain::get_num_vertices() const
{
	if (this->num_vertices == 0)
	{
		this->num_vertices = this->profile->get_num_terrain_vertices(
			this->vertex_attributes);
	}

	return this->num_vertices;
}

glm::vec3 kvre::Terrain::get_position(std::size_t index) const
{
	if (this->positions.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->positions.resize(this->get_num_vertices());
		for (std::size_t i = 0; i < num_vertices; ++i)
		{
			this->positions[i] = this->profile->get_terrain_vertex_position(
				*this->positions_fetcher, i);
		}
	}

	return this->positions[index];
}

glm::vec4 kvre::Terrain::get_color(std::size_t index) const
{
	if (this->colors.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->colors.resize(this->get_num_vertices());
		for (std::size_t i = 0; i < num_vertices; ++i)
		{
			this->colors[i] = this->profile->get_terrain_vertex_color(
				*this->colors_fetcher, i);
		}
	}

	return this->colors[index];
}

kvre::Terrain& kvre::Terrain::operator =(const kvre::Terrain& other)
{
	this->vertex_attributes = other.vertex_attributes;
	this->profile = other.profile;

	this->positions_fetcher = this->profile->get_terrain_vertex_positions_fetcher(
		this->vertex_attributes);
	this->colors_fetcher = this->profile->get_terrain_vertex_colors_fetcher(
		this->vertex_attributes);

	assert(this->positions_fetcher != nullptr);
	assert(this->colors_fetcher != nullptr);

	this->num_vertices = other.num_vertices;

	this->positions.clear();
	this->colors.clear();

	return *this;
}
