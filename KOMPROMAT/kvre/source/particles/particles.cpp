// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kvre/draw/rangeVertexIterator.hpp"
#include "kvre/particles/particles.hpp"

kvre::Particles::Particles(
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

	this->positions_fetcher = this->profile->get_particle_vertex_positions_fetcher(
		this->vertex_attributes);
	this->colors_fetcher = this->profile->get_particle_vertex_colors_fetcher(
		this->vertex_attributes);
	this->texture_coordinates_fetcher = this->profile->get_particle_vertex_texture_coordinates_fetcher(
		this->vertex_attributes);
	this->texture_metas_fetcher = this->profile->get_particle_vertex_texture_metas_fetcher(
		this->vertex_attributes);

	this->is_billboard = this->profile->get_is_billboard(attributes);
}

kvre::Particles::Particles(const Particles& other)
{
	*this = other;
}

glm::vec3 kvre::Particles::get_position(std::size_t index) const
{
	if (this->positions.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->positions.resize(this->get_num_vertices(), glm::vec3(0.0f));
		if (this->positions_fetcher != nullptr)
		{
			for (std::size_t i = 0; i < num_vertices; ++i)
			{
				this->positions[i] = this->profile->get_particle_vertex_position(
					*this->positions_fetcher, i, get_is_billboard());
			}
		}
	}

	return this->positions[index];
}

glm::vec4 kvre::Particles::get_color(std::size_t index) const
{
	if (this->colors.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->colors.resize(this->get_num_vertices(), glm::vec4(1.0f));
		if (this->colors_fetcher != nullptr)
		{
			for (std::size_t i = 0; i < num_vertices; ++i)
			{
				this->colors[i] = this->profile->get_particle_vertex_color(
					*this->colors_fetcher, i, get_is_billboard());
			}
		}
	}

	return this->colors[index];
}

glm::vec2 kvre::Particles::get_texture_coordinate(std::size_t index) const
{
	if (this->texture_coordinates.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->texture_coordinates.resize(this->get_num_vertices(), glm::vec2(0.0f));
		if (this->texture_coordinates_fetcher != nullptr)
		{
			for (std::size_t i = 0; i < num_vertices; ++i)
			{
				this->texture_coordinates[i] = this->profile->get_particle_vertex_texture_coordinate(
					*this->texture_coordinates_fetcher, i, get_is_billboard());
			}
		}
	}

	return this->texture_coordinates[index];
}

glm::vec4 kvre::Particles::get_texture_meta(std::size_t index) const
{
	if (this->texture_metas.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->texture_metas.resize(this->get_num_vertices(), glm::vec4(0.0f));
		if (this->texture_metas_fetcher != nullptr)
		{
			for (std::size_t i = 0; i < num_vertices; ++i)
			{
				this->texture_metas[i] = this->profile->get_particle_vertex_texture_meta(
					*this->texture_metas_fetcher, i, get_is_billboard());
			}
		}
	}

	return this->texture_metas[index];
}

glm::vec2 kvre::Particles::compute_atlas_texture_coordinates(std::size_t index)
{
	if (has_texture_coordinates() && has_texture_metas())
	{
		auto meta = this->profile->get_default_atlas_meta();
		return this->profile->compute_texture_coordinate_to_atlas(
			this->get_texture_coordinate(index),
			this->get_texture_meta(index),
			meta);
	}

	return glm::vec2(0.0f);
}

bool kvre::Particles::has_positions() const
{
	return this->positions_fetcher != nullptr;
}

bool kvre::Particles::has_colors() const
{
	return this->colors_fetcher != nullptr;
}

bool kvre::Particles::has_texture_coordinates() const
{
	return this->texture_coordinates_fetcher != nullptr;
}

bool kvre::Particles::has_texture_metas() const
{
	return this->texture_metas_fetcher != nullptr;
}

const kompvter::VertexAttributes& kvre::Particles::get_vertex_attributes() const
{
	return this->vertex_attributes;
}

const kvre::Profile* kvre::Particles::get_profile() const
{
	return this->profile;
}

std::size_t kvre::Particles::get_num_vertices() const
{
	if (this->num_vertices == 0)
	{
		this->num_vertices = this->profile->get_num_model_vertices(
			this->vertex_attributes);
	}

	return this->num_vertices;
}

bool kvre::Particles::get_is_billboard() const
{
	return this->is_billboard;
}

kvre::Particles& kvre::Particles::operator =(const Particles& other)
{
	this->vertex_attributes = other.vertex_attributes;
	this->profile = other.profile;

	this->positions_fetcher = this->profile->get_particle_vertex_positions_fetcher(
		this->vertex_attributes);
	this->colors_fetcher = this->profile->get_particle_vertex_colors_fetcher(
		this->vertex_attributes);
	this->texture_coordinates_fetcher = this->profile->get_particle_vertex_texture_coordinates_fetcher(
		this->vertex_attributes);
	this->texture_metas_fetcher = this->profile->get_particle_vertex_texture_metas_fetcher(
		this->vertex_attributes);

	this->num_vertices = other.num_vertices;
	this->positions.clear();
	this->colors.clear();
	this->texture_coordinates.clear();
	this->texture_metas.clear();

	this->is_billboard = this->profile->get_is_billboard(
		this->vertex_attributes);

	return *this;
}
