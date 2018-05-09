// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "continent1/heuristics.hpp"

bool continent1::heuristics::is_tile_impassable(
	const thoreau::TileCoordinate& tile_coordinate,
	const CandidateManager& candidate_manager)
{
	auto begin = candidate_manager.space_begin(tile_coordinate);
	auto end = candidate_manager.space_end(tile_coordinate);
	if (begin == end)
	{
		return false;
	}

	int count = 0;
	int thins = 0;
	int not_thins = 0;
	for (auto i = begin; i != end; ++i)
	{
		auto& candidate = *candidate_manager.get(i->second);

		bool discard = false;
		if (candidate.get_num_token_intersections(tile_coordinate) == 0)
		{
			discard = true;
		}

		if (candidate.get_max_triangle_surface_area_by_normal(Candidate::normal_y) < 64 &&
			candidate.get_max_triangle_surface_area_by_normal(Candidate::normal_x) < 12 * 1024 &&
			candidate.get_max_triangle_surface_area_by_normal(Candidate::normal_y) < 12 * 1024)
		{
			++thins;
		}
		else
		{
			++not_thins;
		}

		auto size = candidate.get_bounds().compute_size();
		if (size.x < 512 && size.z < 512)
		{
			float diagonal = std::sqrt(size.x * size.x + size.z * size.z);
			float ratio = size.y / diagonal;

			if (ratio > 3.0f)
			{
				//discard = true;
			}
		}

		auto l = candidate.get_num_triangles_on_side(
			tile_coordinate, Candidate::side_left);
		auto r = candidate.get_num_triangles_on_side(
			tile_coordinate, Candidate::side_right);
		auto t = candidate.get_num_triangles_on_side(
			tile_coordinate, Candidate::side_bottom);
		auto b = candidate.get_num_triangles_on_side(
			tile_coordinate, Candidate::side_top);
		auto n = candidate.get_num_triangles(tile_coordinate);

		if ((l >= n && r == 0) ||
			(r >= n && l == 0) ||
			(t >= n && b == 0) ||
			(b >= n && t == 0))
		{
			discard = true;
		}

		if (!discard)
		{
			++count;
		}
	}

	if (count == 0)
	{
		return false;
	}

	if (thins > not_thins)
	{
		return false;
	}

	return true;
}

bool continent1::heuristics::is_tile_sparse(
	const thoreau::TileCoordinate& tile_coordinate,
	const CandidateManager& candidate_manager)
{
	auto begin = candidate_manager.space_begin(tile_coordinate);
	auto end = candidate_manager.space_end(tile_coordinate);
	if (begin == end)
	{
		return true;
	}

	for (auto i = begin; i != end; ++i)
	{
		auto& candidate = *candidate_manager.get(i->second);

		auto l = candidate.get_num_triangles_on_side(
			tile_coordinate, Candidate::side_left);
		auto r = candidate.get_num_triangles_on_side(
			tile_coordinate, Candidate::side_right);
		auto t = candidate.get_num_triangles_on_side(
			tile_coordinate, Candidate::side_top);
		auto b = candidate.get_num_triangles_on_side(
			tile_coordinate, Candidate::side_bottom);
		auto n = candidate.get_num_triangles(tile_coordinate);
		auto v = candidate.get_num_vertices(tile_coordinate);

		if ((l >= n && r == 0) ||
			(r >= n && l == 0) ||
			(t >= n && b == 0) ||
			(b >= n && t == 0))
		{
			continue;
		}
	
		if (candidate.get_num_token_intersections(tile_coordinate) == 0)
		{
			continue;
		}

		if (v < 3)
		{
			continue;
		}

		if (candidate.get_surface_area_sum() > 96*1024)
		{
			return false;
		}
	}

	return true;
}
