// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cmath>
#include "autonomaus/core/rng.hpp"
#include "autonomaus/state/camera.hpp"
#include "autonomaus/state/walker.hpp"
#include "autonomaus/state/mapProvider.hpp"
#include "autonomaus/state/worldProvider.hpp"
#include "thoreau/sectorCoordinate.hpp"

autonomaus::Walker::Walker(
	const Autonomaus& autonomaus,
	float max_distance) :
		map(&autonomaus.get_map_state()->get_map()),
		view(autonomaus.get_world_state(), autonomaus.get_map_state()),
		path_finder(*this->map)
{
	this->autonomaus = &autonomaus;
	this->max_distance = max_distance;
	this->path_finder.set_neighbor_weighting_radius(1.0f);
}

float autonomaus::Walker::get_max_distance() const
{
	return this->max_distance;
}

void autonomaus::Walker::set_max_distance(float value)
{
	this->max_distance = value;
}

const thoreau::PathFinder& autonomaus::Walker::get_path() const
{
	return this->path_finder;
}

const autonomaus::View& autonomaus::Walker::get_view() const
{
	return this->view;
}

bool autonomaus::Walker::try_find_path(const thoreau::TileCoordinate& tile_coordinate)
{
	return try_find_path(this->view.get_player_tile_coordinate(), tile_coordinate);
}

bool autonomaus::Walker::try_find_path(
	const thoreau::TileCoordinate& from,
	const thoreau::TileCoordinate& to)
{
	if (!this->path_finder.try_find_path(from, to))
	{
		return false;
	}

	this->target_tile_coordinate = to;
	this->current_tile_coordinate = this->path_finder.at(0);
	this->current_path_index = 0;

	return true;
}

bool autonomaus::Walker::try_find_next_tile(
	thoreau::TileCoordinate& result,
	float max_distance,
	int flags)
{
	if (this->path_finder.empty())
	{
		return false;
	}

	std::size_t max_index;
	thoreau::TileCoordinate tile_coordinate;
	bool is_obstacle = false;
	if (find_next_obstacle(this->current_path_index, max_index, tile_coordinate))
	{
		is_obstacle = true;
		if (flags & continue_past_obstacle)
		{
			if (is_at_door())
			{
				is_obstacle = find_next_obstacle(max_index + 1, max_index, tile_coordinate);
			}
			else if (is_at_portal())
			{
				is_obstacle = find_next_obstacle(max_index, max_index, tile_coordinate);
			}
		}
	}

	if (is_obstacle)
	{
		auto player_coordinate = this->view.get_player_tile_coordinate();
		glm::vec2 difference;
		difference.x = tile_coordinate.x - player_coordinate.x;
		difference.y = tile_coordinate.y - player_coordinate.y;

		if (this->view.is_in_bounds(tile_coordinate) &&
			glm::length(difference) < max_distance)
		{
			result = tile_coordinate;
			this->current_tile_coordinate = tile_coordinate;
			this->current_path_index = max_index;
			return true;
		}
	}

	if (flags & greater_distance)
	{
		return find_distant_tile(result, max_index, max_distance);
	}
	else if (flags & greater_visibility)
	{
		return find_visible_tile(result, max_index, max_distance);
	}

	return false;
}

bool autonomaus::Walker::find_next_obstacle(
	std::size_t start_index, std::size_t& index,
	thoreau::TileCoordinate& tile_coordinate)
{
	index = this->path_finder.length() - 1;
	for (std::size_t i = start_index; i + 1 < this->path_finder.length(); ++i)
	{
		auto current_coordinate = this->path_finder.at(i);
		auto next_coordinate = this->path_finder.at(i + 1);
		auto& current_tile = this->map->get_tile(current_coordinate);
		auto& next_tile = this->map->get_tile(next_coordinate);

		if (current_tile.is_portal() && next_tile.is_portal())
		{
			index = i;
			tile_coordinate = next_coordinate;

			return true;
		}

		if (is_door_blocking(current_coordinate, next_coordinate))
		{
			index = i;
			if (next_tile.is_door())
			{
				tile_coordinate = next_coordinate;
			}
			else
			{
				tile_coordinate = current_coordinate;
			}

			return true;
		}
	}

	return false;
}

