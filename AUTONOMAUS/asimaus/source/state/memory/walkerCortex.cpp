// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <future>
#include <unordered_map>
#include "autonomaus/core/config.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/core/rng.hpp"
#include "autonomaus/state/mapProvider.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/memory/minimapPanelCortex.hpp"
#include "autonomaus/state/memory/toolTipCortex.hpp"
#include "autonomaus/state/memory/walkerCortex.hpp"
#include "autonomaus/state/memory/worldCortex.hpp"
#include "autonomaus/state/memory/viewCortex.hpp"

const int autonomaus::WalkerCortex::MAX_VISIBILITY_RADIUS =
	Config::get<int>("game", "WalkerCortex", "MAX_VISIBILITY_RADIUS");
const int autonomaus::WalkerCortex::MAX_SNAP_RADIUS =
	Config::get<int>("game", "WalkerCortex", "MAX_SNAP_RADIUS");
const int autonomaus::WalkerCortex::PAN_RADIUS =
	Config::get<int>("game", "WalkerCortex", "PAN_RADIUS");
const int autonomaus::WalkerCortex::MAX_MINIMAP_RADIUS =
	Config::get<int>("game", "WalkerCortex", "MAX_MINIMAP_RADIUS");
const int autonomaus::WalkerCortex::MAX_WORLD_RADIUS =
	Config::get<int>("game", "WalkerCortex", "MAX_WORLD_RADIUS");

struct autonomaus::WalkerCortex::FindPathAction : public Action
{
	FindPathAction(
		WalkerCortex* walker,
		const thoreau::TileCoordinate& from,
		const thoreau::TileCoordinate& to);
	~FindPathAction() = default;

	Result get_result() override;
	void update();

private:
	WalkerCortex* cortex;

	std::future<thoreau::PathFinder> query;

	Result::StatusReference status;
	Result result;
};

struct autonomaus::WalkerCortex::StepAction : public Action
{
	StepAction(
		WalkerCortex* cortex,
		const thoreau::PathFinder& path);
	~StepAction();

	void cancel();

	Result get_result() override;
	void update() override;

	WalkerCortex* cortex;
	thoreau::PathFinder path;
	std::size_t current_index = 0;

private:
	enum
	{
		state_start,
		state_plan,
		state_focus_obstacle,
		state_peek_door,
		state_peeking_door,
		state_open_door,
		state_opened_door,
		state_use_portal,
		state_used_portal,
		state_decide,
		state_move,
		state_stop,
		state_pan,
		state_click,
		state_minimap_click,
		state_focus,
		state_focus_minimap,
		state_failure,
		state_success,
		state_finish,
		state_wait,
		state_wait_stop,
		state_wait_poke,
		state_done
	};
	int state = state_start;
	int next_state = state_failure;

	std::future<std::size_t> nearest_point_query;

	std::unordered_set<std::size_t> passable_obstacles;

	Result current_result;

	int click_frame;
	void click();

	int distance = 0;
	int desired_distance = 0;
	thoreau::TileCoordinate target_previous_tile;
	glm::vec3 target_previous_world;
	int last_frame = 0;
	void prepare_for_wait(int distance, int state);

	bool waiting_for_poke = false;
	int poke_success, poke_failure;
	void wait_for_poke(int success_state, int failure_state);

	float target_time = 0.0f;

	std::size_t peek_index;
	std::size_t max_world_index = 0;
	std::size_t max_minimap_index = 0;
	thoreau::TileCoordinate pending_tile_coordinate;

	Result::StatusReference status;
	Result final_result;
};

autonomaus::WalkerCortex::WalkerCortex(Autonomaus& autonomaus) :
	Cortex("Game::Walker", autonomaus),
	current_path(autonomaus.get_map_state()->get_map())
{
	on_poke(
		"find_path",
		[this](auto& message)
		{
			auto x = message.get_integer("x");
			auto y = message.get_integer("y");
			auto layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(x, y, layer);

			return this->find_path(tile_coordinate);
		});

	on_poke(
		"enable_minimap",
		[this](auto)
		{
			return this->enable_minimap();
		});

	on_poke(
		"disable_minimap",
		[this](auto)
		{
			return this->disable_minimap();
		});

	on_pull(
		"is_minimap_enabled",
		[this](auto)
		{
			Message result;
			result.set_boolean("result", this->is_minimap_enabled());
			return result;
		});

	on_poke(
		"step",
		[this](auto& message)
		{
			return this->step();
		});

	on_poke(
		"cancel",
		[this](auto& message)
		{
			return this->cancel();
		});

	on_poke(
		"finish",
		[this](auto& message)
		{
			return this->finish();
		});

	on_pull(
		"at_destination",
		[this](auto& message)
		{
			Message result;
			result.set_boolean("result", this->at_destination(message.get_single("distance")));
			return result;
		});

	on_pull(
		"pending",
		[this](auto)
		{
			Message result;
			result.set_boolean("result", this->pending());
			return result;
		});
}

