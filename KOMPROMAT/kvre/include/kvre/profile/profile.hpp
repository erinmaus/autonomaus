// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVRE_PROFILE_PROFILE_HPP
#define KVRE_PROFILE_PROFILE_HPP

#include <cstddef>
#include <functional>
#include <glm/glm.hpp>
#include "kompvter/graphics/vertexAttributes.hpp"
#include "kompvter/graphics/vertexFetcher.hpp"

namespace kvre
{
	class Model;
	class AtlasMeta;

	class Profile
	{
	public:
		virtual ~Profile() = default;

		static const Profile* get_default();

		struct AtlasMeta
		{
			virtual ~AtlasMeta() = default;
		};
		virtual const AtlasMeta* get_default_atlas_meta() const = 0;
		virtual glm::vec2 compute_texture_coordinate_to_atlas(
			const glm::vec2& texture_coordinate,
			const glm::vec4& texture_meta,
			const AtlasMeta* atlas_meta) const = 0;

		virtual std::size_t get_num_model_vertices(
			const kompvter::VertexAttributes& attributes) const = 0;

		virtual glm::vec3 get_model_vertex_position(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const = 0;
		virtual int get_model_vertex_bone(
			const kompvter::VertexFetcher& fetcher,
			std::size_t) const = 0;
		virtual glm::vec4 get_model_vertex_color(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const = 0;
		virtual glm::vec2 get_model_vertex_texture_coordinate(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const = 0;
		virtual glm::vec4 get_model_vertex_texture_meta(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const = 0;
		virtual glm::vec3 get_model_vertex_normal(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const = 0;

		virtual const kompvter::VertexFetcher* get_model_vertex_positions_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_model_vertex_bones_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_model_vertex_colors_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_model_vertex_texture_coordinates_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_model_vertex_texture_metas_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_model_vertex_normals_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;

		virtual std::size_t get_num_terrain_vertices(
			const kompvter::VertexAttributes& attributes) const = 0;

		virtual glm::vec3 get_terrain_vertex_position(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const = 0;
		virtual glm::vec4 get_terrain_vertex_color(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index) const = 0;

		virtual const kompvter::VertexFetcher* get_terrain_vertex_positions_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_terrain_vertex_colors_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;

		virtual std::size_t get_num_particle_vertices(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual bool get_is_billboard(
			const kompvter::VertexAttributes& attributes) const = 0;

		virtual glm::vec3 get_particle_vertex_position(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			bool is_billboard) const = 0;
		virtual glm::vec4 get_particle_vertex_color(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			bool is_billboard) const = 0;
		virtual glm::vec2 get_particle_vertex_texture_coordinate(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			bool is_billboard) const = 0;
		virtual glm::vec4 get_particle_vertex_texture_meta(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			bool is_billboard) const = 0;

		virtual const kompvter::VertexFetcher* get_particle_vertex_positions_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_particle_vertex_colors_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_particle_vertex_texture_coordinates_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_particle_vertex_texture_metas_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;

		virtual std::size_t get_num_flat_vertices(
			const kompvter::VertexAttributes& attributes) const = 0;

		virtual void get_flat_vertex_positions(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			std::size_t count,
			glm::vec2* output_buffer,
			std::size_t output_stride) const = 0;
		virtual void get_flat_vertex_colors(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			std::size_t count,
			glm::vec4* output_buffer,
			std::size_t output_stride) const = 0;
		virtual void get_flat_vertex_texture_coordinates(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			std::size_t count,
			glm::vec2* output_buffer,
			std::size_t output_stride) const = 0;
		virtual void get_flat_vertex_atlas_mins(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			std::size_t count,
			glm::vec2* output_buffer,
			std::size_t output_stride) const = 0;
		virtual void get_flat_vertex_atlas_maxes(
			const kompvter::VertexFetcher& fetcher,
			std::size_t index,
			std::size_t count,
			glm::vec2* output_buffer,
			std::size_t output_stride) const = 0;

		virtual const kompvter::VertexFetcher* get_flat_vertex_positions_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_flat_vertex_colors_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_flat_vertex_texture_coordinates_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_flat_vertex_atlas_mins_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;
		virtual const kompvter::VertexFetcher* get_flat_vertex_atlas_maxes_fetcher(
			const kompvter::VertexAttributes& attributes) const = 0;

		virtual bool is_flat_vertex_textured(const glm::vec2& texture_coordinate) const = 0;

	private:
		static Profile* current_profile;
	};
}

#endif
