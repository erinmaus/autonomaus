// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/log.hpp"
#include "autonomaus/core/config.hpp"
#include "autonomaus/state/camera.hpp"
#include "autonomaus/state/gui.hpp"
#include "autonomaus/state/view.hpp"
#include "autonomaus/state/world.hpp"
#include "thoreau/thoreau.hpp"

const float autonomaus::View::SAFE_VIEWPORT_MARGIN =
	Config::get<float>("common", "view", "SAFE_VIEWPORT_MARGIN");

autonomaus::View::View(const World& world, const Map& map)
{
	this->world = &world;
	this->map = &map.get_map();
	this->minimap = &map.get_minimap();

	dInitODE();
	this->scene_world = dWorldCreate();
	dWorldSetAutoDisableFlag(this->scene_world, false);
	this->scene_space = dHashSpaceCreate(0);
	dHashSpaceSetLevels(this->scene_space, 8, 12);
}

autonomaus::View::~View()
{
	clear(TILE_TAG);
	clear(ACTOR_TAG);

	dSpaceDestroy(this->scene_space);
	dWorldDestroy(this->scene_world);
}

bool autonomaus::View::occluded(const kvlc::Ray& ray, float length) const
{
	populate_scene();

	dGeomID geometry = dCreateRay(this->scene_space, length);
	dGeomRaySet(geometry,
		ray.position.x, ray.position.y, ray.position.z,
		ray.direction.x, ray.direction.y, ray.direction.z);

	int count = 0;
	dSpaceCollide2((dGeomID)this->scene_space, geometry, &count, &perform_space_collision);

	dGeomDestroy(geometry);

	return count > 0;
}

bool autonomaus::View::occluded(const thoreau::TileCoordinate& tile_coordinate) const
{
	if (!this->map->has_tile(tile_coordinate))
	{
		return true;
	}

	auto tile = this->map->get_tile(tile_coordinate);
	glm::vec3 tile_position;
	tile_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	tile_position.y = tile.get_elevation();
	tile_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

	auto screen_position = this->camera.world_to_screen(tile_position);
	screen_position.z = 0.0f;

	glm::vec3 a = tile_position;
	glm::vec3 b = this->camera.screen_to_world(screen_position);

	kvlc::Ray ray;
	ray.position = b;
	ray.direction = glm::normalize(a - b);
	float length = glm::length(a - b);

	return occluded(ray, length);
}

bool autonomaus::View::is_in_bounds(const thoreau::TileCoordinate& tile_coordinate) const
{
	if (!this->map->has_tile(tile_coordinate))
	{
		return false;
	}

	auto tile = this->map->get_tile(tile_coordinate);
	glm::vec3 tile_position;
	tile_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	tile_position.y = tile.get_elevation();
	tile_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

	auto screen_position = this->camera.world_to_screen(tile_position);
	auto viewport = this->camera.get_viewport();
	viewport.x += SAFE_VIEWPORT_MARGIN * viewport.z / 2.0f;
	viewport.z *= 1.0f - (SAFE_VIEWPORT_MARGIN / 2.0f);
	viewport.y += SAFE_VIEWPORT_MARGIN * viewport.w / 2.0f;
	viewport.w *= 1.0f - (SAFE_VIEWPORT_MARGIN / 2.0f);

	if (screen_position.x < viewport.x ||
		screen_position.x > viewport.x + viewport.z ||
		screen_position.y < viewport.y ||
		screen_position.y > viewport.y + viewport.w ||
		screen_position.z < 0.0f || screen_position.z > 1.0f)
	{
		return false;
	}

	return true;
}