autonomaus::Cortex::Result
autonomaus::WalkerCortex::find_path(const thoreau::TileCoordinate& destination)
{
	Log::info(
		"(WalkerCortex) Finding path to ({}, {}; {})...",
		destination.x,
		destination.y,
		destination.layer);

	auto view = get_autonomaus().get_userdata<ViewCortex>();
	if (view == nullptr)
	{
		Log::warn("(WalkerCortex) View cortex not valid. Bad logic!");
		return Result::Status::failure;
	}

	auto source = view->get_target_tile_coordinate();
	Log::info(
		"(WalkerCortex) Currently on tile ({}, {}; {}).",
		source.x,
		source.y,
		source.layer);

	return perform(new FindPathAction(this, source, destination));
}

autonomaus::Cortex::Result autonomaus::WalkerCortex::disable_minimap()
{
	this->minimap_enabled = false;
	return Result::Status::success;
}

autonomaus::Cortex::Result autonomaus::WalkerCortex::enable_minimap()
{
	this->minimap_enabled = true;
	return Result::Status::success;
}

bool autonomaus::WalkerCortex::is_minimap_enabled() const
{
	return this->minimap_enabled;
}

autonomaus::Cortex::Result autonomaus::WalkerCortex::step()
{
	if (this->current_path.empty())
	{
		Log::warn("(WalkerCortex) Path empty!");
		return Result::Status::failure;
	}

	return perform(new StepAction(this, this->current_path));
}

autonomaus::Cortex::Result autonomaus::WalkerCortex::cancel()
{
	this->current_path.clear();
	return Result::Status::success;
}

autonomaus::Cortex::Result autonomaus::WalkerCortex::finish()
{
	this->current_path.clear();
	return Result::Status::success;
}

bool autonomaus::WalkerCortex::at_destination(float distance) const
{
	if (this->current_path.empty())
	{
		return true;
	}

	auto view = get_autonomaus().get_userdata<ViewCortex>();
	if (view == nullptr)
	{
		return false;
	}

	auto destination_tile_coordinate = this->current_path.at(this->current_path.length() - 1);
	auto current_tile_coordinate = view->get_target_tile_coordinate();

	if (destination_tile_coordinate.layer != current_tile_coordinate.layer)
	{
		return false;
	}

	int distance_x = destination_tile_coordinate.x - current_tile_coordinate.x;
	int distance_y = destination_tile_coordinate.y - current_tile_coordinate.y;
	int distance_from_destination = distance_x * distance_x + distance_y * distance_y;

	return distance_from_destination < distance * distance;
}

bool autonomaus::WalkerCortex::pending() const
{
	return !this->current_path.empty();
}

std::size_t autonomaus::WalkerCortex::get_nearest_path_point(const thoreau::PathFinder& path)
{
	auto view = get_autonomaus().get_userdata<ViewCortex>();
	if (view == nullptr)
	{
		return 0;
	}

	auto current_tile_coordinate = view->get_target_tile_coordinate();

	std::vector<std::pair<std::size_t, int>> points;
	for (std::size_t i = 0; i < path.length(); ++i)
	{
		auto tile_coordinate = path.at(i);
		if (tile_coordinate.layer == current_tile_coordinate.layer)
		{
			int distance_x = tile_coordinate.x - current_tile_coordinate.x;
			int distance_y = tile_coordinate.y - current_tile_coordinate.y;
			int distance = distance_x * distance_x + distance_y * distance_y;

			if (distance < MAX_SNAP_RADIUS * MAX_SNAP_RADIUS)
			{
				points.emplace_back(i, distance);
			}
		}
	}

	if (points.empty())
	{
		return 0;
	}

	std::sort(
		points.begin(),
		points.end(),
		[](auto& a, auto& b)
		{
			return a.second < b.second;
		});

	for (auto i: points)
	{
		if (reachable_without_obstacle(current_tile_coordinate, path.at(i.first)))
		{
			return i.first;
		}
	}

	return 0;
}

