// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kvre/draw/rangeVertexIterator.hpp"
#include "kvre/model/model.hpp"

kvre::Model::Model(
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

	this->positions_fetcher = this->profile->get_model_vertex_positions_fetcher(
		this->vertex_attributes);
	this->bones_fetcher = this->profile->get_model_vertex_bones_fetcher(
		this->vertex_attributes);
	this->colors_fetcher = this->profile->get_model_vertex_colors_fetcher(
		this->vertex_attributes);
	this->texture_coordinates_fetcher = this->profile->get_model_vertex_texture_coordinates_fetcher(
		this->vertex_attributes);
	this->texture_metas_fetcher = this->profile->get_model_vertex_texture_metas_fetcher(
		this->vertex_attributes);
	this->normals_fetcher = this->profile->get_model_vertex_normals_fetcher(
		this->vertex_attributes);
}

kvre::Model::Model(const Model& other)
{
	*this = other;
}

glm::vec3 kvre::Model::get_position(std::size_t index) const
{
	if (this->positions.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->positions.resize(this->get_num_vertices(), glm::vec3(0.0f));
		if (this->positions_fetcher != nullptr)
		{
			for (std::size_t i = 0; i < num_vertices; ++i)
			{
				this->positions[i] = this->profile->get_model_vertex_position(
					*this->positions_fetcher, i);
			}
		}
	}

	return this->positions[index];
}

int kvre::Model::get_bone(std::size_t index) const
{
	if (this->bones.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->bones.resize(this->get_num_vertices(), 0);
		if (this->bones_fetcher != nullptr)
		{
			for (std::size_t i = 0; i < num_vertices; ++i)
			{
				this->bones[i] = this->profile->get_model_vertex_bone(
					*this->bones_fetcher, i);
			}
		}
	}

	return this->bones[index];
}

glm::vec4 kvre::Model::get_color(std::size_t index) const
{
	if (this->colors.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->colors.resize(this->get_num_vertices(), glm::vec4(1.0f));
		if (this->colors_fetcher != nullptr)
		{
			for (std::size_t i = 0; i < num_vertices; ++i)
			{
				this->colors[i] = this->profile->get_model_vertex_color(
					*this->colors_fetcher, i);
			}
		}
	}

	return this->colors[index];
}

glm::vec2 kvre::Model::get_texture_coordinate(std::size_t index) const
{
	if (this->texture_coordinates.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->texture_coordinates.resize(this->get_num_vertices(), glm::vec2(0.0f));
		if (this->texture_coordinates_fetcher != nullptr)
		{
			for (std::size_t i = 0; i < num_vertices; ++i)
			{
				this->texture_coordinates[i] = this->profile->get_model_vertex_texture_coordinate(
					*this->texture_coordinates_fetcher, i);
			}
		}
	}

	return this->texture_coordinates[index];
}

glm::vec4 kvre::Model::get_texture_meta(std::size_t index) const
{
	if (this->texture_metas.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->texture_metas.resize(this->get_num_vertices(), glm::vec4(0.0f));
		if (this->texture_metas_fetcher != nullptr)
		{
			for (std::size_t i = 0; i < num_vertices; ++i)
			{
				this->texture_metas[i] = this->profile->get_model_vertex_texture_meta(
					*this->texture_metas_fetcher, i);
			}
		}
	}

	return this->texture_metas[index];
}

glm::vec3 kvre::Model::get_normal(std::size_t index) const
{
	if (this->normals.empty())
	{
		std::size_t num_vertices = this->get_num_vertices();
		this->normals.resize(this->get_num_vertices(), glm::vec3(0.0f, 1.0f, 0.0f));
		if (this->normals_fetcher != nullptr)
		{
			for (std::size_t i = 0; i < num_vertices; ++i)
			{
				this->normals[i] = this->profile->get_model_vertex_normal(
					*this->normals_fetcher, i);
			}
		}
	}

	return this->normals[index];
}

glm::vec2 kvre::Model::compute_atlas_texture_coordinates(std::size_t index)
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

bool kvre::Model::has_positions() const
{
	return this->positions_fetcher != nullptr;
}

bool kvre::Model::has_bones() const
{
	return this->bones_fetcher != nullptr;
}

bool kvre::Model::has_colors() const
{
	return this->colors_fetcher != nullptr;
}

bool kvre::Model::has_texture_coordinates() const
{
	return this->texture_coordinates_fetcher != nullptr;
}

bool kvre::Model::has_texture_metas() const
{
	return this->texture_metas_fetcher != nullptr;
}

bool kvre::Model::has_normals() const
{
	return this->normals_fetcher != nullptr;
}


const kompvter::VertexAttributes& kvre::Model::get_vertex_attributes() const
{
	return this->vertex_attributes;
}

const kvre::Profile* kvre::Model::get_profile() const
{
	return this->profile;
}

std::size_t kvre::Model::get_num_vertices() const
{
	if (this->num_vertices == 0)
	{
		this->num_vertices = this->profile->get_num_model_vertices(
			this->vertex_attributes);
	}

	return this->num_vertices;
}

kvlc::Bounds kvre::Model::get_bounds() const
{
	if (!this->bounds_valid)
	{
		refresh_bounds();
	}

	return this->bounds;
}

kvre::Model& kvre::Model::operator =(const Model& other)
{
	this->vertex_attributes = other.vertex_attributes;
	this->profile = other.profile;

	this->positions_fetcher = this->profile->get_model_vertex_positions_fetcher(
		this->vertex_attributes);
	this->bones_fetcher = this->profile->get_model_vertex_bones_fetcher(
		this->vertex_attributes);
	this->colors_fetcher = this->profile->get_model_vertex_colors_fetcher(
		this->vertex_attributes);
	this->texture_coordinates_fetcher = this->profile->get_model_vertex_texture_coordinates_fetcher(
		this->vertex_attributes);
	this->texture_metas_fetcher = this->profile->get_model_vertex_texture_metas_fetcher(
		this->vertex_attributes);
	this->normals_fetcher = this->profile->get_model_vertex_normals_fetcher(
		this->vertex_attributes);

	this->num_vertices = other.num_vertices;
	this->positions.clear();
	this->bones.clear();
	this->colors.clear();
	this->texture_coordinates.clear();
	this->texture_metas.clear();
	this->normals.clear();

	return *this;
}

void kvre::Model::refresh_bounds() const
{
	kvlc::Bounds b(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));

	RangeVertexIterator begin(0), end(get_num_vertices());
	for_each_position(
		begin, end,
		[&b](const glm::vec3& element, std::size_t)
		{
			b.expand(element);
		});

	if (b.min != glm::vec3(HUGE_VALF) && b.max != glm::vec3(-HUGE_VALF))
	{
		this->bounds = b;
		this->bounds_valid = true;
	}
}