void autonomaus::View::update(const Camera& camera)
{
	this->camera = camera;
	{
		auto rotation = glm::mat4(glm::mat3(camera.get_view()));
		auto inverse_rotation = glm::inverse(rotation);
		auto translation = inverse_rotation * camera.get_view();
		this->camera_position = -glm::vec3(translation[3]);
	}

	bool player_moved = false;
	if (this->minimap->get_is_visible())
	{
		this->confidence = HUGE_VALF;
		this->y_rotation = this->minimap->get_rotation();
		this->player_tile_coordinate = this->minimap->get_center_tile();

		auto sector_coordinate = thoreau::sector_from_tile(this->player_tile_coordinate);
		if (sector_coordinate != this->player_sector_coordinate)
		{
			player_moved = true;
			this->player_sector_coordinate = sector_coordinate;
		}

		float best_y = -HUGE_VALF;
		float best_distance = HUGE_VALF;
		for (auto& i: this->world->get_animated_state())
		{
			auto key = i.first;
			auto& actor = i.second;
			glm::vec3 actor_position;
			{
				auto origin = glm::vec4(glm::vec3(0), 1);
				auto transformed_origin = actor.model_matrix * origin;
				actor_position = glm::vec3(transformed_origin);
			}

			glm::vec2 position;
			position.x = actor_position.x / thoreau::TILE_SIZE;
			position.y = actor_position.z / thoreau::TILE_SIZE;

			auto distance = glm::length(this->minimap->get_center() - position);
			if (distance < best_distance && distance <= 2.0f)
			{
				if (actor_position.y > best_y)
				{
					this->player_key = key;
					this->player_position = actor_position;
					best_distance = distance;
					best_y = actor_position.y;
				}
			}

			this->map->try_get_tile_layer(
				this->player_tile_coordinate.x, this->player_tile_coordinate.y,
				this->player_position.y + 512.0f, this->player_tile_coordinate.layer);
		}

		if (best_distance != HUGE_VALF)
		{
			auto projected_camera_position = this->camera_position;
			projected_camera_position.y = this->player_position.y;

			auto a = this->camera_position - this->player_position;
			auto a_length = glm::length(a);
			auto b = this->player_position - projected_camera_position;
			auto b_length = glm::length(b);

			auto dot = glm::dot(a, b) / (a_length * b_length);
			this->x_rotation = std::acos(dot);
		}
	}
	else
	{
		kvlc::Ray ray;
		{
			auto origin = glm::vec4(0, 0, 0, 1);
			origin = camera.get_view() * origin;
			auto forward = glm::vec4(0, 0, -1, 0);
			forward = camera.get_view() * forward;
			ray.position = glm::vec3(origin);
			ray.direction = glm::normalize(glm::vec3(forward));
		}

		static bool warned = false;
		if (!warned)
		{
			Log::critical("View state when minimap isn't visible not yet implemented");
			warned = true;
		}

		this->confidence = -HUGE_VALF;
		this->player_position = glm::vec3(0.0f);
		this->player_tile_coordinate = thoreau::TileCoordinate(0, 0);
		this->player_key = thoreau::WorldCoordinate(0, 0);
		this->x_rotation = 0.0f;
		this->y_rotation = 0.0f;
	}

	if (player_moved)
	{
		clear(TILE_TAG);
		this->tiles_dirty = true;
	}

	clear(ACTOR_TAG);
	this->actors_dirty = true;
}

const glm::vec3& autonomaus::View::get_player_position() const
{
	return this->player_position;
}

const thoreau::TileCoordinate& autonomaus::View::get_player_tile_coordinate() const
{
	return this->player_tile_coordinate;
}

float autonomaus::View::get_x_rotation() const
{
	return this->x_rotation;
}

float autonomaus::View::get_y_rotation() const
{
	return this->y_rotation;
}

float autonomaus::View::get_z_rotation() const
{
	return 0.0f; // . . . Gotta implement them all!
}

float autonomaus::View::get_confidence() const
{
	return this->confidence;
}

float autonomaus::View::get_y_rotation_difference(
	const thoreau::TileCoordinate& tile_coordinate) const
{
	return get_y_rotation_difference(tile_coordinate, this->player_tile_coordinate);
}

float autonomaus::View::get_y_rotation_difference(
	const thoreau::TileCoordinate& tile_coordinate,
	const thoreau::TileCoordinate& reference) const
{
	glm::vec3 world_position;
	world_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	world_position.y = 0.0f;
	world_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

	glm::vec3 reference_position;
	reference_position.x = (reference.x + 0.5f) * thoreau::TILE_SIZE;
	reference_position.y = 0.0f;
	reference_position.z = (reference.y + 0.5f) * thoreau::TILE_SIZE;

	return get_y_rotation_difference(world_position, reference_position);
}

float autonomaus::View::get_y_rotation_difference(const glm::vec3& position) const
{
	return get_y_rotation_difference(position, this->player_position);
}

float autonomaus::View::get_y_rotation_difference(
	const glm::vec3& position,
	const glm::vec3& reference) const
{
	auto camera_difference = reference - this->camera_position;
	camera_difference.y = 0.0f;
	auto camera_direction = glm::normalize(camera_difference);

	auto target_difference = (position - reference);
	target_difference.y = 0.0f;
	auto target_direction = glm::normalize(target_difference);

	if (glm::length(target_difference) < 1.0f)
	{
		return 0.0f;
	}

	auto angle = std::acos(glm::dot(target_direction, camera_direction));

	auto i = (position.x - this->camera_position.x) *
		(reference.z - this->camera_position.z);
	auto j = (position.z - this->camera_position.z) *
		(reference.x - this->camera_position.x);
	auto k = i - j;

	if (k < 0.0f)
	{
		return -angle;
	}
	else
	{
		return angle;
	}
}