bool autonomaus::WalkerCortex::reachable_without_obstacle(
	const thoreau::TileCoordinate& start,
	const thoreau::TileCoordinate& stop) const
{
	auto& map = get_autonomaus().get_map_state()->get_map();
	thoreau::PathFinder path(map);
	path.add_sector_restriction(thoreau::sector_from_tile(start));
	path.add_sector_restriction(thoreau::sector_from_tile(stop));

	if (path.try_find_path(start, stop))
	{
		if ((int)path.length() > MAX_SNAP_RADIUS)
		{
			return false;
		}

		for (std::size_t i = 0; i < path.length() - 1; ++i)
		{
			if (is_obstacle_blocking(path.at(i), path.at(i + 1)))
			{
				return false;
			}
		}

		return true;
	}

	return false;
}

bool autonomaus::WalkerCortex::do_reachable_without_obstacle(
	const thoreau::Map& map,
	const thoreau::TileCoordinate& current,
	const thoreau::TileCoordinate& stop,
	std::unordered_set<thoreau::TileCoordinate>& visited) const
{
	if (current == stop)
	{
		return true;
	}

	if (visited.count(current) != 0)
	{
		return false;
	}

	visited.insert(current);

	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			if (i != 0 && j != 0)
			{
				continue;
			}

			auto next = current;
			next.x += i;
			next.y += j;

			int distance_x = next.x - stop.x;
			int distance_y = next.y - stop.y;
			int distance = distance_x * distance_x + distance_y * distance_y;
			if (distance > MAX_SNAP_RADIUS * MAX_SNAP_RADIUS)
			{
				continue;
			}

			if (!map.can_move(current, next) || !is_door_blocking(current, next))
			{
				continue;
			}

			if (do_reachable_without_obstacle(map, next, stop, visited))
			{
				return true;
			}
		}
	}

	return false;
}

bool autonomaus::WalkerCortex::is_obstacle(
	const thoreau::PathFinder& path,
	std::size_t index) const
{
	if (index + 1 < path.length())
	{
		return is_obstacle_blocking(path.at(index), path.at(index + 1));
	}

	return false;
}

bool autonomaus::WalkerCortex::is_obstacle_blocking(
	const thoreau::TileCoordinate& current_coordinate,
	const thoreau::TileCoordinate& next_coordinate) const
{
	return is_door_blocking(current_coordinate, next_coordinate) ||
		is_portal_blocking(current_coordinate, next_coordinate);
}

bool autonomaus::WalkerCortex::is_door_blocking(
	const thoreau::TileCoordinate& current_coordinate,
	const thoreau::TileCoordinate& next_coordinate) const
{
	auto& map = get_autonomaus().get_map_state()->get_map();

	if (!map.has_tile(current_coordinate) ||
		!map.has_tile(next_coordinate))
	{
		return false;
	}

	auto& current_tile = map.get_tile(current_coordinate);
	auto& next_tile = map.get_tile(next_coordinate);

	int offset_x = next_coordinate.x - current_coordinate.x;
	int offset_y = next_coordinate.y - current_coordinate.y;

	int current_flags = current_tile.get_flags();
	int next_flags = next_tile.get_flags();
	if (offset_x < 0 &&
		(current_flags & thoreau::Tile::flag_door_left ||
		next_flags & thoreau::Tile::flag_door_right))
	{
		return true;
	}

	if (offset_x > 0 &&
		(current_flags & thoreau::Tile::flag_door_right ||
		next_flags & thoreau::Tile::flag_door_left))
	{
		return true;
	}

	if (offset_y > 0 &&
		(current_flags & thoreau::Tile::flag_door_top ||
		next_flags & thoreau::Tile::flag_door_bottom))
	{
		return true;
	}

	if (offset_y < 0 &&
		(current_flags & thoreau::Tile::flag_door_bottom ||
		next_flags & thoreau::Tile::flag_door_top))
	{
		return true;
	}

	return false;
}

bool autonomaus::WalkerCortex::is_portal_blocking(
	const thoreau::TileCoordinate& current_coordinate,
	const thoreau::TileCoordinate& next_coordinate) const
{
	auto& map = get_autonomaus().get_map_state()->get_map();

	if (!map.has_tile(current_coordinate) ||
		!map.has_tile(next_coordinate))
	{
		return false;
	}

	auto& current_tile = map.get_tile(current_coordinate);
	if (current_coordinate.layer != next_coordinate.layer &&
		current_tile.is_portal())
	{
		return true;
	}

	return false;
}

