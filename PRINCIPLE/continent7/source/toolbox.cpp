// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "continent7/application.hpp"
#include "principle/resources/draw.hpp"
#include "kvlc/collision/query.hpp"
#include "thoreau/thoreau.hpp"

continent7::Application::Toolbox::Toolbox(Application* application)
{
	this->application = application;
}

void continent7::Application::Toolbox::neighbor_elevations_from_ray(
	const thoreau::TileCoordinate& tile_coordinate,
	float elevation_hint, float neighbor_offset,
	bool make_passable)
{
	std::set<thoreau::TileCoordinate> visited;
	do_neighbor_elevations_from_ray(
		tile_coordinate,
		elevation_hint, neighbor_offset,
		make_passable, 0,
		visited);
}

void continent7::Application::Toolbox::elevation_from_ray(
	const thoreau::TileCoordinate& tile_coordinate,
	float elevation_hint, bool make_passable)
{
	int tag;
	float elevation = test_elevation_from_ray(tile_coordinate, elevation_hint, tag);
	if (elevation != HUGE_VALF)
	{
		set_elevation(tile_coordinate, elevation);
		if (make_passable)
		{
			mark_passable(tile_coordinate);
		}
	}
}

void continent7::Application::Toolbox::mark_impassable(
	const thoreau::TileCoordinate& tile_coordinate)
{
	auto& map = this->application->get_game_manager().get_runtime_map();
	if (map.has_tile(tile_coordinate))
	{
		auto& tile = map.get_tile(tile_coordinate);
		if (tile.get_flags() & thoreau::Tile::flag_passable)
		{
			tile.unset_flags(thoreau::Tile::flag_passable);
		}
		tile.append_flags(thoreau::Tile::flag_impassable);

		mark_dirty(tile_coordinate);
	}
}

void continent7::Application::Toolbox::mark_passable(
	const thoreau::TileCoordinate& tile_coordinate)
{
	auto& map = this->application->get_game_manager().get_runtime_map();
	if (map.has_tile(tile_coordinate))
	{
		auto& tile = map.get_tile(tile_coordinate);
		tile.append_flags(thoreau::Tile::flag_passable);

		mark_dirty(tile_coordinate);
	}
}

void continent7::Application::Toolbox::undo()
{
	if (!this->undo_stack.empty())
	{
		auto previous_map = this->undo_stack.top();
		this->undo_stack.pop();

		this->application->get_game_manager().get_runtime_map() = previous_map;
		for (auto i: this->application->visible_sectors)
		{
			this->application->dirty_sectors.insert(i.first);
		}
		this->application->notify_map_update();
	}
}

void continent7::Application::Toolbox::clear_undo_stack()
{
	this->undo_stack = std::stack<thoreau::Map>();
}

std::size_t continent7::Application::Toolbox::get_undo_stack_size() const
{
	return this->undo_stack.size();
}

void continent7::Application::Toolbox::push_undo_stack()
{
	this->undo_stack.push(this->application->get_game_manager().get_runtime_map());
}

float continent7::Application::Toolbox::test_elevation_from_ray(
	const thoreau::TileCoordinate& tile_coordinate,
	float elevation_hint, int& result_tag)
{
	auto& scene = this->application->map_collision_scene;

	glm::vec3 world_position;
	world_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	world_position.y = elevation_hint;
	world_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

	kvlc::Ray ray;
	ray.position = world_position;
	ray.direction = glm::vec3(0.0f, -1.0f, 0.0f);

	std::map<int, glm::vec3> hits;
	auto length = this->application->bounds.compute_size().y;
	
	kvlc::Query query;
	if (!scene.test(ray, length, query))
	{
		return HUGE_VALF;
	}

	float max_elevation = -HUGE_VALF;
	int draw_type = continent1::DrawSnapshot::type_none;
	int tag = 0;
	for (auto& result: query)
	{
		if (result.second.y > max_elevation)
		{
			max_elevation = result.second.y;
			draw_type = this->application->map_collision_scene_types[result.first];
			tag = result.first;
		}
	}

	if (draw_type != principle::Draw::draw_type_scenery &&
		draw_type != principle::Draw::draw_type_terrain)
	{
		return HUGE_VALF;
	}

	result_tag = tag;
	return max_elevation;
}

void continent7::Application::Toolbox::do_neighbor_elevations_from_ray(
	const thoreau::TileCoordinate& tile_coordinate,
	float elevation_hint, float neighbor_offset,
	bool make_passable, int tag,
	std::set<thoreau::TileCoordinate>& visited)
{
	int result_tag;
	float elevation = test_elevation_from_ray(tile_coordinate, elevation_hint + neighbor_offset, result_tag);
	if (elevation == HUGE_VALF)
	{
		return;
	}

	if (tag == 0)
	{
		tag = result_tag;
	}
	else if (tag != result_tag && this->application->restrain_tag)
	{
		return;
	}

	float difference = std::fabs(elevation - elevation_hint);
	if (difference > neighbor_offset)
	{
		return;
	}

	visited.insert(tile_coordinate);
	set_elevation(tile_coordinate, elevation);
	if (make_passable)
	{
		mark_passable(tile_coordinate);
	}

	for (int y = -1; y <= 1; ++y)
	{
		for (int x = -1; x <= 1; ++x)
		{
			if (x == 0 && y == 0)
			{
				continue;
			}

			auto neighbor_tile_coordinate = thoreau::TileCoordinate(
				tile_coordinate.x + x, tile_coordinate.y + y,
				tile_coordinate.layer);
			if (visited.count(neighbor_tile_coordinate) == 0)
			{
				do_neighbor_elevations_from_ray(
					neighbor_tile_coordinate,
					elevation, neighbor_offset,
					make_passable, tag,
					visited);
			}
		}
	}
}

void continent7::Application::Toolbox::set_elevation(
	const thoreau::TileCoordinate& tile_coordinate,
	float value)
{
	float half_range = this->application->tile_elevation_range / 2.0f;
	auto& map = this->application->get_game_manager().get_runtime_map();

	auto t = tile_coordinate;
	if (this->application->override_layer)
	{
		t.layer = this->application->current_layer;
	}

	auto& tile = map.add_tile(t);
	tile.set_elevation(value - half_range, value + half_range);

	mark_dirty(tile_coordinate);
}

void continent7::Application::Toolbox::mark_dirty(
	const thoreau::TileCoordinate& tile_coordinate)
{
	auto sector_coordinate = thoreau::sector_from_tile(tile_coordinate);
	this->application->dirty_sectors.insert(sector_coordinate);
	this->application->modified_tiles.insert(tile_coordinate);
}
