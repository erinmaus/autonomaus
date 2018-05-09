// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVRE_MODEL_DETAIL_SPLITTER_HPP
#define KVRE_MODEL_DETAIL_SPLITTER_HPP

#include <functional>
#include <map>
#include <set>
#include <vector>
#include <glm/glm.hpp>
#include "kompvter/graphics/drawParameters.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "kvre/model/batchedModelDraws.hpp"
#include "kvre/model/model.hpp"

#include <boost/container/flat_map.hpp>

namespace kvre { namespace detail
{
	class Splitter
	{
	public:
		Splitter(
			const Model& model,
			const kompvter::VertexIndexMapper& index_mapper,
			const kompvter::DrawParameters& draw_parameters);
		~Splitter() = default;

		void collect(BatchedModelDraws& result);

	private:
		void split();

		const Model& model;
		const kompvter::VertexIndexMapper& index_mapper;
		const IndexedVertexIterator index_begin;
		const IndexedVertexIterator index_end;

		bool is_complete = false;

		typedef std::uint32_t Index;
		typedef std::size_t Vertex;
		typedef int GroupID;
		typedef std::set<std::size_t> VertexSet;
		typedef std::vector<Index> TriangleIndexList;

		std::map<Vertex, glm::vec3> vertex_positions;

		struct Group
		{
			GroupID id = 0;
			Splitter* splitter = nullptr;
			TriangleIndexList triangles;
			VertexSet vertices;
			kvlc::Bounds bounds;

			typedef std::pair<int, int> Cell;
			static const int CELL_SIZE = 512;
			std::multimap<Cell, std::size_t /* triangle first index */> triangle_space_map;

			template <typename VertexIter>
			void append(
				VertexIter begin, VertexIter end,
				Index triangle[]);
		};
		std::map<GroupID, Group> groups;
		void merge(GroupID parent, GroupID child);
		void collect_groups_by_triangle();
		void collect_groups_by_projection();
		bool overlap(
			GroupID a, GroupID b,
			const std::function<glm::vec2(const glm::vec3&)>& project);

		int current_group_id = 0;
		Group& make_group();

		std::map<Vertex, GroupID> vertex_to_group;
		std::map<Index, Vertex> index_to_vertex;
		void collapse_unique_vertices_by_position();
	};
} }

template <typename VertexIter>
void kvre::detail::Splitter::Group::append(
	VertexIter begin, VertexIter end, Index triangle[])
{
	auto triangle_bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
	while (begin != end)
	{
		auto vertex = *begin;
		this->vertices.insert(vertex);

		auto position = this->splitter->vertex_positions.find(vertex);
		assert(position != this->splitter->vertex_positions.end());
		this->bounds.expand(position->second);
		triangle_bounds.expand(position->second);

		this->splitter->vertex_to_group[vertex] = this->id;

		++begin;
	}

	float left = triangle_bounds.min.x;// - CELL_SIZE;
	float right = triangle_bounds.max.x;// + CELL_SIZE;
	float top = triangle_bounds.min.z;// - CELL_SIZE;
	float bottom = triangle_bounds.max.z;// + CELL_SIZE;
	for (auto i = left; i <= right; i += CELL_SIZE)
	{
		for (auto j = top; j <= bottom; j += CELL_SIZE)
		{
			auto cell = std::make_pair<int, int>(i / CELL_SIZE, j / CELL_SIZE);
			this->triangle_space_map.emplace(cell, this->triangles.size());
		}
	}

	this->triangles.insert(
		this->triangles.end(),
		triangle, triangle + 3);
}

#endif
