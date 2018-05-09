// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cmath>
#include <set>
#include <glm/gtc/type_ptr.hpp>
#include "continent1/candidate.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "thoreau/thoreau.hpp"

const glm::vec3 continent1::Candidate::PLANES[] =
{
	glm::vec3(1.0f, 0.0f, 0.0f),
	glm::vec3(-1.0f, 0.0f, 0.0f),
	glm::vec3(0.0f, 0.0f, -1.0f),
	glm::vec3(0.0f, 0.0f, 1.0)
};

continent1::Candidate::Candidate(
	const kvre::Model* model,
	const kompvter::VertexIndexMapper& index_mapper,
	const glm::mat4& model_matrix)
{
	this->model = model;
	this->index_mapper = index_mapper;
	this->model_matrix = model_matrix;
}

void continent1::Candidate::populate(
	const thoreau::Map& map,
	const glm::vec3& token_size,
	float token_elevation)
{
	compute_geometry_data();
	compute_mesh_data(map);
	compute_tile_geometry_data(map, token_size, token_elevation);

	// This populates empty tiles.
	for_each_tile_in_bounds(this->min_tile, this->max_tile, [](auto&, auto&) {});
}

void continent1::Candidate::compute_geometry_data()
{
	this->max_surface_area = 0.0f;
	this->surface_area_sum = 0.0f;
	for (int i = 0; i < normal_max; ++i)
	{
		this->max_surface_area_by_normal[i] = 0.0f;
		this->surface_area_sum_by_normal[i] = 0.0f;
	}

	auto begin = kvre::IndexedVertexIterator::begin(this->index_mapper);
	auto end = kvre::IndexedVertexIterator::end(this->index_mapper);
	if (begin == end)
	{
		return;
	}

	auto current = begin;
	while (current != end)
	{
		glm::vec3 triangle[3];
		for (int i = 0; i < 3; ++i)
		{
			auto index = *current;
			++current;

			triangle[i] = this->model->get_position(index);
		}

		float edge_ab = glm::length(triangle[1] - triangle[0]);
		float edge_bc = glm::length(triangle[2] - triangle[1]);
		float edge_ca = glm::length(triangle[2] - triangle[0]);
		float edge_half_sum = (edge_ab + edge_bc + edge_ca) / 2.0f;
		float area = std::sqrt(
			edge_half_sum *
			(edge_half_sum - edge_ab) *
			(edge_half_sum - edge_bc) *
			(edge_half_sum - edge_ca));

		this->surface_area_sum += area;
		this->max_surface_area = std::max(this->max_surface_area, area);

		glm::vec3 triangle_u = triangle[1] - triangle[0];
		glm::vec3 triangle_v = triangle[2] - triangle[0];
		glm::vec3 normal = glm::normalize(glm::cross(triangle_u, triangle_v));

		normal = glm::abs(normal);
		auto n = glm::value_ptr(normal);
		for (int i = 0; i < 3; ++i)
		{
			bool greater_equal_a = n[i] >= n[(i + 1) % 3];
			bool greater_equal_b = n[i] >= n[(i + 2) % 3];

			if (greater_equal_a && greater_equal_b)
			{
				this->surface_area_sum_by_normal[i] += area;
				this->max_surface_area_by_normal[i] = std::max(
					this->max_surface_area_by_normal[i], area);
			}
		}
	}
}

