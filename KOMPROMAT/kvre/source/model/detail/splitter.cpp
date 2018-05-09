// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <algorithm>
#include "kvre/model/detail/splitter.hpp"

kvre::detail::Splitter::Splitter(
	const Model& model,
	const kompvter::VertexIndexMapper& index_mapper,
	const kompvter::DrawParameters& draw_parameters) :
		model(model),
		index_mapper(index_mapper),
		index_begin(kvre::IndexedVertexIterator::begin(index_mapper, draw_parameters)),
		index_end(kvre::IndexedVertexIterator::end(index_mapper, draw_parameters))
{
	// Nopes.
}

void kvre::detail::Splitter::collect(BatchedModelDraws& result)
{
	if (!this->is_complete)
	{
		split();
		this->is_complete = true;
	}

	for (auto& i: this->groups)
	{
		auto& triangles = i.second.triangles;

		kompvter::Buffer buffer;
		buffer.allocate(triangles.size() * sizeof(Index));
		buffer.update(0, (const std::uint8_t*)&triangles[0], buffer.get_data_size());

		result.index_mappers.emplace_back(
			buffer,
			kompvter::VertexIndexMapper::format_unsigned_integer);
	}
}

void kvre::detail::Splitter::split()
{
	assert(!this->is_complete);

	collapse_unique_vertices_by_position();
	collect_groups_by_triangle();
	// TODO: Allow this to be toggled.
	//collect_groups_by_projection();
}

void kvre::detail::Splitter::merge(GroupID parent, GroupID child)
{
	auto parent_group_iter = this->groups.find(parent);
	assert(parent_group_iter != this->groups.end());

	auto child_group_iter = this->groups.find(child);
	assert(child_group_iter != this->groups.end());

	auto& parent_group = parent_group_iter->second;
	auto& child_group = child_group_iter->second;

	for (auto i: child_group.triangle_space_map)
	{
		auto cell = i.first;
		auto triangle = i.second + parent_group.triangles.size();

		parent_group.triangle_space_map.emplace(cell, triangle);
	}

	parent_group.vertices.insert(
		child_group.vertices.begin(),
		child_group.vertices.end());
	parent_group.triangles.insert(
		parent_group.triangles.end(),
		child_group.triangles.begin(),
		child_group.triangles.end());
	parent_group.bounds.expand(child_group.bounds);

	for (auto vertex: child_group.vertices)
	{
		this->vertex_to_group[vertex] = parent;
	}

	this->groups.erase(child_group_iter);
}

void kvre::detail::Splitter::collect_groups_by_triangle()
{
	auto current = this->index_begin;
	while (current != this->index_end)
	{
		VertexSet pending_vertices;
		Index triangle[3];
		std::set<int> pending_groups;

		for (int i = 0; i < 3; ++i)
		{
			assert(current != this->index_end);

			auto index = *current;
			triangle[i] = index;

			assert(this->index_to_vertex.count(index) != 0);
			auto vertex = index_to_vertex[*current];
			pending_vertices.insert(vertex);

			auto vertex_group = this->vertex_to_group.find(vertex);
			if (vertex_group != this->vertex_to_group.end())
			{
				pending_groups.insert(vertex_group->second);
			}

			++current;
		}

		if (pending_groups.empty())
		{
			auto& group = make_group();
			group.append(pending_vertices.begin(), pending_vertices.end(), triangle);
		}
		else
		{
			auto iter = pending_groups.begin();
			auto parent_group_iter = this->groups.find(*iter);
			assert(parent_group_iter != this->groups.end());

			auto parent_group_id = parent_group_iter->first;
			auto& parent_group = parent_group_iter->second;
			parent_group.append(
				pending_vertices.begin(), pending_vertices.end(),
				triangle);

			for (++iter; iter != pending_groups.end(); ++iter)
			{
				merge(parent_group_id, *iter);
			}
		}
	}
}

static float determinant(
	const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
{
	return a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y);
}

static void ensure_ccw_winding(glm::vec2& a, glm::vec2& b, glm::vec2& c)
{
	float d = determinant(a, b, c);
	if (d < 0.0f)
	{
		std::swap(b, c);
	}
}

static bool check_edge(
	const glm::vec2 a, const glm::vec2& b, const glm::vec2& c)
{
	static const float E = 1.0f;
	return determinant(a, b, c) <= E;
}