bool autonomaus::WalkerCortex::find_most_visible_tile(
	const thoreau::PathFinder& path,
	std::size_t index,
	std::size_t count,
	thoreau::TileCoordinate& result) const
{
	auto view = get_autonomaus().get_userdata<ViewCortex>();
	if (view == nullptr)
	{
		return false;
	}

	std::unordered_set<thoreau::TileCoordinate> pending_tiles;
	for (std::size_t k = 0; k + index < path.length() && k < count; ++k)
	{
		for (int i = -MAX_VISIBILITY_RADIUS; i <= MAX_VISIBILITY_RADIUS; ++i)
		{
			for (int j = -MAX_VISIBILITY_RADIUS; j <= MAX_VISIBILITY_RADIUS; ++j)
			{
				auto tile_coordinate = path.at(k + index);
				tile_coordinate.x += i;
				tile_coordinate.y += j;

				if (!view->possibly_occluded(tile_coordinate))
				{
					pending_tiles.insert(tile_coordinate);
				}
			}
		}
	}

	if (pending_tiles.empty())
	{
		return false;
	}

	std::vector<std::pair<int, thoreau::TileCoordinate>> f_scores;
	for (auto& i: pending_tiles)
	{
		f_scores.emplace_back(compute_tile_f_score(i), i);
	}

	std::sort(
		f_scores.begin(),
		f_scores.end(),
		[](auto& a, auto& b) { return a.first > b.first; });
	result = f_scores.front().second;
	return true;
}

int autonomaus::WalkerCortex::compute_tile_f_score(
	const thoreau::TileCoordinate& tile_coordinate) const
{
	auto view = get_autonomaus().get_userdata<ViewCortex>();
	if (view == nullptr)
	{
		return -1;
	}

	auto& map = get_autonomaus().get_map_state()->get_map();

	std::unordered_set<thoreau::TileCoordinate> visited;
	int f_score = 0;
	do_compute_tile_f_score(
		map, view,
		tile_coordinate, tile_coordinate,
		0, 0,
		f_score, visited);

	return f_score;
}

bool autonomaus::WalkerCortex::should_skip_tile(
	const thoreau::Map& map,
	const thoreau::TileCoordinate& previous_coordinate,
	const thoreau::TileCoordinate& current_coordinate,
	int offset_x, int offset_y) const
{
	if (!map.has_tile(current_coordinate))
	{
		return true;
	}

	if (!map.can_move(previous_coordinate, current_coordinate))
	{
		return true;
	}

	if (is_door_blocking(previous_coordinate, current_coordinate))
	{
		return true;
	}

	auto distance = glm::vec2(offset_x, offset_y);
	if (glm::length(distance) > MAX_VISIBILITY_RADIUS)
	{
		return true;
	}

	return false;
}

void autonomaus::WalkerCortex::do_compute_tile_f_score(
	const thoreau::Map& map,
	const ViewCortex* view,
	const thoreau::TileCoordinate& center_coordinate,
	const thoreau::TileCoordinate& previous_coordinate,
	int offset_x, int offset_y,
	int& f_score,
	std::unordered_set<thoreau::TileCoordinate>& visited) const
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

	if (should_skip_tile(map, previous_coordinate, tile_coordinate, offset_x, offset_y))
	{
		return;
	}

	auto& tile = map.get_tile(tile_coordinate);
	auto& previous = map.get_tile(tile_coordinate);
	if (!view->possibly_occluded(tile_coordinate))
	{
		++f_score;
	}
	if (view->is_in_bounds(tile_coordinate))
	{
		++f_score;
	}
	if (!tile.is_wall())
	{
		++f_score;
	}
	if (!tile.is_impassable())
	{
		++f_score;
	}
	if (!previous.is_wall())
	{
		++f_score;
	}
	if (!previous.is_impassable())
	{
		++f_score;
	}

	for (int i = -1; i <= 1; ++i)
	{
		for (int j = -1; j <= 1; ++j)
		{
			do_compute_tile_f_score(
				map,
				view,
				center_coordinate,
				tile_coordinate,
				offset_x + i, offset_y + j,
				f_score, visited);
		}
	}
}

autonomaus::WalkerCortex::FindPathAction::FindPathAction(
	WalkerCortex* cortex,
	const thoreau::TileCoordinate& from,
	const thoreau::TileCoordinate& to) :
		status(std::make_shared<Result::Status>(Result::Status::working)),
		result(status)
{
	this->cortex = cortex;

	auto& map = cortex->get_autonomaus().get_map_state()->get_map();
	this->query = std::async(std::launch::async,
		[map, from, to]
		{
			thoreau::PathFinder result(map);

			static const float NEIGHBOR_WEIGHT_RADIUS = Config::get<float>(
				"game", "WalkerCortex", "NEIGHBOR_WEIGHT_RADIUS");
			result.set_neighbor_weighting_radius(NEIGHBOR_WEIGHT_RADIUS);

			static const float WALL_WEIGHT = Config::get<float>(
				"game", "WalkerCortex", "WALL_WEIGHT");
			result.set_wall_weight(WALL_WEIGHT);

			static const float IMPASSABLE_WEIGHT = Config::get<float>(
				"game", "WalkerCortex", "IMPASSABLE_WEIGHT");
			result.set_impassable_weight(IMPASSABLE_WEIGHT);

			static const float DOOR_WEIGHT = Config::get<float>(
				"game", "WalkerCortex", "DOOR_WEIGHT");
			result.set_door_weight(DOOR_WEIGHT);

			static const float PORTAL_WEIGHT = Config::get<float>(
				"game", "WalkerCortex", "PORTAL_WEIGHT");
			result.set_portal_weight(PORTAL_WEIGHT);

			result.try_find_path(from, to);
			return result;
		});
}

