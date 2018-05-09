// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CONTINENT1_CANDIDATE_HPP
#define CONTINENT1_CANDIDATE_HPP

#include <functional>
#include <map>
#include <glm/glm.hpp>
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvre/model/model.hpp"
#include "kvre/model/batchedModelDraws.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "thoreau/map.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace continent1
{
	class Candidate
	{
	public:
		enum
		{
			normal_x = 0,
			normal_y,
			normal_z,
			normal_max
		};

		Candidate() = default;
		Candidate(
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper,
			const glm::mat4& model_matrix = glm::mat4(1.0f));
		~Candidate() = default;

		void populate(
			const thoreau::Map& map,
			const glm::vec3& token_size,
			float token_elevation);

		const kvre::Model* get_model() const;
		const kompvter::VertexIndexMapper& get_vertex_index_mapper() const;
		const glm::mat4& get_model_matrix() const;

		int get_tag() const;
		void set_tag(int value);

		float get_max_triangle_surface_area() const;
		float get_max_triangle_surface_area_by_normal(int normal) const;

		float get_surface_area_sum() const;
		float get_surface_area_sum_by_normal(int normal) const;

		const kvlc::Bounds& get_bounds() const;
		float get_bounding_box_volume() const;
		float get_surface_area_bounding_box_volume_ratio() const;
		const thoreau::TileCoordinate& get_min_tile() const;
		const thoreau::TileCoordinate& get_max_tile() const;

		std::size_t get_num_unique_vertices() const;
		std::size_t get_num_vertices() const;
		std::size_t get_num_triangles() const;

		std::size_t get_num_unique_vertices(const thoreau::TileCoordinate& tile_coordinate) const;
		std::size_t get_num_vertices(const thoreau::TileCoordinate& tile_coordinate) const;
		std::size_t get_num_triangles(const thoreau::TileCoordinate& tile_coordinate) const;
		std::size_t get_num_token_intersections(const thoreau::TileCoordinate& tile_coordinate) const;

		enum
		{
			side_left = 0,
			side_right,
			side_top,
			side_bottom,
			side_max
		};
		std::size_t get_num_triangles_on_side(const thoreau::TileCoordinate& tile_coordinate, int side) const;

		template <typename OutputIter>
		static void collect(
			OutputIter output,
			const kvre::Model* input_model,
			const kompvter::VertexIndexMapper& index_mapper,
			const glm::mat4& model_matrix,
			const glm::vec3& token_size, float token_elevation);

	private:
		static const glm::vec3 PLANES[side_max];

		void compute_geometry_data();
		void compute_mesh_data(const thoreau::Map& map);
		void compute_tile_geometry_data(
			const thoreau::Map& map,
			const glm::vec3& token_size, float token_elevation);

		const kvre::Model* model = nullptr;
		kompvter::VertexIndexMapper index_mapper;
		glm::mat4 model_matrix;
		int tag = 0;

		float max_surface_area = 0.0f;
		float max_surface_area_by_normal[normal_max] = {};
		float surface_area_sum = 0.0f;
		float surface_area_sum_by_normal[normal_max] = {};

		kvlc::Bounds bounds = kvlc::Bounds(glm::vec3(0.0f), glm::vec3(0.0f));
		thoreau::TileCoordinate min_tile = thoreau::TileCoordinate(0, 0);
		thoreau::TileCoordinate max_tile = thoreau::TileCoordinate(0, 0);

		std::size_t num_unique_vertices = 0;
		std::size_t num_vertices = 0;
		std::size_t num_triangles = 0;

		struct TileProperties
		{
			std::size_t num_unique_vertices = 0;
			std::size_t num_vertices = 0;
			std::size_t num_triangles = 0;
			std::size_t num_token_intersections = 0;
			std::size_t num_triangles_on_side[side_max] = {};
		};
		std::map<thoreau::TileCoordinate, TileProperties> tile_properties;
		const TileProperties& get_tile_properties(const thoreau::TileCoordinate& tile_coordinate) const;
		void for_each_tile_in_bounds(
			const thoreau::TileCoordinate& min,
			const thoreau::TileCoordinate& max,
			const std::function<void(const thoreau::TileCoordinate&, TileProperties&)>& iter);
	};
}

template <typename OutputIter>
void continent1::Candidate::collect(
	OutputIter iter,
	const kvre::Model* model,
	const kompvter::VertexIndexMapper& index_mapper,
	const glm::mat4& model_matrix,
	const glm::vec3& token_size,
	float token_elevation)
{
	kvre::BatchedModelDraws draws(*model, index_mapper);
	for (auto& draw: draws)
	{
		*iter = Candidate(model, draw, model_matrix);
		iter->populate_per_tile_properties(token_size, token_elevation);

		++iter;
	}
}

#endif
