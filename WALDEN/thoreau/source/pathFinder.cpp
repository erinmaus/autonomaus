// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cmath>
#include <algorithm>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include "thoreau/pathFinder.hpp"

thoreau::PathFinder::PathFinder(const thoreau::Map& map)
{
	this->map = &map;
}

thoreau::PathFinder::PathFinder(const thoreau::Map& map, const thoreau::PathFinder& other)
{
	*this = other;
	this->map = &map;
}

float thoreau::PathFinder::get_neighbor_weighting_radius() const
{
	return this->neighbor_weighting_radius;
}

void thoreau::PathFinder::set_neighbor_weighting_radius(float value)
{
	this->neighbor_weighting_radius = value;
}

float thoreau::PathFinder::get_wall_weight() const
{
	return this->wall_weight;
}

void thoreau::PathFinder::set_wall_weight(float value)
{
	this->wall_weight = value;
}
float thoreau::PathFinder::get_impassable_weight() const
{
	return this->impassable_weight;
}

void thoreau::PathFinder::set_impassable_weight(float value)
{
	this->impassable_weight = value;
}

float thoreau::PathFinder::get_portal_weight() const
{
	return this->portal_weight;
}

void thoreau::PathFinder::set_portal_weight(float value)
{
	this->portal_weight = value;
}

float thoreau::PathFinder::get_door_weight() const
{
	return this->door_weight;
}

void thoreau::PathFinder::set_door_weight(float value)
{
	this->door_weight = value;
}

void thoreau::PathFinder::add_sector_restriction(
	const thoreau::SectorCoordinate& sector_coordinate)
{
	this->sectors.insert(sector_coordinate);
}

void thoreau::PathFinder::clear_sector_restrictions()
{
	this->sectors.clear();
}

bool thoreau::PathFinder::has_sector_restrictions() const
{
	return !this->sectors.empty();
}

bool thoreau::PathFinder::try_find_path(
	const TileCoordinate& start_coordinate,
	const TileCoordinate& stop_coordinate)
{
	this->result.clear();

	if (!this->map->has_tile(start_coordinate) || !this->map->has_tile(stop_coordinate))
	{
		return false;
	}

	auto& start = this->map->get_tile(start_coordinate);
	auto& stop = this->map->get_tile(stop_coordinate);

	std::unordered_map<TileCoordinate, float> g_scores;
	g_scores.emplace(start_coordinate, 0.0f);

	std::unordered_map<TileCoordinate, float> f_scores;
	f_scores.emplace(start_coordinate, tile_heuristic(start_coordinate, stop_coordinate));

	auto compare_f_score = [&f_scores](TileCoordinate a, TileCoordinate b)
	{
		return f_scores[a] < f_scores[b];
	};
	typedef std::multiset<TileCoordinate, decltype(compare_f_score)> Queue;
	Queue queue(compare_f_score);
	queue.insert(start_coordinate);

	std::unordered_set<TileCoordinate> closed;
	std::unordered_set<TileCoordinate> open;
	std::unordered_map<TileCoordinate, TileCoordinate> path;
	open.insert(start_coordinate);

	while (!open.empty())
	{
		auto queue_top = queue.begin();
		auto top = *queue_top;
		queue.erase(queue_top);

		if (top == stop_coordinate)
		{
			this->result.push_back(top);

			auto current = top;
			while (path.find(current) != path.end())
			{
				auto iter = path.find(current);
				this->result.insert(this->result.begin(), iter->second);

				current = iter->second;
			}

			return true;
		}

		assert(open.find(top) != open.end());
		open.erase(top);
		closed.insert(top);

		std::vector<TileCoordinate> neighbors;
		get_neighbors(top, neighbors);

		for(auto& neighbor: neighbors)
		{
			if (closed.find(neighbor) != closed.end())
			{
				continue;
			}

			float g = g_scores[top] + tile_distance(top, neighbor);
			if (open.find(neighbor) == open.end())
			{
				open.insert(neighbor);
			}
			else if (g_scores.find(neighbor) != g_scores.end() && g > g_scores[neighbor])
			{
				continue;
			}

			path[neighbor] = top;
			g_scores[neighbor] = g;

			auto f = g + tile_heuristic(neighbor, stop_coordinate);

			auto range = queue.equal_range(neighbor);
			for (auto& i = range.first; i != range.second; ++i)
			{
				if (*i == neighbor)
				{
					queue.erase(i);
					break;
				}
			}

			f_scores[neighbor] = f;
			queue.insert(neighbor);
		}
	}

	return false;
}