autonomaus::Cortex::Result autonomaus::WalkerCortex::FindPathAction::get_result()
{
	return this->result;
}

void autonomaus::WalkerCortex::FindPathAction::update()
{
	auto status = this->query.wait_for(std::chrono::seconds(0));
	if (status == std::future_status::ready)
	{
		// We need to set the map, since the old map is no longer valid.
		auto& map = cortex->get_autonomaus().get_map_state()->get_map();

		this->cortex->current_path = thoreau::PathFinder(map, this->query.get());
		if (this->cortex->current_path.empty())
		{
			Log::warn("(WalkerCortex::FindPathAction) Couldn't find path.");
			*this->status = Result::Status::failure;
		}
		else
		{
			Log::info("(WalkerCortex::FindPathAction) Found path, {} tiles long.", this->cortex->current_path.length());
			*this->status = Result::Status::success;
		}
	}
}

autonomaus::WalkerCortex::StepAction::StepAction(
	WalkerCortex* cortex,
	const thoreau::PathFinder& path) :
		path(path),
		status(std::make_shared<Result::Status>(Result::Status::working)),
		final_result(status, [this] { this->cancel(); })

{
	this->cortex = cortex;

	this->nearest_point_query = std::async(
		std::launch::async,
		[this, path] { return this->cortex->get_nearest_path_point(path); });
}

autonomaus::WalkerCortex::StepAction::~StepAction()
{
	cancel();
	this->final_result.terminate();
}

void autonomaus::WalkerCortex::StepAction::cancel()
{
	if (this->current_result.pending())
	{
		this->current_result.cancel();
	}

	if (*this->status == Result::Status::working)
	{
		*this->status = Result::Status::failure;
	}

	this->state = state_done;
}

autonomaus::Cortex::Result autonomaus::WalkerCortex::StepAction::get_result()
{
	return this->final_result;
}