bool autonomaus::Walker::find_distant_tile(
	thoreau::TileCoordinate& result,
	std::size_t max_index,
	float max_distance)
{
	auto player_coordinate = this->view.get_player_tile_coordinate();

	// First, try and find a tile in view on the path.
	std::size_t best = this->path_finder.length();
	for (std::size_t i = max_index; i > this->current_path_index; --i)
	{
		auto current_coordinate = this->path_finder.at(i);
		if (this->view.is_in_bounds(current_coordinate))
		{
			glm::vec2 difference;
			difference.x = player_coordinate.x - current_coordinate.x;
			difference.y = player_coordinate.y - current_coordinate.y;
			if (glm::length(difference) < max_distance)
			{
				if (try_find_visible_tile(current_coordinate, result))
				{
					this->current_tile_coordinate = result;
					this->current_path_index = i;
					return true;
				}
				else if (best == this->path_finder.length())
				{
					best = i;
				}
			}
		}
	}

	if (best == this->path_finder.length())
	{
		// All the tiles are outside the viewport.
		return false;
	}

	result = this->path_finder.at(best);
	this->current_tile_coordinate = result;
	this->current_path_index = best;
	return true;
}

bool autonomaus::Walker::find_visible_tile(
	thoreau::TileCoordinate& result,
	std::size_t max_index,
	float max_distance)
{
	auto player_coordinate = this->view.get_player_tile_coordinate();

	typedef std::pair<std::size_t, thoreau::TileCoordinate> PathPoint;
	std::set<PathPoint> candidates;
	for (std::size_t i = max_index; i > this->current_path_index + this->max_distance; --i)
	{
		auto current_coordinate = this->path_finder.at(i);
		thoreau::TileCoordinate adjacent_coordinate;
		if (this->view.is_in_bounds(current_coordinate))
		{
			glm::vec2 difference;
			difference.x = player_coordinate.x - current_coordinate.x;
			difference.y = player_coordinate.y - current_coordinate.y;
			if (glm::length(difference) < max_distance)
			{
				if (try_find_visible_tile(current_coordinate, adjacent_coordinate))
				{
					candidates.emplace(i, adjacent_coordinate);
				}
			}
		}
	}

	if (candidates.empty())
	{
		return false;
	}

	PathPoint best;
	float best_f_score = 0;
	for (auto i: candidates)
	{
		int f_score = compute_tile_f_score(i.second);
		if (f_score > best_f_score)
		{
			best_f_score = f_score;
			best = i;
		}
	}

	if (best_f_score > 0)
	{
		result = best.second;
		this->current_tile_coordinate = best.second;
		this->current_path_index = best.first;
		return true;
	}

	return false;
}