std::size_t thoreau::PathFinder::length() const
{
	return this->result.size();
}

bool thoreau::PathFinder::empty() const
{
	return this->result.empty();
}

thoreau::TileCoordinate thoreau::PathFinder::at(std::size_t index) const
{
	if (index > length())
	{
		return thoreau::TileCoordinate();
	}

	return this->result[index];
}

void thoreau::PathFinder::clear()
{
	this->result.clear();
}

thoreau::PathFinder::const_iterator thoreau::PathFinder::begin() const
{
	return this->result.begin();
}

thoreau::PathFinder::const_iterator thoreau::PathFinder::end() const
{
	return this->result.end();
}

float thoreau::PathFinder::tile_distance(
	const TileCoordinate& a,
	const TileCoordinate& b) const
{
	static const float D = 1.0f;
	static const float D2 = std::sqrt(2.0f);

	float dx = std::abs(a.x - b.x);
	float dy = std::abs(a.y - b.y);

	return D * (dx + dy) + (D2 - 2.0f * D) * std::min(dx, dy);
}

float thoreau::PathFinder::tile_heuristic(
	const TileCoordinate& tile_coordinate,
	const TileCoordinate& stop_coordinate) const
{
	auto& tile = this->map->get_tile(tile_coordinate);

	float cost;
	if (tile.is_door())
	{
		cost = this->door_weight;
	}
	else if (tile.is_portal())
	{
		cost = this->portal_weight;
	}
	else
	{
		cost = 0.0f;
	}

	for (int i = -neighbor_weighting_radius; i < neighbor_weighting_radius; ++i)
	{
		for (int j = -neighbor_weighting_radius; j < neighbor_weighting_radius; ++j)
		{
			float distance = std::sqrt(i * i + j * j);
			if (distance > this->neighbor_weighting_radius)
			{
				continue;
			}

			thoreau::TileCoordinate neighbor_coordinate = tile_coordinate;
			neighbor_coordinate.x += i;
			neighbor_coordinate.y += j;

			if (!this->map->has_tile(neighbor_coordinate))
			{
				continue;
			}

			auto& tile = this->map->get_tile(neighbor_coordinate);

			if (tile.is_impassable())
			{
				cost += this->impassable_weight;
			}
			else if (tile.is_wall())
			{
				cost += this->door_weight;
			}
		}
	}

	return tile_distance(tile_coordinate, stop_coordinate) + cost;
}

void thoreau::PathFinder::get_neighbors(
	const TileCoordinate& tile_coordinate,
	std::vector<TileCoordinate>& result) const
{
	auto add_neighbor = [this, &result, &tile_coordinate](int offset_x, int offset_y, int offset_layer)
	{
		auto neighbor = tile_coordinate;
		neighbor.x += offset_x;
		neighbor.y += offset_y;
		neighbor.layer += offset_layer;

		if (this->sectors.empty() ||
			this->sectors.count(sector_from_tile(neighbor)) != 0)
		{
			result.push_back(neighbor);
		}
	};

	if (this->map->can_move(tile_coordinate, -1, 0))
	{
		add_neighbor(-1, 0, 0);
	}

	if (this->map->can_move(tile_coordinate, 1, 0))
	{
		add_neighbor(1, 0, 0);
	}

	if (this->map->can_move(tile_coordinate, 0, -1))
	{
		add_neighbor(0, -1, 0);
	}

	if (this->map->can_move(tile_coordinate, 0, 1))
	{
		add_neighbor(0, 1, 0);
	}

	if (this->map->can_move(tile_coordinate, 0, 0, 1))
	{
		add_neighbor(0, 0, 1);
	}

	if (this->map->can_move(tile_coordinate, 0, 0, -1))
	{
		add_neighbor(0, 0, -1);
	}

	if (this->can_move_diagonally)
	{
		if (this->map->can_move(tile_coordinate, -1, -1, 0))
		{
			add_neighbor(-1, -1, 0);
		}

		if (this->map->can_move(tile_coordinate, 1, -1, 0))
		{
			add_neighbor(1, -1, 0);
		}

		if (this->map->can_move(tile_coordinate, -1, 1, 0))
		{
			add_neighbor(-1, 1, 0);
		}

		if (this->map->can_move(tile_coordinate, 1, 1, 0))
		{
			add_neighbor(1, 1, 0);
		}
	}
}
