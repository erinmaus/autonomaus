// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kvre/ui/flatDrawBuffer.hpp"

kvre::FlatDrawBuffer::FlatDrawBuffer(
	const kompvter::VertexAttributes& vertex_attributes,
	const Profile* profile)
{
	this->vertex_attributes = vertex_attributes;
	if (profile == nullptr)
	{
		this->profile = Profile::get_default();
	}
	else
	{
		this->profile = profile;
	}

	this->positions_fetcher = this->profile->get_flat_vertex_positions_fetcher(
		this->vertex_attributes);
	this->colors_fetcher = this->profile->get_flat_vertex_colors_fetcher(
		this->vertex_attributes);
	this->texture_coordinates_fetcher = this->profile->get_flat_vertex_texture_coordinates_fetcher(
		this->vertex_attributes);
	this->atlas_mins_fetcher = this->profile->get_flat_vertex_atlas_mins_fetcher(
		this->vertex_attributes);
	this->atlas_maxes_fetcher = this->profile->get_flat_vertex_atlas_maxes_fetcher(
		this->vertex_attributes);

	assert(this->positions_fetcher != nullptr);
	assert(this->colors_fetcher != nullptr);
	assert(this->texture_coordinates_fetcher != nullptr);
	assert(this->atlas_mins_fetcher != nullptr);
	assert(this->atlas_maxes_fetcher != nullptr);

	this->min_vertex = 0;
	this->max_vertex = get_num_vertices();
}

kvre::FlatDrawBuffer::FlatDrawBuffer(
	const kompvter::GUIDrawEvent& event,
	const Profile* profile)
{
	this->vertex_attributes = event.get_vertex_attributes();
	if (profile == nullptr)
	{
		this->profile = Profile::get_default();
	}
	else
	{
		this->profile = profile;
	}

	this->positions_fetcher = this->profile->get_flat_vertex_positions_fetcher(
		this->vertex_attributes);
	this->colors_fetcher = this->profile->get_flat_vertex_colors_fetcher(
		this->vertex_attributes);
	this->texture_coordinates_fetcher = this->profile->get_flat_vertex_texture_coordinates_fetcher(
		this->vertex_attributes);
	this->atlas_mins_fetcher = this->profile->get_flat_vertex_atlas_mins_fetcher(
		this->vertex_attributes);
	this->atlas_maxes_fetcher = this->profile->get_flat_vertex_atlas_maxes_fetcher(
		this->vertex_attributes);

	assert(this->positions_fetcher != nullptr);
	assert(this->colors_fetcher != nullptr);
	assert(this->texture_coordinates_fetcher != nullptr);
	assert(this->atlas_mins_fetcher != nullptr);
	assert(this->atlas_maxes_fetcher != nullptr);

	auto& index_mapper = event.get_vertex_index_mapper();
	auto& draw_parameters = event.get_draw_parameters();
	this->min_vertex = draw_parameters.get_offset_start() / index_mapper.get_index_size() / 6 * 4;
	this->max_vertex = draw_parameters.get_offset_stop() / index_mapper.get_index_size() / 6 * 4;
}

kvre::FlatDrawBuffer::FlatDrawBuffer(const FlatDrawBuffer& other)
{
	*this = other;
}

std::size_t kvre::FlatDrawBuffer::get_num_vertices() const
{
	if (this->num_vertices == 0)
	{
		this->num_vertices = this->profile->get_num_flat_vertices(
			this->vertex_attributes);
	}

	return this->num_vertices;
}

glm::vec2 kvre::FlatDrawBuffer::get_position(std::size_t index) const
{
	if (this->positions.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->positions.resize(num_vertices);
		this->profile->get_flat_vertex_positions(
			*this->positions_fetcher,
			this->min_vertex, this->max_vertex - this->min_vertex,
			&this->positions[this->min_vertex], sizeof(glm::vec2));
	}

	return this->positions[index];
}

glm::vec4 kvre::FlatDrawBuffer::get_color(std::size_t index) const
{
	if (this->colors.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->colors.resize(num_vertices);
		this->profile->get_flat_vertex_colors(
			*this->colors_fetcher,
			this->min_vertex, this->max_vertex - this->min_vertex,
			&this->colors[this->min_vertex], sizeof(glm::vec4));
	}

	return this->colors[index];
}