bool autonomaus::Walker::snap_to_closest_tile(float max_distance)
{
	if (this->path_finder.empty())
	{
		return false;
	}

	typedef std::pair<float, std::size_t> DistanceIndexPair;
	auto compare_distance_index = [](DistanceIndexPair a, DistanceIndexPair b)
	{
		// We want to find the closest tile, and for collisions, the tile
		// furthest along the path.
		if (a.first == b.first)
		{
			return a.second < b.second;
		}
		else
		{
			return a.first < b.first;
		}
	};
	typedef std::set<DistanceIndexPair, decltype(compare_distance_index)> TileDistanceSet;
	TileDistanceSet tile_distances(compare_distance_index);

	auto player_coordinate = this->view.get_player_tile_coordinate();
	for (std::size_t i = this->current_path_index; i < this->path_finder.length(); ++i)
	{
		auto tile_coordinate = this->path_finder.at(i);
		glm::vec2 difference;
		difference.x = player_coordinate.x - tile_coordinate.x;
		difference.y = player_coordinate.y - tile_coordinate.y;
		auto distance = glm::length(difference);

		if (tile_coordinate.layer != player_coordinate.layer)
		{
			continue;
		}

		if (distance < 1.0f)
		{
			this->current_path_index = i;
			this->current_tile_coordinate = tile_coordinate;
			return true;
		}
		else if (distance < max_distance && is_line_clear(player_coordinate, tile_coordinate))
		{
			tile_distances.emplace(distance, i);
		}
	}

	for (auto i: tile_distances)
	{
		auto tile_coordinate = this->path_finder.at(i.second);
		if (this->view.is_in_bounds(tile_coordinate))
		{
			this->current_tile_coordinate = tile_coordinate;
			this->current_path_index = i.second;
			return true;
		}
	}

	return false;
}

bool autonomaus::Walker::rewind(float distance)
{
	if (this->current_path_index == 0)
	{
		return false;
	}

	std::size_t best = this->current_path_index - 1;
	auto tile_coordinate = this->path_finder.at(this->current_path_index);
	auto previous_coordinate = tile_coordinate;
	for (auto i = this->current_path_index - 1; i > 0; --i)
	{
		auto current_coordinate = this->path_finder.at(i);
		glm::vec2 difference;
		difference.x = tile_coordinate.x - current_coordinate.x;
		difference.y = tile_coordinate.y - current_coordinate.y;
		auto d = glm::length(difference);

		if (is_obstacle_blocking(previous_coordinate, current_coordinate))
		{
			break;
		}

		if (d > distance)
		{
			break;
		}
		
		best = i;
	}

	this->current_path_index = best;
	return true;
}

const thoreau::TileCoordinate& autonomaus::Walker::get_target_tile_coordinate() const
{
	return this->target_tile_coordinate;
}

const thoreau::TileCoordinate& autonomaus::Walker::get_current_tile_coordinate() const
{
	return this->current_tile_coordinate;
}

std::size_t autonomaus::Walker::get_current_path_index() const
{
	return this->current_path_index;
}

bool autonomaus::Walker::is_at_door() const
{
	if (this->path_finder.empty())
	{
		return false;
	}

	if (this->current_path_index >= this->path_finder.length() - 1)
	{
		return false;
	}

	auto current_coordinate = this->path_finder.at(this->current_path_index);
	auto next_coordinate = this->path_finder.at(this->current_path_index + 1);
	
	return is_door_blocking(current_coordinate, next_coordinate);
}

bool autonomaus::Walker::is_at_portal() const
{
	if (this->path_finder.empty())
	{
		return false;
	}

	if (this->current_path_index >= this->path_finder.length() - 1)
	{
		return false;
	}

	auto current_coordinate = this->path_finder.at(this->current_path_index);
	auto next_coordinate = this->path_finder.at(this->current_path_index + 1);
	
	return is_portal_blocking(current_coordinate, next_coordinate);
}

bool autonomaus::Walker::is_at_obstacle() const
{
	return is_at_door() || is_at_portal();
}