void continent1::Candidate::compute_tile_geometry_data(
	const thoreau::Map& map,
	const glm::vec3& token_size, float token_elevation)
{
	auto token_half_size = token_size / 2.0f;

	auto begin = kvre::IndexedVertexIterator::begin(this->index_mapper);
	auto end = kvre::IndexedVertexIterator::end(this->index_mapper);
	if (begin == end)
	{
		return;
	}

	auto current = begin;
	while (current != end)
	{
		auto triangle_tile_min = thoreau::TileCoordinate(
			thoreau::TileCoordinate::MAX_COORDINATE,
			thoreau::TileCoordinate::MAX_COORDINATE,
			thoreau::TileCoordinate::MAX_LAYER);
		auto triangle_tile_max = thoreau::TileCoordinate(
			thoreau::TileCoordinate::MIN_COORDINATE,
			thoreau::TileCoordinate::MIN_COORDINATE,
			thoreau::TileCoordinate::MIN_LAYER);
		glm::vec3 triangle[3];
		int num_triangle_vertices = 0;

		for (int i = 0; i < 3; ++i)
		{
			auto index = *current;
			++current;

			auto position = this->model->get_position(index);
			auto transformed_position = this->model_matrix * glm::vec4(position, 1.0f);
			auto tile_coordinate = thoreau::tile_from_world(glm::vec3(transformed_position));
			if (map.has_tile(tile_coordinate.x, tile_coordinate.y))
			{
				map.try_get_tile_layer(
					tile_coordinate.x, tile_coordinate.y,
					transformed_position.y,
					tile_coordinate.layer);
			}

			triangle[i] = glm::vec3(transformed_position);
			++num_triangle_vertices;

			triangle_tile_min.x = std::min(tile_coordinate.x, triangle_tile_min.x);
			triangle_tile_min.y = std::min(tile_coordinate.y, triangle_tile_min.y);
			triangle_tile_min.layer = std::min(tile_coordinate.layer, triangle_tile_min.layer);
			triangle_tile_max.x = std::max(tile_coordinate.x, triangle_tile_max.x);
			triangle_tile_max.y = std::max(tile_coordinate.y, triangle_tile_max.y);
			triangle_tile_max.layer = std::max(tile_coordinate.layer, triangle_tile_max.layer);
		}

		if (num_triangle_vertices != 3)
		{
			continue;
		}

		auto compare_sides = [&triangle, &token_size, &token_elevation, &map](const thoreau::TileCoordinate& tile_coordinate, TileProperties& tile_properties)
		{
			if (!map.has_tile(tile_coordinate))
			{
				return;
			}

			for (int i = 0; i < side_max; ++i)
			{
				auto& tile = map.get_tile(tile_coordinate);

				glm::vec3 world_position;
				world_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
				world_position.y = tile.get_max_elevation();
				world_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

				auto normal = PLANES[i];
				//auto d = glm::abs(glm::dot(normal, token_size)) / 2.0f;
				auto d = 0.0f;

				bool is_match = true;
				for (int j = 0; j < 3; j++)
				{
					if (glm::dot(normal, triangle[j] - world_position) + d > 0.0f)
					{
						is_match = false;
						break;
					}
				}

				if (is_match)
				{
					++tile_properties.num_triangles_on_side[i];
				}
			}
		};

		auto intersects_token = [&triangle, &map, &token_half_size, &token_elevation](const thoreau::TileCoordinate& tile_coordinate, TileProperties& tile_properties)
		{
			if (!map.has_tile(tile_coordinate))
			{
				return;
			}

			auto& tile = map.get_tile(tile_coordinate);
			glm::vec3 world_position;
			world_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
			world_position.y = tile.get_max_elevation() + token_elevation;
			world_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

			kvlc::Bounds bounds;
			bounds.min = world_position - token_half_size;
			bounds.max = world_position + token_half_size;

			if (bounds.intersects_triangle(triangle[0], triangle[1], triangle[2]))
			{
				++tile_properties.num_token_intersections;
			}
			else
			{
				bool contains = true;
				for (int i = 0; i < 3; ++i)
				{
					if (!bounds.contains(triangle[i]))
					{
						contains = false;
						break;
					}
				}

				if (contains)
				{
					++tile_properties.num_token_intersections;
				}
			}
		};

		for_each_tile_in_bounds(triangle_tile_min, triangle_tile_max, compare_sides);
		for_each_tile_in_bounds(triangle_tile_min, triangle_tile_max, intersects_token);
	}
}