void autonomaus::WalkerCortex::StepAction::update()
{
	auto& map = this->cortex->get_autonomaus().get_map_state()->get_map();

	auto view = this->cortex->get_autonomaus().get_userdata<ViewCortex>();
	if (view == nullptr)
	{
		Log::warn("(WalkerCortex::StepAction) View cortex not valid. Bad logic!");
		cancel();
		return;
	}

	auto world = this->cortex->get_autonomaus().get_userdata<WorldCortex>();
	if (world == nullptr)
	{
		Log::warn("(WalkerCortex::StepAction) World cortex not valid. Bad logic!");
		cancel();
		return;
	}

	auto tool_tip = this->cortex->get_autonomaus().get_userdata<ToolTipCortex>();
	if (tool_tip == nullptr)
	{
		Log::warn("(WalkerCortex::StepAction) Tool tip cortex not valid. Bad logic!");
		cancel();
		return;
	}

	switch (this->state)
	{
		case state_start:
			{
				auto status = this->nearest_point_query.wait_for(std::chrono::seconds(0));
				if (status == std::future_status::ready)
				{
					this->current_index = this->nearest_point_query.get();
					this->state = state_plan;
				}
			}
			break;
		case state_plan:
			{
				bool found = false;

				auto minimap_panel = this->cortex->get_autonomaus().get_userdata<MinimapPanelCortex>();
				if (minimap_panel != nullptr && minimap_panel->is_visible() && this->cortex->minimap_enabled)
				{
					auto center_tile_coordinate = this->path.at(this->current_index);
					for (std::size_t i = this->current_index; i < this->path.length(); ++i)
					{
						auto tile_coordinate = this->path.at(i);

						auto distance_x = tile_coordinate.x - center_tile_coordinate.x;
						auto distance_y = tile_coordinate.y - center_tile_coordinate.y;
						int distance = distance_x * distance_x + distance_y * distance_y;

						if (distance > MAX_MINIMAP_RADIUS * MAX_MINIMAP_RADIUS)
						{
							break;
						}

						if (!minimap_panel->is_tile_clickable(tile_coordinate))
						{
							break;
						}

						this->max_minimap_index = i;
					}
				}

				auto center_tile_coordinate = view->get_target_tile_coordinate();
				for (std::size_t i = this->current_index; i < this->path.length(); ++i)
				{
					auto tile_coordinate = this->path.at(i);
					auto distance_x = tile_coordinate.x - center_tile_coordinate.x;
					auto distance_y = tile_coordinate.y - center_tile_coordinate.y;
					int distance = distance_x * distance_x + distance_y * distance_y;

					if (distance > MAX_WORLD_RADIUS * MAX_WORLD_RADIUS)
					{
						break;
					}

					if (!view->is_in_bounds(tile_coordinate))
					{
						break;
					}

					found = true;
					this->max_world_index = i;
				}

				if (!found)
				{
					this->cortex->cancel();

					Log::warn("(WalkerCortex::StepAction) No visible tiles found along path. Woops!");
					cancel();
					break;
				}

				auto max_index = std::max(this->max_minimap_index, this->max_world_index);

				std::size_t obstacle_index;
				bool obstacle = false;
				bool obstacle_visible = false;
				for (std::size_t i = this->current_index; i <= max_index; ++i)
				{
					if (this->cortex->is_obstacle(this->path, i) &&
						this->passable_obstacles.count(i) == 0)
					{
						obstacle_index = i;
						obstacle = true;

						obstacle_visible = view->is_in_bounds(this->path.at(i));
						break;
					}
				}

				if (!obstacle_visible)
				{
					if (obstacle)
					{
						this->max_minimap_index = std::min(this->max_minimap_index, obstacle_index);
						this->max_world_index = std::min(this->max_world_index, obstacle_index);
					}

					this->state = state_decide;
				}
				else
				{
					Log::info(
						"(WalkerCortex::StepAction) Obstacle along path,"
						" on tile ({}, {}; {}) (path index: {}). Focusing...",
						this->path.at(obstacle_index).x,
						this->path.at(obstacle_index).y,
						this->path.at(obstacle_index).layer,
						obstacle_index);

					this->state = state_focus_obstacle;
					this->peek_index = obstacle_index;
				}
			}
			break;
		case state_focus_obstacle:
			{
				auto tile_coordinate = this->path.at(this->peek_index);
				if (!map.has_tile(tile_coordinate))
				{
					Log::info(
						"(WalkerCortex::StepAction) Can't focus obstacle: "
						" Map doesn't have tile ({}, {}; {}). Bad logic!",
						tile_coordinate.x,
						tile_coordinate.y,
						tile_coordinate.layer);
				}

				auto& tile = map.get_tile(tile_coordinate);
				if (tile.is_portal() && this->peek_index + 1 < this->path.length())
				{
					Log::info("(WalkerCortex::StepAction) Obstacle is portal.");
					this->next_state = state_use_portal;
				}
				else if (tile.is_door())
				{
					Log::info("(WalkerCortex::StepAction) Obstacle is door.");
					this->next_state = state_peek_door;
				}
				else
				{
					Log::warn("(WalkerCortex::StepAction) Unknown obstacle. Bad logic!");
					this->state = state_failure;
					break;
				}

				this->current_result = view->focus_y(tile_coordinate);
				this->state = state_wait;
			}
			break;
		case state_peek_door:
			{
				Log::info("(WalkerCortex::StepAction) Peeking to see if door is open...");
				auto tile_coordinate = this->path.at(this->peek_index);
				this->current_result = world->peek_door(tile_coordinate);
				this->state = state_peeking_door;
			}
			break;
		case state_peeking_door:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					this->passable_obstacles.insert(this->peek_index);
					this->state = this->state_plan;
				}
				else
				{
					auto tile_coordinate = this->path.at(this->peek_index);
					this->current_result = world->open_door(tile_coordinate);
					this->state = state_open_door;
				}
			}
			break;
		case state_open_door:
			if (!this->current_result.pending())
			{
				if (this->current_result.get() == Result::Status::success)
				{
					wait_for_poke(state_opened_door, state_peek_door);
				}
				else
				{
					this->state = state_failure;
				}
			}
			break;
		case state_opened_door:
			if (view->get_target_idle_time() > 0.6)
			{
				this->state = state_success;
			}
			break;
		case state_use_portal:
			{
				auto current_tile_coordinate = this->path.at(this->peek_index);
				auto next_tile_coordinate = this->path.at(this->peek_index + 1);
				
				if (current_tile_coordinate.layer < next_tile_coordinate.layer)
				{
					this->current_result = world->ascend(current_tile_coordinate);
				}
				else if (current_tile_coordinate.layer > next_tile_coordinate.layer)
				{
					this->current_result = world->descend(current_tile_coordinate);
				}
				else
				{
					Log::warn("(WalkerCortex::StepAction) Portal obstacle, but to the same level. Bad logic!");
					this->state = state_failure;
					break;
				}

				wait_for_poke(state_used_portal, state_use_portal);
			}
			break;
		case state_used_portal:
			{
				auto next_tile_coordinate = this->path.at(this->peek_index + 1);
				auto current_tile_coordinate = view->get_target_tile_coordinate();
				if (current_tile_coordinate.layer == next_tile_coordinate.layer)
				{
					this->state = state_success;
				}
				else if (view->get_target_idle_time() > 1.2f)
				{
					Log::warn(
						"(WalkerCortex::StepAction) Used portal at ({}, {}; {}), but still on same layer.",
						this->path.at(this->peek_index).x,
						this->path.at(this->peek_index).y,
						this->path.at(this->peek_index).layer);
					this->state = state_failure;
				}
			}
			break;
		case state_decide:
			{
				if (this->max_minimap_index > this->max_world_index)
				{
					auto minimap_panel = this->cortex->get_autonomaus().get_userdata<MinimapPanelCortex>();
					if (minimap_panel != nullptr && minimap_panel->is_visible() && this->cortex->minimap_enabled)
					{
						this->pending_tile_coordinate = this->path.at(this->max_minimap_index);
						this->current_result = view->focus_y(this->pending_tile_coordinate);
						this->state = state_focus_minimap;
						break;
					}
				}

				auto count = this->max_world_index - this->current_index + 1;
				thoreau::TileCoordinate tile_coordinate;
				if (!this->cortex->find_most_visible_tile(this->path, this->current_index, count, tile_coordinate))
				{
					tile_coordinate = this->path.at(this->max_world_index);
				}

				this->pending_tile_coordinate = tile_coordinate;
				this->current_result = view->focus_y(pending_tile_coordinate);
				this->state = state_focus;
			}
			break;
		case state_focus:
			if (!this->current_result.pending())
			{
				this->current_result = world->hover(this->pending_tile_coordinate);
				this->state = state_move;
			}
			break;
		case state_focus_minimap:
			if (!this->current_result.pending())
			{
				auto minimap_panel = this->cortex->get_autonomaus().get_userdata<MinimapPanelCortex>();
				if (minimap_panel == nullptr)
				{
					Log::warn("(WalkerCortex::StepAction) Minimap panel cortex not valid. Bad logic!");
					this->state = state_failure;
					break;
				}

				this->current_result = minimap_panel->click_tile(this->pending_tile_coordinate);
				this->state = state_minimap_click;
			}
		case state_move:
			{
				if (this->current_result.pending())
				{
					auto mouse_tile_coordinate = view->get_tile_under_mouse();
					auto distance = glm::vec2(
						mouse_tile_coordinate.x - this->pending_tile_coordinate.x,
						mouse_tile_coordinate.y - this->pending_tile_coordinate.y);
					if (glm::length(distance) <= 1 && !tool_tip->is_visible())
					{
						this->current_result.cancel();
						click();
					}
				}
				else
				{
					if (this->current_result.get() == Result::Status::success)
					{
						if (tool_tip->is_visible())
						{
							this->state = state_pan;
						}
						else
						{
							click();
						}
					}
					else
					{
						this->state = state_failure;
					}
				}
			}
			break;
		case state_pan:
			{
				std::vector<thoreau::TileCoordinate> candidates;
				for (auto i = -PAN_RADIUS; i <= PAN_RADIUS; ++i)
				{
					for (auto j = -PAN_RADIUS; j <= PAN_RADIUS; ++j)
					{
						auto candidate = this->pending_tile_coordinate;
						candidate.x += i;
						candidate.y += j;

						if (!view->possibly_occluded(candidate) &&
							this->cortex->reachable_without_obstacle(candidate, this->pending_tile_coordinate))
						{
							candidates.push_back(candidate);
						}
					}
				}

				if (candidates.empty())
				{
					Log::warn("(WalkerCortex::StepAction) Panning failed: no candidates.");
					this->state = state_failure;
					break;
				}

				this->pending_tile_coordinate = candidates[RNG::next(candidates.size())];
				this->current_result = world->hover(this->pending_tile_coordinate);
				this->state = state_move;
			}
			break;
		case state_click:
			{
				auto current_frame = this->cortex->get_autonomaus().get_game_state().get_last_update_frame();

				if (current_frame > this->click_frame)
				{
					auto world_click_frame = world->get_last_click_frame();
					if (world_click_frame >= this->click_frame)
					{
						if (world->get_last_click_type() != WorldCortex::click_walk)
						{
							Log::info("(WalkerCortex::StepAction) Woops, poked something!");
							this->state = state_pan;
							break;
						}
					}

					this->state = state_finish;
				}
			}
			break;
		case state_minimap_click:
			if (!this->current_result.pending())
			{
				this->current_result = view->focus_y(this->pending_tile_coordinate);
				this->state = state_finish;
			}
			break;
		case state_finish:
			{
				auto current_tile_coordinate = view->get_target_tile_coordinate();
				auto distance = glm::vec2(
					current_tile_coordinate.x - this->pending_tile_coordinate.x,
					current_tile_coordinate.y - this->pending_tile_coordinate.y);

				float divisor;
				{
					auto minimap_panel = this->cortex->get_autonomaus().get_userdata<MinimapPanelCortex>();
					if (minimap_panel != nullptr)
					{
						if (minimap_panel->is_running())
						{
							divisor = 2.0f;
						}
						else
						{
							divisor = 1.5f;
						}
					}
					else
					{
						Log::warn("(WalkerCortex::StepAction) Minimap panel cortex not valid. Bad logic!");
						divisor = 1.0f;
					}
				}

				prepare_for_wait(glm::length(distance) / divisor, state_success);
			}
			break;
		case state_wait:
			if (!this->current_result.pending())
			{
				this->state = this->next_state;
				this->next_state = state_failure;
			}
			break;
		case state_wait_stop:
			if (!this->current_result.pending())
			{
				auto current_frame = this->cortex->get_autonomaus().get_game_state().get_last_update_frame();
				if (current_frame > this->last_frame)
				{
					this->last_frame = current_frame;

					bool stopped = false;
					auto current_tile = view->get_target_tile_coordinate();
					auto current_position = view->get_target_position();
					if (current_position == this->target_previous_world)
					{
						auto& minimap = this->cortex->get_autonomaus().get_map_state()->get_minimap();
						if (!minimap.get_is_visible() || minimap.features_begin("flag") == minimap.features_end("flag"))
						{
							stopped = true;
						}
					}
					else
					{
						if (current_tile != this->target_previous_tile)
						{
							++this->distance;

							if (this->distance >= this->desired_distance)
							{
								stopped = true;
							}
						}
					}

					this->target_previous_tile = current_tile;
					this->target_previous_world = current_position;

					if (stopped)
					{
						this->state = this->next_state;
						this->next_state = state_failure;
					}
				}
			}
			break;
		case state_wait_poke:
			if (world->get_last_click_frame() > this->last_frame &&
				world->get_last_click_type() != WorldCortex::click_poke)
			{
				this->state = poke_failure;
			}
			else
			{
				this->state = poke_success;
			}

			this->poke_success = state_failure;
			this->poke_failure = state_failure;
			break;
		case state_failure:
			*this->status = Result::Status::failure;
			this->state = state_done;
			break;
		case state_success:
			if (!this->current_result.pending())
			{
				*this->status = this->current_result.get();
				this->state = state_done;
			}
			break;
		case state_done:
			// Nothing.
			break;
		default:
			Log::warn("(WalkerCortex::StepAction) Unhandled state! Value: '{}'.", this->state);
			cancel();
			break;
	}
}

void autonomaus::WalkerCortex::StepAction::click()
{
	MotionSequence sequence(*this->cortex);
	sequence.left_click();

	this->current_result = this->cortex->perform(sequence);

	this->state = state_click;
	this->click_frame = this->cortex->get_autonomaus().get_current_frame() + 1;
}

void autonomaus::WalkerCortex::StepAction::prepare_for_wait(int estimated_distance, int state)
{
	auto view = this->cortex->get_autonomaus().get_userdata<ViewCortex>();
	if (view == nullptr)
	{
		this->next_state = state_failure;
		return;
	}

	this->state = state_wait_stop;
	this->next_state = state;
	this->distance = 0;
	this->desired_distance = std::max(estimated_distance, 1);
	this->target_previous_tile = view->get_target_tile_coordinate();
	this->target_previous_world = view->get_target_position();
	this->last_frame = this->cortex->get_autonomaus().get_game_state().get_last_update_frame();
}

void autonomaus::WalkerCortex::StepAction::wait_for_poke(int success_state, int failure_state)
{
	this->last_frame = this->cortex->get_autonomaus().get_current_frame() + 1;

	prepare_for_wait(1, state_wait_poke);
	this->poke_success = success_state;
	this->poke_failure = failure_state;
}