glm::ivec4 autonomaus::View::get_safe_viewport(
	const chomsky::GUIContent& content) const
{
	static bool warned = false;
	if (!warned)
	{
		Log::critical("Safe viewport not yet implemented. Returning world viewport.");
		warned = true;
	}

	return camera.get_viewport();
}

void autonomaus::View::populate_scene() const
{
	if (this->tiles_dirty)
	{
		for (int i = -1; i <= 1; ++i)
		{
			for (int j = -1; j <= 1; ++j)
			{
				thoreau::SectorCoordinate sector_coordinate = this->player_sector_coordinate;
				sector_coordinate.x += i;
				sector_coordinate.y += j;
				add_sector(sector_coordinate);
			}
		}

		this->tiles_dirty = false;
	}

	if (this->actors_dirty)
	{
		for (auto& i: this->world->get_animated_state())
		{
			auto key = i.first;
			auto& actor = i.second;

			if (key != this->player_key)
			{
				add_actor(actor.bounds, actor.model_matrix);
			}
		}

		this->actors_dirty = false;
	}
}

void autonomaus::View::add_sector(const thoreau::SectorCoordinate& sector) const
{
	auto current = this->map->sector_begin(sector);
	auto end = this->map->sector_end(sector);

	while (current != end)
	{
		if (current->layer == this->player_tile_coordinate.layer)
		{
			auto& tile = this->map->get_tile(*current);
			if (tile.is_impassable())
			{
				add_tile(*current, tile);
			}
		}

		++current;
	}
}

void autonomaus::View::add_actor(const kvlc::Bounds& bounds, const glm::mat4& transform) const
{
	glm::vec3 box_size = bounds.compute_size() * 1.5f;
	dMatrix3 box_rotation;
	{
		auto input = glm::mat3(transform);

		int current = 0;
		for (int i = 0; i < 3; ++i)
		{
			for (int j = 0; j < 3; ++j)
			{
				box_rotation[current++] = input[i][j];
			}

			++current;
		}
	}
	glm::vec3 box_position;
	{
		auto transformed_origin = transform * glm::vec4(glm::vec3(0.0f), 1.0f);
		box_position = glm::vec3(transformed_origin);
		box_position.y += box_size.y / 2.0f;
	}


	Obstacle obstacle;
	obstacle.tag = ACTOR_TAG;
	obstacle.shape = dCreateBox(this->scene_space, box_size.x, box_size.y, box_size.z);
	obstacle.body = dBodyCreate(this->scene_world);
	dGeomSetBody(obstacle.shape, obstacle.body);
	dBodySetPosition(obstacle.body, box_position.x, box_position.y, box_position.z);
	dBodySetRotation(obstacle.body, box_rotation);

	this->obstacles.push_back(obstacle);
}

void autonomaus::View::add_tile(
	const thoreau::TileCoordinate& tile_coordinate,
	const thoreau::Tile& tile) const
{
	static const float DEFAULT_HEIGHT = 1024.0f;

	glm::vec3 box_position;
	box_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	box_position.y = tile.get_elevation();
	box_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

	glm::vec3 box_size(0.0f);
	box_size.x = thoreau::TILE_SIZE * 1.5f;
	box_size.z = thoreau::TILE_SIZE * 1.5f;

	if (tile.get_height() == 0)
	{
		box_size.y = DEFAULT_HEIGHT;
		box_position.y += DEFAULT_HEIGHT / 2.0f;
	}
	else if (tile.get_flags() & thoreau::Tile::flag_impassable)
	{
		box_size.y = tile.get_height();
		box_position.y += tile.get_height() / 2.0f;
	}

	Obstacle obstacle;
	obstacle.tag = TILE_TAG;
	obstacle.shape = dCreateBox(this->scene_space, box_size.x, box_size.y, box_size.z);
	obstacle.body = dBodyCreate(this->scene_world);

	dGeomSetBody(obstacle.shape, obstacle.body);
	dBodySetPosition(obstacle.body, box_position.x, box_position.y, box_position.z);

	this->obstacles.push_back(obstacle);
}

void autonomaus::View::clear(int tag)
{
	for (auto i = this->obstacles.begin(); i != this->obstacles.end(); /* Nop. */)
	{
		if (i->tag == tag)
		{
			dGeomDestroy(i->shape);
			dBodyDestroy(i->body);

			i = this->obstacles.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void autonomaus::View::perform_space_collision(void* data, dGeomID a, dGeomID b)
{
	int* count = (int*)data;
	dContact contact;

	if (dCollide(a, b, 1, &contact.geom, sizeof(dContact)))
	{
		++(*count);
	}
}