glm::vec2 kvre::FlatDrawBuffer::get_texture_coordinate(std::size_t index) const
{
	if (this->texture_coordinates.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->texture_coordinates.resize(num_vertices);
		this->profile->get_flat_vertex_texture_coordinates(
			*this->texture_coordinates_fetcher,
			this->min_vertex, this->max_vertex - this->min_vertex,
			&this->texture_coordinates[this->min_vertex], sizeof(glm::vec2));
	}

	return this->texture_coordinates[index];
}

glm::vec2 kvre::FlatDrawBuffer::get_texture_atlas_min(std::size_t index) const
{
	if (this->texture_atlas_mins.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->texture_atlas_mins.resize(num_vertices);
		this->profile->get_flat_vertex_atlas_mins(
			*this->atlas_mins_fetcher,
			this->min_vertex, this->max_vertex - this->min_vertex,
			&this->texture_atlas_mins[this->min_vertex], sizeof(glm::vec2));
	}

	return this->texture_atlas_mins[index];
}

glm::vec2 kvre::FlatDrawBuffer::get_texture_atlas_max(std::size_t index) const
{
	if (this->texture_atlas_maxes.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->texture_atlas_maxes.resize(num_vertices);
		this->profile->get_flat_vertex_atlas_maxes(
			*this->atlas_maxes_fetcher,
			this->min_vertex, this->max_vertex - this->min_vertex,
			&this->texture_atlas_maxes[this->min_vertex], sizeof(glm::vec2));
	}

	return this->texture_atlas_maxes[index];
}

bool kvre::FlatDrawBuffer::is_textured(std::size_t index) const
{
	if (this->textured_vertices.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->textured_vertices.resize(num_vertices);
		for (std::size_t i = 0; i < num_vertices; ++i)
		{
			this->textured_vertices[i] = this->profile->is_flat_vertex_textured(
				get_texture_coordinate(i));
		}
	}

	return this->textured_vertices[index];
}

glm::vec2 kvre::FlatDrawBuffer::compute_wrapped_texture_coordinate(std::size_t index) const
{
	auto texture_coordinate = get_texture_coordinate(index);
	auto min = get_texture_atlas_min(index);
	auto max = get_texture_atlas_max(index);

	// XXX: Edge-case: if min == 0 and max == 1, and any component of
	// texture_coordinate == 0 or == 1, then the wrap produces a bad value.
	if (min.x == 0.0f && min.y == 0.0f && max.x == 1.0f && max.y == 1.0f)
	{
		return texture_coordinate;
	}

	auto size = max - min;
	auto texture_relative_min = texture_coordinate - min;
	return min + glm::mod(texture_relative_min, size);
}

kvre::FlatDrawBuffer& kvre::FlatDrawBuffer::operator =(
	const kvre::FlatDrawBuffer& other)
{
	this->vertex_attributes = other.vertex_attributes;
	this->profile = other.profile;

	this->positions_fetcher = this->profile->get_flat_vertex_positions_fetcher(
		this->vertex_attributes);
	this->colors_fetcher = this->profile->get_flat_vertex_colors_fetcher(
		this->vertex_attributes);
	this->texture_coordinates_fetcher = this->profile->get_flat_vertex_texture_coordinates_fetcher(
		this->vertex_attributes);
	this->atlas_mins_fetcher = this->profile->get_flat_vertex_atlas_mins_fetcher(
		this->vertex_attributes);
	this->atlas_maxes_fetcher = this->profile->get_flat_vertex_atlas_maxes_fetcher(
		this->vertex_attributes);

	assert(this->positions_fetcher != nullptr);
	assert(this->colors_fetcher != nullptr);
	assert(this->texture_coordinates_fetcher != nullptr);
	assert(this->atlas_mins_fetcher != nullptr);
	assert(this->atlas_maxes_fetcher != nullptr);

	this->num_vertices = other.num_vertices;
	this->positions.clear();
	this->colors.clear();
	this->texture_coordinates.clear();
	this->texture_atlas_mins.clear();
	this->texture_atlas_maxes.clear();
	this->textured_vertices.clear();

	return *this;
}
