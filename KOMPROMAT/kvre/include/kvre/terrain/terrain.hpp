// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVRE_TERRAIN_TERRAIN_HPP
#define KVRE_TERRAIN_TERRAIN_HPP

#include <vector>
#include <glm/glm.hpp>
#include "kompvter/graphics/vertexAttributes.hpp"
#include "kvre/profile/profile.hpp"

namespace kvre
{
	class Terrain
	{
	public:
		explicit Terrain(
			const kompvter::VertexAttributes& attributes,
			const Profile* profile = nullptr);
		Terrain(const Terrain& other);
		~Terrain() = default;

		const kompvter::VertexAttributes& get_vertex_attributes() const;
		const Profile* get_profile() const;

		std::size_t get_num_vertices() const;

		glm::vec3 get_position(std::size_t index) const;
		glm::vec4 get_color(std::size_t index) const;

		template <typename IndexIter>
		void for_each_position(
			IndexIter iter_begin, IndexIter iter_end,
			const std::function<void(const glm::vec3& position, std::size_t index)>& func) const;

		template <typename IndexIter>
		void for_each_color(
			IndexIter iter_begin, IndexIter iter_end,
			const std::function<void(const glm::vec4& position, std::size_t index)>& func) const;

		Terrain& operator =(const Terrain& other);

	private:
		template <typename IndexIter, typename ElementType>
		void for_each_vertex_element(
			IndexIter iter_begin, IndexIter iter_end,
			const std::function<ElementType(std::size_t)>& getter,
			const std::function<void(const ElementType& element, std::size_t index)>& func) const;

		kompvter::VertexAttributes vertex_attributes;
		const Profile* profile;

		const kompvter::VertexFetcher* positions_fetcher;
		mutable std::vector<glm::vec3> positions;

		const kompvter::VertexFetcher* colors_fetcher;
		mutable std::vector<glm::vec4> colors;

		mutable std::size_t num_vertices = 0;
	};
}

template <typename IndexIter>
void kvre::Terrain::for_each_position(
	IndexIter iter_begin, IndexIter iter_end,
	const std::function<void(const glm::vec3&, std::size_t)>& func) const
{
	using namespace std::placeholders;

	for_each_vertex_element<IndexIter, glm::vec3>(
		iter_begin, iter_end,
		std::bind(&Terrain::get_position, this, _1),
		func);
}

template <typename IndexIter>
void kvre::Terrain::for_each_color(
	IndexIter iter_begin, IndexIter iter_end,
	const std::function<void(const glm::vec4&, std::size_t)>& func) const
{
	using namespace std::placeholders;

	for_each_vertex_element<IndexIter, glm::vec3>(
		iter_begin, iter_end,
		std::bind(&Terrain::get_color, this, _1),
		func);
}

template <typename IndexIter, typename ElementType>
void kvre::Terrain::for_each_vertex_element(
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
