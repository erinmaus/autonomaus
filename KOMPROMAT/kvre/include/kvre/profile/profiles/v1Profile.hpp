// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVRE_PROFILE_PROFILES_V1_PROFILE_HPP
#define KVRE_PROFILE_PROFILES_V1_PROFILE_HPP

#include "kvre/profile/profile.hpp"

namespace kvre
{
	class V1Profile : public Profile
	{
	public:
		struct V1AtlasMeta : public AtlasMeta
		{
			glm::vec4 uniform = glm::vec4(256.0f, 16.0f, 1.0f / 4096.0f, 4.0f);
		};

		glm::vec2 compute_texture_coordinate_to_atlas(
			const glm::vec2& texture_coordinate,
			const glm::vec4& texture_meta,
			const AtlasMeta* atlas_meta) const override;

		const AtlasMeta* get_default_atlas_meta() const override;

		std::size_t get_num_model_vertices(
			const kompvter::VertexAttributes& attributes) const override;

		glm::vec3 get_model_vertex_position(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const override;
		int get_model_vertex_bone(
			const kompvter::VertexFetcher& fetcher,
			std::size_t) const override;
		glm::vec4 get_model_vertex_color(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const override;
		glm::vec2 get_model_vertex_texture_coordinate(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const override;
		glm::vec4 get_model_vertex_texture_meta(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const override;
		glm::vec3 get_model_vertex_normal(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const override;

		const kompvter::VertexFetcher* get_model_vertex_positions_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_model_vertex_bones_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_model_vertex_colors_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_model_vertex_texture_coordinates_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_model_vertex_texture_metas_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_model_vertex_normals_fetcher(
			const kompvter::VertexAttributes& attributes) const override;

		std::size_t get_num_terrain_vertices(
			const kompvter::VertexAttributes& attributes) const override;

		glm::vec3 get_terrain_vertex_position(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const override;
		glm::vec4 get_terrain_vertex_color(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const override;

		const kompvter::VertexFetcher* get_terrain_vertex_positions_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_terrain_vertex_colors_fetcher(
			const kompvter::VertexAttributes& attributes) const override;

		std::size_t get_num_particle_vertices(
			const kompvter::VertexAttributes& attributes) const override;
		bool get_is_billboard(
			const kompvter::VertexAttributes& attributes) const override;

		glm::vec3 get_particle_vertex_position(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			bool is_billboard) const override;
		glm::vec4 get_particle_vertex_color(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			bool is_billboard) const override;
		glm::vec2 get_particle_vertex_texture_coordinate(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			bool is_billboard) const override;
		glm::vec4 get_particle_vertex_texture_meta(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			bool is_billboard) const override;

		const kompvter::VertexFetcher* get_particle_vertex_positions_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_particle_vertex_colors_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_particle_vertex_texture_coordinates_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_particle_vertex_texture_metas_fetcher(
			const kompvter::VertexAttributes& attributes) const override;

		std::size_t get_num_flat_vertices(
			const kompvter::VertexAttributes& attributes) const override;

		void get_flat_vertex_positions(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			std::size_t count,
			glm::vec2* output_buffer,
			std::size_t output_stride) const override;
		void get_flat_vertex_colors(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			std::size_t count,
			glm::vec4* output_buffer,
			std::size_t output_stride) const override;
		void get_flat_vertex_texture_coordinates(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			std::size_t count,
			glm::vec2* output_buffer,
			std::size_t output_stride) const override;
		void get_flat_vertex_atlas_mins(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			std::size_t count,
			glm::vec2* output_buffer,
			std::size_t output_stride) const override;
		void get_flat_vertex_atlas_maxes(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			std::size_t count,
			glm::vec2* output_buffer,
			std::size_t output_stride) const override;

		const kompvter::VertexFetcher* get_flat_vertex_positions_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_flat_vertex_colors_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_flat_vertex_texture_coordinates_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_flat_vertex_atlas_mins_fetcher(
			const kompvter::VertexAttributes& attributes) const override;
		const kompvter::VertexFetcher* get_flat_vertex_atlas_maxes_fetcher(
			const kompvter::VertexAttributes& attributes) const override;

		bool is_flat_vertex_textured(const glm::vec2& texture_coordinate) const override;

	private:
		V1AtlasMeta default_atlas_meta;
	};
}

#endif