void continent1::Candidate::compute_mesh_data(const thoreau::Map& map)
{
	this->num_unique_vertices = 0;
	this->num_vertices = 0;
	this->num_triangles = 0;
	this->bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
	this->min_tile = thoreau::TileCoordinate(
		thoreau::TileCoordinate::MAX_COORDINATE,
		thoreau::TileCoordinate::MAX_COORDINATE,
		thoreau::TileCoordinate::MAX_LAYER);
	this->max_tile = thoreau::TileCoordinate(
		thoreau::TileCoordinate::MIN_COORDINATE,
		thoreau::TileCoordinate::MIN_COORDINATE,
		thoreau::TileCoordinate::MIN_LAYER);

	auto begin = kvre::IndexedVertexIterator::begin(this->index_mapper);
	auto end = kvre::IndexedVertexIterator::end(this->index_mapper);
	if (begin == end)
	{
		return;
	}

	typedef std::tuple<int, int, int> VertexPosition;
	std::set<VertexPosition> unique_positions;

	auto current = begin;
	while (current != end)
	{
		auto triangle_tile_min = thoreau::TileCoordinate(
			thoreau::TileCoordinate::MAX_COORDINATE,
			thoreau::TileCoordinate::MAX_COORDINATE,
			thoreau::TileCoordinate::MAX_LAYER);
		auto triangle_tile_max = thoreau::TileCoordinate(
			thoreau::TileCoordinate::MIN_COORDINATE,
			thoreau::TileCoordinate::MIN_COORDINATE,
			thoreau::TileCoordinate::MIN_LAYER);
		int num_triangle_vertices = 0;

		for (int i = 0; i < 3; ++i)
		{
			auto index = *current;
			++current;

			auto position = this->model->get_position(index);
			auto unique_position = std::make_tuple<int, int, int>(
				position.x, position.y, position.z);

			bool is_unique;
			if (unique_positions.count(unique_position) == 0)
			{
				unique_positions.insert(unique_position);
				++this->num_unique_vertices;
				is_unique = true;
			}
			else
			{
				is_unique = false;
			}
			++this->num_vertices;

			auto transformed_position = glm::vec3(this->model_matrix * glm::vec4(position, 1.0f));
			this->bounds.expand(transformed_position);

			auto tile_coordinate = thoreau::tile_from_world(transformed_position);
			if (map.has_tile(tile_coordinate.x, tile_coordinate.y))
			{
				map.try_get_tile_layer(
					tile_coordinate.x, tile_coordinate.y,
					transformed_position.y, tile_coordinate.layer);
			}

			triangle_tile_min.x = std::min(tile_coordinate.x, triangle_tile_min.x);
			triangle_tile_min.y = std::min(tile_coordinate.y, triangle_tile_min.y);
			triangle_tile_min.layer = std::min(tile_coordinate.layer, triangle_tile_min.layer);
			triangle_tile_max.x = std::max(tile_coordinate.x, triangle_tile_max.x);
			triangle_tile_max.y = std::max(tile_coordinate.y, triangle_tile_max.y);
			triangle_tile_max.layer = std::max(tile_coordinate.layer, triangle_tile_max.layer);

			auto& tile = this->tile_properties[tile_coordinate];
			if (is_unique)
			{
				++tile.num_unique_vertices;
			}
			++tile.num_vertices;

			++num_triangle_vertices;
		}
		++this->num_triangles;

		if (num_triangle_vertices > 0)
		{
			this->min_tile.x = std::min(this->min_tile.x, triangle_tile_min.x);
			this->min_tile.y = std::min(this->min_tile.y, triangle_tile_min.y);
			this->min_tile.layer = std::min(this->min_tile.layer, triangle_tile_min.layer);
			this->max_tile.x = std::max(this->max_tile.x, triangle_tile_max.x);
			this->max_tile.y = std::max(this->max_tile.y, triangle_tile_max.y);
			this->max_tile.layer = std::max(this->max_tile.layer, triangle_tile_max.layer);
		}
		if (num_triangle_vertices == 3)
		{
			for_each_tile_in_bounds(
				triangle_tile_min, triangle_tile_max,
				[](auto&, auto& tile)
				{
					++tile.num_triangles;
				});
		}
	}
}

const kvre::Model* continent1::Candidate::get_model() const
{
	return this->model;
}

const kompvter::VertexIndexMapper& continent1::Candidate::get_vertex_index_mapper() const
{
	return this->index_mapper;
}