bool autonomaus::Walker::is_line_clear(
	const thoreau::TileCoordinate& a,
	const thoreau::TileCoordinate& b)
{
	if (a.layer != b.layer)
	{
		return false;
	}

	int x1 = a.x;
	int y1 = a.y;
	int x2 = b.x;
	int y2 = b.y;
	int layer = a.layer;

	if (x1 == x2)
	{
		int min_y = std::min(y1, y2);
		int max_y = std::max(y1, y2);
		thoreau::TileCoordinate previous = a;
		for (int y = min_y; y <= max_y; ++y)
		{
			thoreau::TileCoordinate current;
			current.x = x1;
			current.y = y;
			current.layer = layer;

			if (!this->map->can_move(previous, current) ||
				is_obstacle_blocking(previous, current))
			{
				return false;
			}

			previous = current;
		}
	}
	else if (y1 == y2)
	{
		int min_x = std::min(x1, x2);
		int max_x = std::max(x1, x2);
		thoreau::TileCoordinate previous = a;
		for (int x = min_x; x <= max_x; ++x)
		{
			thoreau::TileCoordinate current;
			current.x = x;
			current.y = y1;
			current.layer = layer;

			if (!this->map->can_move(previous, current) ||
				is_obstacle_blocking(previous, current))
			{
				return false;
			}

			previous = current;
		}
	}
	else
	{
		float delta_x = x2 - x1;
		float delta_y = y2 - y1;
		float delta_error = glm::abs(delta_y / delta_x);
		float error = delta_error - 0.5f;

		int y = y1;
		thoreau::TileCoordinate previous = a;
		for (int x = x1; x <= x2; ++x)
		{
			thoreau::TileCoordinate current;
			current.x = x;
			current.y = y;
			current.layer = layer;

			if (!this->map->can_move(previous, current) ||
				is_obstacle_blocking(previous, current))
			{
				return false;
			}

			error += delta_error;
			if (error >= 0.5f)
			{
				++y;
				error -= 1.0f;
			}
			
			previous = current;
		}
	}

	return true;
}

void autonomaus::Walker::update()
{
	this->view.update(Camera(this->autonomaus->get_draw_state()));
}

bool autonomaus::Walker::should_skip_tile(
	const thoreau::TileCoordinate& previous_coordinate,
	const thoreau::TileCoordinate& current_coordinate,
	int offset_x, int offset_y)
{
	if (!this->map->has_tile(current_coordinate))
	{
		return true;
	}

	if (!this->map->can_move(previous_coordinate, current_coordinate))
	{
		return true;
	}

	if (is_door_blocking(previous_coordinate, current_coordinate))
	{
		return true;
	}

	auto distance = glm::vec2(offset_x, offset_y);
	if (glm::length(distance) > this->max_distance)
	{
		return true;
	}

	auto center_coordinate = current_coordinate;
	center_coordinate.x += offset_x;
	center_coordinate.y += offset_y;
	if (!is_line_clear(center_coordinate, current_coordinate))
	{
		return false;
	}

	return false;
}

bool autonomaus::Walker::try_find_visible_tile(
	const thoreau::TileCoordinate& tile_coordinate,
	thoreau::TileCoordinate& result)
{
	std::set<thoreau::TileCoordinate> visible;
	std::set<thoreau::TileCoordinate> visited;
	do_try_find_visible_tile(tile_coordinate, tile_coordinate, 0, 0, visible, visited);

	if (visible.empty())
	{
		return false;
	}

	typedef std::pair<int, thoreau::TileCoordinate> ScoreCoordPair;
	auto compare_f_score = [](ScoreCoordPair a, ScoreCoordPair b)
	{
		return a.first > b.first;
	};
	typedef std::multiset<ScoreCoordPair, decltype(compare_f_score)> ScoreSet;
	ScoreSet scores(compare_f_score);

	for (auto tile_coordinate: visible)
	{
		ScoreCoordPair pair;
		pair.first = compute_tile_f_score(tile_coordinate);
		pair.second = tile_coordinate;

		scores.insert(pair);
	}

	std::vector<thoreau::TileCoordinate> candidates;
	auto current = scores.begin();
	auto end = scores.end();
	while (current != end)
	{
		candidates.push_back(current->second);

		auto previous = current++;
		if (current->first != previous->first)
		{
			break;
		}
	}

	result = candidates[RNG::next(candidates.size())];
	return true;
}

