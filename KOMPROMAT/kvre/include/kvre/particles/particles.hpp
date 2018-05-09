// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVRE_PARTICLES_PARTICLES_HPP
#define KVRE_PARTICLES_PARTICLES_HPP

#include <functional>
#include <type_traits>
#include <vector>
#include <glm/glm.hpp>
#include "kompvter/graphics/vertexAttributes.hpp"
#include "kvre/profile/profile.hpp"

namespace kvre
{
	class Particles
	{
	public:
		Particles(
			const kompvter::VertexAttributes& attributes,
			const Profile* profile = nullptr);
		Particles(const Particles& other);
		~Particles() = default;

		const kompvter::VertexAttributes& get_vertex_attributes() const;
		const Profile* get_profile() const;

		std::size_t get_num_vertices() const;
		bool get_is_billboard() const;

		glm::vec3 get_position(std::size_t index) const;
		glm::vec4 get_color(std::size_t index) const;
		glm::vec2 get_texture_coordinate(std::size_t index) const;
		glm::vec4 get_texture_meta(std::size_t index) const;
		glm::vec2 compute_atlas_texture_coordinates(
			std::size_t index);

		bool has_positions() const;
		bool has_colors() const;
		bool has_texture_coordinates() const;
		bool has_texture_metas() const;

		template <typename IndexIter>
		void for_each_position(
			IndexIter iter_begin, IndexIter iter_end,
			const std::function<void(const glm::vec3& position, std::size_t index)>& func) const;

		template <typename IndexIter>
		void for_each_bone(
			IndexIter iter_begin, IndexIter iter_end,
			const std::function<void(int element, std::size_t index)>& func) const;

		template <typename IndexIter>
		void for_each_color(
			IndexIter iter_begin, IndexIter iter_end,
			const std::function<void(const glm::vec4& element, std::size_t index)>& func) const;

		template <typename IndexIter>
		void for_each_texture_coordinate(
			IndexIter iter_begin, IndexIter iter_end,
			const std::function<void(const glm::vec2& element, std::size_t index)>& func) const;

		template <typename IndexIter>
		void for_each_texture_meta(
			IndexIter iter_begin, IndexIter iter_end,
			const std::function<void(const glm::vec2& element, std::size_t index)>& func) const;

		template <typename IndexIter>
		void for_each_normal(
			IndexIter iter_begin, IndexIter iter_end,
			const std::function<void(const glm::vec3& element, std::size_t index)>& func) const;

		Particles& operator =(const Particles& other);

	private:
		template <typename IndexIter, typename ElementType>
		void for_each_vertex_element(
			IndexIter iter_begin, IndexIter iter_end,
			const std::function<ElementType(std::size_t index)>& getter,
			const std::function<void(const ElementType& element, std::size_t index)>& func) const;

		kompvter::VertexAttributes vertex_attributes;
		const Profile* profile;

		const kompvter::VertexFetcher* positions_fetcher;
		const kompvter::VertexFetcher* colors_fetcher;
		const kompvter::VertexFetcher* texture_coordinates_fetcher;
		const kompvter::VertexFetcher* texture_metas_fetcher;

		mutable std::vector<glm::vec3> positions;
		mutable std::vector<glm::vec4> colors;
		mutable std::vector<glm::vec2> texture_coordinates;
		mutable std::vector<glm::vec4> texture_metas;

		mutable std::size_t num_vertices = 0;

		bool is_billboard = false;
	};
}

template <typename IndexIter>
void kvre::Particles::for_each_position(
	IndexIter iter_begin, IndexIter iter_end,
	const std::function<void(const glm::vec3&, std::size_t)>& func) const
{
	using namespace std::placeholders;

	for_each_vertex_element<IndexIter, glm::vec3>(
		iter_begin, iter_end,
		std::bind(&Particles::get_position, this, _1),
		func);
}

template <typename IndexIter>
void kvre::Particles::for_each_color(
	IndexIter iter_begin, IndexIter iter_end,
	const std::function<void(const glm::vec4&, std::size_t)>& func) const
{
	using namespace std::placeholders;

	for_each_vertex_element<IndexIter, glm::vec4>(
		iter_begin, iter_end,
		std::bind(&Particles::get_color, this, _1),
		func);
}

template <typename IndexIter>
void kvre::Particles::for_each_texture_coordinate(
	IndexIter iter_begin, IndexIter iter_end,
	const std::function<void(const glm::vec2&, std::size_t)>& func) const
{
	using namespace std::placeholders;

	for_each_vertex_element<IndexIter, glm::vec2>(
		iter_begin, iter_end,
		std::bind(&Particles::get_texture_coordinate, this, _1),
		func);
}

template <typename IndexIter>
void kvre::Particles::for_each_texture_meta(
	IndexIter iter_begin, IndexIter iter_end,
	const std::function<void(const glm::vec2&, std::size_t)>& func) const
{
	using namespace std::placeholders;

	for_each_vertex_element<IndexIter, glm::vec2>(
		iter_begin, iter_end,
		std::bind(&Particles::get_texture_meta, this, _1),
		func);
}

template <typename IndexIter, typename ElementType>
void kvre::Particles::for_each_vertex_element(
	IndexIter iter_begin, IndexIter iter_end,
	const std::function<ElementType(std::size_t)>& getter,
	const std::function<void(const ElementType&, std::size_t)>& func) const
{
	for (auto& i = iter_begin; i != iter_end; ++i)
	{
		auto index = *i;
		func(getter(index), index);
	}
}

#endif