const glm::mat4& continent1::Candidate::get_model_matrix() const
{
	return this->model_matrix;
}

int continent1::Candidate::get_tag() const
{
	return this->tag;
}

void continent1::Candidate::set_tag(int value)
{
	this->tag = value;
}

float continent1::Candidate::get_max_triangle_surface_area() const
{
	return this->max_surface_area;
}

float continent1::Candidate::get_max_triangle_surface_area_by_normal(int normal) const
{
	assert(normal >= 0);
	assert(normal < normal_max);
	return this->max_surface_area_by_normal[normal];
}

float continent1::Candidate::get_surface_area_sum() const
{
	return this->surface_area_sum;
}

float continent1::Candidate::get_surface_area_sum_by_normal(int normal) const
{
	assert(normal >= 0);
	assert(normal < normal_max);
	return this->surface_area_sum_by_normal[normal];
}

const kvlc::Bounds& continent1::Candidate::get_bounds() const
{
	return this->bounds;
}

float continent1::Candidate::get_bounding_box_volume() const
{
	auto size = this->bounds.compute_size();
	return glm::dot(size, size);
}

float continent1::Candidate::get_surface_area_bounding_box_volume_ratio() const
{
	return get_surface_area_sum() / get_bounding_box_volume();
}

const thoreau::TileCoordinate& continent1::Candidate::get_min_tile() const
{
	return this->min_tile;
}

const thoreau::TileCoordinate& continent1::Candidate::get_max_tile() const
{
	return this->max_tile;
}

std::size_t continent1::Candidate::get_num_unique_vertices() const
{
	return this->num_unique_vertices;
}

std::size_t continent1::Candidate::get_num_vertices() const
{
	return this->num_vertices;
}

std::size_t continent1::Candidate::get_num_triangles() const
{
	return this->num_triangles;
}

std::size_t continent1::Candidate::get_num_unique_vertices(const thoreau::TileCoordinate& tile_coordinate) const
{
	return get_tile_properties(tile_coordinate).num_unique_vertices;
}

std::size_t continent1::Candidate::get_num_vertices(const thoreau::TileCoordinate& tile_coordinate) const
{
	return get_tile_properties(tile_coordinate).num_vertices;
}

std::size_t continent1::Candidate::get_num_triangles(const thoreau::TileCoordinate& tile_coordinate) const
{
	return get_tile_properties(tile_coordinate).num_triangles;
}

std::size_t continent1::Candidate::get_num_token_intersections(const thoreau::TileCoordinate& tile_coordinate) const
{
	return get_tile_properties(tile_coordinate).num_token_intersections;
}

std::size_t continent1::Candidate::get_num_triangles_on_side(
	const thoreau::TileCoordinate& tile_coordinate, int side) const
{
	assert(side >= 0 && side < side_max);
	return get_tile_properties(tile_coordinate).num_triangles_on_side[side];
}

const continent1::Candidate::TileProperties&
continent1::Candidate::get_tile_properties(
	const thoreau::TileCoordinate& tile_coordinate) const
{
	assert(tile_coordinate.x >= this->min_tile.x && tile_coordinate.x <= this->max_tile.x);
	assert(tile_coordinate.y >= this->min_tile.y && tile_coordinate.y <= this->max_tile.y);
	assert(tile_coordinate.layer >= this->min_tile.layer && tile_coordinate.layer <= this->max_tile.layer);

	auto iter = this->tile_properties.find(tile_coordinate);
	assert(iter != this->tile_properties.end());

	return iter->second;
}

void continent1::Candidate::for_each_tile_in_bounds(
	const thoreau::TileCoordinate& min, const thoreau::TileCoordinate& max,
	const std::function<void(const thoreau::TileCoordinate&, TileProperties&)>& iter)
{
	for (int y = min.y; y <= max.y; ++y)
	{
		for (int x = min.x; x <= max.x; ++x)
		{
			for (int z = min.layer; z <= max.layer; ++z)
			{
				auto tile_coordinate = thoreau::TileCoordinate(x, y, z);
				iter(tile_coordinate, this->tile_properties[tile_coordinate]);
			}
		}
	}
}