void autonomaus::Walker::do_try_find_visible_tile(
	const thoreau::TileCoordinate& center_coordinate,
	const thoreau::TileCoordinate& previous_coordinate,
	int offset_x, int offset_y,
	std::set<thoreau::TileCoordinate>& visible,
	std::set<thoreau::TileCoordinate>& visited)
{
	thoreau::TileCoordinate tile_coordinate = center_coordinate;
	tile_coordinate.x += offset_x;
	tile_coordinate.y += offset_y;
	if (visited.count(tile_coordinate) != 0)
	{
		return;
	}
	else
	{
		visited.insert(tile_coordinate);
	}

	if (should_skip_tile(previous_coordinate, tile_coordinate, offset_x, offset_y))
	{
		return;
	}

	if (!this->view.occluded(tile_coordinate) &&
		this->view.is_in_bounds(tile_coordinate))
	{
		visible.insert(tile_coordinate);
	}

	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			if (i == 0 || j == 0)
			{
				do_try_find_visible_tile(
					center_coordinate,
					tile_coordinate,
					offset_x + i, offset_y + j,
					visible, visited);
			}
		}
	}
}

int autonomaus::Walker::compute_tile_f_score(const thoreau::TileCoordinate& tile_coordinate)
{
	std::set<thoreau::TileCoordinate> visited;
	int f_score = 0;
	do_compute_tile_f_score(tile_coordinate, tile_coordinate, 0, 0, f_score, visited);

	return f_score;
}

void autonomaus::Walker::do_compute_tile_f_score(
	const thoreau::TileCoordinate& center_coordinate,
	const thoreau::TileCoordinate& previous_coordinate,
	int offset_x, int offset_y,
	int& f_score,
	std::set<thoreau::TileCoordinate>& visited)
{
	thoreau::TileCoordinate tile_coordinate = center_coordinate;
	tile_coordinate.x += offset_x;
	tile_coordinate.y += offset_y;
	if (visited.count(tile_coordinate) != 0)
	{
		return;
	}
	else
	{
		visited.insert(tile_coordinate);
	}

	if (should_skip_tile(previous_coordinate, tile_coordinate, offset_x, offset_y))
	{
		return;
	}

	auto& tile = this->map->get_tile(tile_coordinate);
	auto& previous = this->map->get_tile(tile_coordinate);
	if (!this->view.occluded(tile_coordinate) &&
		this->view.is_in_bounds(tile_coordinate) &&
		!tile.is_wall() && !tile.is_impassable() &&
		!previous.is_wall() && !previous.is_impassable())
	{
		++f_score;
	}

	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			do_compute_tile_f_score(
				center_coordinate,
				tile_coordinate,
				offset_x + i, offset_y + j,
				f_score, visited);
		}
	}
}

bool autonomaus::Walker::is_door_blocking(
	const thoreau::TileCoordinate& current_coordinate,
	const thoreau::TileCoordinate& next_coordinate) const
{
	if (!this->map->has_tile(current_coordinate) ||
		!this->map->has_tile(next_coordinate))
	{
		return false;
	}

	auto& current = this->map->get_tile(current_coordinate);
	auto& next = this->map->get_tile(next_coordinate);

	return current.is_door() && next.is_door();
}

bool autonomaus::Walker::is_portal_blocking(
	const thoreau::TileCoordinate& current_coordinate,
	const thoreau::TileCoordinate& next_coordinate) const
{
	if (!this->map->has_tile(current_coordinate))
	{
		return false;
	}

	if (!this->map->has_tile(next_coordinate))
	{
		return false;
	}

	auto& current = this->map->get_tile(current_coordinate);
	auto& next = this->map->get_tile(next_coordinate);

	bool is_portal = current.is_portal() && next.is_portal();
	bool different_layer = current_coordinate.layer != next_coordinate.layer;

	return is_portal && different_layer;
}

bool autonomaus::Walker::is_obstacle_blocking(
	const thoreau::TileCoordinate& current_coordinate,
	const thoreau::TileCoordinate& next_coordinate) const
{
	return is_door_blocking(current_coordinate, next_coordinate) ||
		is_portal_blocking(current_coordinate, next_coordinate);
}