static bool triangles_overlap(
	glm::vec2 a1, glm::vec2 a2, glm::vec2 a3,
	glm::vec2 b1, glm::vec2 b2, glm::vec2 b3)
{
	ensure_ccw_winding(a1, a2, a3);
	ensure_ccw_winding(b1, b2, b3);

	glm::vec2 a[] = { a1, a2, a3 };
	glm::vec2 b[] = { b1, b2, b3 };

	for (int i = 0; i < 3; ++i)
	{
		int j = (i + 1) % 3;
		if (check_edge(a[i], a[j], b[0]) &&
			check_edge(a[i], a[j], b[1]) &&
			check_edge(a[i], a[j], b[2]))
		{
			return false;
		}
	}

	for (int i = 0; i < 3; ++i)
	{
		int j = (i + 1) % 3;

		if (check_edge(b[i], b[j], a[0]) &&
			check_edge(b[i], b[j], a[1]) &&
			check_edge(b[i], b[j], a[2]))
		{
			return false;
		}
	}

	return true;
}

void kvre::detail::Splitter::collect_groups_by_projection()
{
	auto xz_plane = [](const glm::vec3& input)
	{
		return glm::vec2(input.x, input.z);
	};
	auto xy_plane = [](const glm::vec3& input)
	{
		return glm::vec2(input.x, input.y);
	};
	auto zy_plane = [](const glm::vec3& input)
	{
		return glm::vec2(input.z, input.y);
	};

	bool merged;
	do
	{
		merged = false;

		for (auto i = this->groups.begin(); i != this->groups.end(); ++i)
		{
			auto j = i;
			for (++j; j != this->groups.end(); ++j)
			{
				if (!kvlc::Bounds::inclusive_intersection(i->second.bounds, j->second.bounds))
				{
					continue;
				}

				if (overlap(i->first, j->first, xz_plane) &&
					overlap(i->first, j->first, xy_plane) &&
					overlap(i->first, j->first, zy_plane))
				{
					merge(i->first, j->first);
					merged = true;
					break;
				}
			}

			if (merged)
			{
				break;
			}
		}
	} while (merged);
}

bool kvre::detail::Splitter::overlap(
	GroupID a, GroupID b,
	const std::function<glm::vec2(const glm::vec3&)>& project)
{
	auto a_iter = this->groups.find(a);
	assert(a_iter != this->groups.end());

	auto b_iter = this->groups.find(b);
	assert(b_iter != this->groups.end());

	auto& a_group = a_iter->second;
	auto& b_group = b_iter->second;

	for (auto a_cell: a_group.triangle_space_map)
	{
		auto b_range = b_group.triangle_space_map.equal_range(a_cell.first);
		if (b_range.first == b_range.second)
		{
			continue;
		}

		auto i = a_cell.second;
		glm::vec2 i_triangle[3];
		for (int k = 0; k < 3; ++k)
		{
			auto index = a_group.triangles[i + k];
			auto position = this->model.get_position(index);
			i_triangle[k] = project(position);
		}

		for (/* Nothing/ */; b_range.first != b_range.second; ++b_range.first)
		{
			auto j = b_range.first->second;

			glm::vec2 j_triangle[3];
			for (int k = 0; k < 3; ++k)
			{
				auto index = b_group.triangles[j + k];
				auto position = this->model.get_position(index);
				j_triangle[k] = project(position);
			}

			if (triangles_overlap(
				i_triangle[0], i_triangle[1], i_triangle[2], 
				j_triangle[0], j_triangle[1], j_triangle[2]))
			{
				return true;
			}
		}
	}

	return false;
}

kvre::detail::Splitter::Group& kvre::detail::Splitter::make_group()
{
	auto id = ++this->current_group_id;
	auto& result = this->groups[id];
	result.id = id;
	result.bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
	result.splitter = this;

	return result;
}

void kvre::detail::Splitter::collapse_unique_vertices_by_position()
{
	assert(this->index_to_vertex.empty());

	typedef std::tuple<int, int, int> Position;
	std::map<Position, std::size_t> position_to_vertex;

	std::size_t unique_vertex_count = 0;
	for (auto i = this->index_begin; i != this->index_end; ++i)
	{
		auto index = *i;
		auto p = this->model.get_position(index);
		auto t = std::make_tuple<int, int, int>((int)p.x, (int)p.y, (int)p.z);
		auto iter = position_to_vertex.find(t);
		if (iter == position_to_vertex.end())
		{
			iter = position_to_vertex.emplace(t, unique_vertex_count++).first;
		}

		this->index_to_vertex[index] = iter->second;
		this->vertex_positions[iter->second] = p;
	}
}
