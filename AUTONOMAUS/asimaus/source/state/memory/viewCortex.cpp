// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include <limits>
#include <glm/gtc/matrix_transform.hpp>
#include "autonomaus/core/config.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/core/rng.hpp"
#include "autonomaus/core/time.hpp"
#include "autonomaus/graphics/drawState.hpp"
#include "autonomaus/input/inputManager.hpp"
#include "autonomaus/state/camera.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/guiProvider.hpp"
#include "autonomaus/state/mapProvider.hpp"
#include "autonomaus/state/npcProvider.hpp"
#include "autonomaus/state/worldProvider.hpp"
#include "autonomaus/state/memory/motionSequence.hpp"
#include "autonomaus/state/memory/viewCortex.hpp"
#include "thoreau/thoreau.hpp"

const float autonomaus::ViewCortex::SAFE_VIEWPORT_MARGIN =
	Config::get<float>("game", "ViewCortex", "SAFE_VIEWPORT_MARGIN");

struct autonomaus::ViewCortex::FocusAction : public Action
{
	FocusAction(ViewCortex* cortex, int num_attempts, float range);
	~FocusAction();

	static const float MIN_LENGTH;
	static const float MAX_LENGTH;
	static const float DEVIATION;

	// Must be (1, 0) or (0, 1).
	virtual glm::vec2 get_normal() = 0;
	virtual float get_angle() = 0;

	Cortex::Result get_result() override;
	void update() override;

	void cancel();

	ViewCortex* cortex;
	int num_attempts;
	float range;

	enum
	{
		state_start,
		state_drag,
		state_done
	};
	int state = state_start;
	float sign = 0.0f;
	int current_attempt = 0;

	Result current_result;

	Result::StatusReference status;
	Result final_result;
};

const float autonomaus::ViewCortex::FocusAction::MIN_LENGTH =
	Config::get<float>("game", "ViewCortex", "MIN_FOCUS_ACTION_LENGTH");
const float autonomaus::ViewCortex::FocusAction::MAX_LENGTH =
	Config::get<float>("game", "ViewCortex", "MAX_FOCUS_ACTION_LENGTH");
const float autonomaus::ViewCortex::FocusAction::DEVIATION =
	Config::get<float>("game", "ViewCortex", "FOCUS_ACTION_DEVIATION");

struct autonomaus::ViewCortex::FocusYAction : public FocusAction
{
	FocusYAction(
		ViewCortex* cortex,
		int num_attempts,
		float range,
		float offset,
		const glm::vec3& target_position,
		const glm::vec3& reference_position) :
			FocusAction(cortex, num_attempts, range)
	{
		this->offset = offset;
		this->target_position = target_position;
		this->reference_position = reference_position;
	}
	~FocusYAction() = default;

	glm::vec2 get_normal() override { return glm::vec2(1.0f, 0.0f); }
	float get_angle() override
	{
		float current = this->cortex->get_y_rotation_difference(
			this->target_position,
			this->reference_position);
		float sum = current + this->offset;
		if (sum < -M_PI)
		{
			sum += M_PI * 2.0f;
		}
		else if (sum > M_PI)
		{
			sum -= M_PI * 2.0f;
		}

		return sum;
	}

	float offset;
	glm::vec3 target_position;
	glm::vec3 reference_position;
};

struct autonomaus::ViewCortex::FocusXAction : public FocusAction
{
	FocusXAction(
		ViewCortex* cortex,
		int num_attempts,
		float range,
		float desired_angle) :
			FocusAction(cortex, num_attempts, range)
	{
		this->desired_angle = desired_angle;
	}
	~FocusXAction() = default;

	glm::vec2 get_normal() override { return glm::vec2(0.0f, 1.0f); }
	float get_angle() override
	{
		float current = this->cortex->get_x_rotation();
		float difference = this->desired_angle - current;
		if (difference < -M_PI)
		{
			difference += M_PI * 2.0f;
		}
		else if (difference > M_PI)
		{
			difference -= M_PI * 2.0f;
		}

		return -difference;
	}

	float desired_angle;
};

autonomaus::ViewCortex::ViewCortex(Autonomaus& autonomaus) :
	Cortex("Game::View", autonomaus)
{
	this->world = &autonomaus.get_world_state().get();
	this->map = &autonomaus.get_map_state()->get_map();
	this->minimap = &autonomaus.get_map_state()->get_minimap();

	dInitODE();
	this->scene_world = dWorldCreate();
	dWorldSetAutoDisableFlag(this->scene_world, false);
	this->scene_space = dHashSpaceCreate(0);
	dHashSpaceSetLevels(this->scene_space, 8, 12);

	load_animations();

	on_pull(
		"is_tile_occluded",
		[this](auto& message)
		{
			int tile_x = message.get_integer("x");
			int tile_y = message.get_integer("y");
			int tile_layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(tile_x, tile_y, tile_layer);

			Message result;
			result.set_boolean("result", this->possibly_occluded(tile_coordinate));
			return result;
		});

	on_pull(
		"is_position_occluded",
		[this](auto& message)
		{
			float x = message.get_integer("x");
			float y = message.get_integer("y");
			float z = message.get_integer("z");
			auto position = glm::vec3(x, y, z);

			Message result;
			result.set_boolean("result", this->possibly_occluded(position));
			return result;
		});

	on_pull(
		"is_tile_in_bounds",
		[this](auto& message)
		{
			int tile_x = message.get_integer("x");
			int tile_y = message.get_integer("y");
			int tile_layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(tile_x, tile_y, tile_layer);

			Message result;
			result.set_boolean("result", this->is_in_bounds(tile_coordinate));
			return result;
		});

	on_pull(
		"is_position_in_bounds",
		[this](auto& message)
		{
			float x = message.get_integer("x");
			float y = message.get_integer("y");
			float z = message.get_integer("z");
			auto position = glm::vec3(x, y, z);

			Message result;
			result.set_boolean("result", this->is_in_bounds(position));
			return result;
		});

	on_poke(
		"reset_target_idle_time",
		[this](auto&)
		{
			return this->reset_target_idle_time();
		});

	on_pull(
		"target_idle_time",
		[this](auto&)
		{
			Message result;
			result.set_single("result", this->get_target_idle_time());
			return result;
		});

	on_pull(
		"target_position",
		[this](auto&)
		{
			auto position = this->get_target_position();

			Message result;
			result.set_single("x", position.x);
			result.set_single("y", position.y);
			result.set_single("z", position.z);
			return result;
		});

	on_pull(
		"target_tile",
		[this](auto&)
		{
			auto position = this->get_target_tile_coordinate();

			Message result;
			result.set_integer("x", position.x);
			result.set_integer("y", position.y);
			result.set_integer("layer", position.layer);
			return result;
		});

	on_pull(
		"target_animations",
		[this](auto&)
		{
			Message result;
			std::size_t i = 0;
			for (auto& animation: this->player_animations)
			{
				result.set_string("result", animation, i);
			}
			return result;
		});

	on_pull(
		"tile_under_mouse",
		[this](auto&)
		{
			auto position = this->get_tile_under_mouse();

			Message result;
			result.set_integer("x", position.x);
			result.set_integer("y", position.y);
			result.set_integer("layer", position.layer);
			return result;
		});

	on_pull(
		"x_rotation",
		[this](auto&)
		{
			auto position = glm::degrees(this->get_x_rotation());

			Message result;
			result.set_single("result", position);
			return result;
		});

	on_pull(
		"y_rotation",
		[this](auto&)
		{
			auto position = glm::degrees(this->get_y_rotation());

			Message result;
			result.set_single("result", position);
			return result;
		});

	on_pull(
		"z_rotation",
		[this](auto&)
		{
			auto position = glm::degrees(this->get_z_rotation());

			Message result;
			result.set_single("result", position);
			return result;
		});

	on_pull(
		"focus_tile",
		[this](auto& message)
		{
			int tile_x = message.get_integer("x");
			int tile_y = message.get_integer("y");
			int tile_layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(tile_x, tile_y, tile_layer);

			Message result;
			result.set_single("result", glm::degrees(this->get_y_rotation_difference(tile_coordinate)));
			return result;
		});

	on_poke(
		"focus_tile",
		[this](auto& message)
		{
			int tile_x = message.get_integer("x");
			int tile_y = message.get_integer("y");
			int tile_layer = message.get_integer("layer");
			auto tile_coordinate = thoreau::tile_from_input(tile_x, tile_y, tile_layer);

			float offset = 0.0f;
			if (message.has("offset"))
			{
				offset = message.get_single("offset");
			}

			float range = M_PI / 4.0f;
			if (message.has("range"))
			{
				range = glm::radians(message.get_single("range"));
			}

			int num_attempts = 1;
			if (message.has("num_attempts"))
			{
				num_attempts = message.get_integer("num_attempts");
			}

			return this->focus_y(tile_coordinate, offset, range, num_attempts);
		});

	on_pull(
		"focus_position",
		[this](auto& message)
		{
			float x = message.get_integer("x");
			float y = message.get_integer("y");
			float z = message.get_integer("z");
			auto position = glm::vec3(x, y, z);

			Message result;
			result.set_single("result", glm::degrees(this->get_y_rotation_difference(position)));
			return result;
		});

	on_poke(
		"focus_position",
		[this](auto& message)
		{
			float x = message.get_integer("x");
			float y = message.get_integer("y");
			float z = message.get_integer("z");
			auto position = glm::vec3(x, y, z);

			float offset = 0.0f;
			if (message.has("offset"))
			{
				offset = message.get_single("offset");
			}

			float range = M_PI / 4.0f;
			if (message.has("range"))
			{
				range = glm::radians(message.get_single("range"));
			}

			int num_attempts = 1;
			if (message.has("num_attempts"))
			{
				num_attempts = message.get_integer("num_attempts");
			}

			return this->focus_y(position, offset, range, num_attempts);
		});

	on_poke(
		"adjust_pitch",
		[this](auto& message)
		{
			float angle = glm::radians(message.get_single("angle"));

			float range = M_PI / 8.0f;
			if (message.has("range"))
			{
				range = glm::radians(message.get_single("range"));
			}

			int num_attempts = 1;
			if (message.has("num_attempts"))
			{
				num_attempts = message.get_integer("num_attempts");
			}

			return this->focus_x(angle, range, num_attempts);
		});

	NPCProvider::add_profile(
		get_autonomaus().get_game_state(),
		get_autonomaus().get_npc_state(),
		"player");
}

autonomaus::ViewCortex::~ViewCortex()
{
	clear(ACTOR_TAG);
	clear(TILE_TAG);
	clear(HIT_TAG);

	dSpaceDestroy(this->scene_space);
	dWorldDestroy(this->scene_world);

	NPCProvider::remove_profile(
		get_autonomaus().get_game_state(),
		get_autonomaus().get_npc_state(),
		"player");
}

bool autonomaus::ViewCortex::possibly_occluded(const kvlc::Ray& ray, float length) const
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

bool autonomaus::ViewCortex::possibly_occluded(const thoreau::TileCoordinate& tile_coordinate) const
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

	return possibly_occluded(tile_position);
}

bool autonomaus::ViewCortex::possibly_occluded(const glm::vec3& position) const
{
	auto screen_position = this->camera.world_to_screen(position);
	screen_position.z = 0.0f;

	glm::vec3 a = position;
	glm::vec3 b = this->camera.screen_to_world(screen_position);

	kvlc::Ray ray;
	ray.position = b;
	ray.direction = glm::normalize(a - b);
	float length = glm::length(a - b);

	return possibly_occluded(ray, length);
}

bool autonomaus::ViewCortex::is_in_bounds(const thoreau::TileCoordinate& tile_coordinate) const
{
	if (!this->map->has_tile(tile_coordinate))
	{
		return false;
	}

	auto tile = this->map->get_tile(tile_coordinate);
	glm::vec3 tile_position;
	tile_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	tile_position.y = tile.get_elevation() + thoreau::TILE_SIZE;
	tile_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

	return is_in_bounds(tile_position);
}

bool autonomaus::ViewCortex::is_in_bounds(const glm::vec3& position) const
{
	auto screen_position = this->camera.world_to_screen(position);
	auto viewport = this->camera.get_viewport();

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

autonomaus::Cortex::Result
autonomaus::ViewCortex::reset_target_idle_time()
{
	this->last_player_movement = get_current_time();
	return Result::Status::success;
}

float autonomaus::ViewCortex::get_target_idle_time() const
{
	if (this->last_player_movement == HUGE_VALF)
	{
		return 0.0f;
	}

	return get_current_time() - this->last_player_movement;
}

const glm::vec3& autonomaus::ViewCortex::get_target_position() const
{
	return this->player_position;
}

const thoreau::TileCoordinate& autonomaus::ViewCortex::get_target_tile_coordinate() const
{
	return this->player_tile_coordinate;
}

std::unordered_set<std::string> autonomaus::ViewCortex::get_target_animations() const
{
	return this->player_animations;
}

thoreau::TileCoordinate autonomaus::ViewCortex::get_tile_under_point(int x, int y) const
{
	populate_scene();

	// Improves accuracy.
	//auto v = glm::scale(glm::mat4(1.0f), glm::vec3(1 / 512.0f)) * this->camera.get_view();
	auto v = this->camera.get_view();
	glm::vec3 a = glm::unProject(glm::vec3(x, y, 0.0f), v, this->camera.get_projection(), this->camera.get_viewport());
	glm::vec3 b = glm::unProject(glm::vec3(x, y, 0.1f), v, this->camera.get_projection(), this->camera.get_viewport());
	glm::vec3 c = glm::unProject(glm::vec3(x, y, 1.0f), v, this->camera.get_projection(), this->camera.get_viewport());

	kvlc::Ray ray;
	ray.position = a;
	ray.direction = glm::normalize(b - a);
	float length = glm::length(c - a);

	dGeomID geometry = dCreateRay(this->scene_space, length);
	dGeomRaySet(geometry,
		ray.position.x, ray.position.y, ray.position.z,
		ray.direction.x, ray.direction.y, ray.direction.z);
	dGeomRaySetClosestHit(geometry, true);

	std::vector<glm::vec3> results;
	dSpaceCollide2(
		(dGeomID)this->scene_space, geometry,
		&results, &get_tiles);
	dGeomDestroy(geometry);

	if (results.empty())
	{
		return thoreau::TileCoordinate(0, 0, 0);
	}

	glm::vec3 best;
	float best_distance = HUGE_VALF;
	for (auto i: results)
	{
		auto v = this->camera.world_to_screen(i);
		if (v.z < best_distance)
		{
			best = i;
			best_distance = v.z;
		}
	}

	auto tile_coordinate = thoreau::tile_from_world(best);
	auto& map = get_autonomaus().get_map_state()->get_map();
	map.try_get_tile_layer(tile_coordinate.x, tile_coordinate.y, best.y, tile_coordinate.layer);

	return tile_coordinate;
}

thoreau::TileCoordinate autonomaus::ViewCortex::get_tile_under_mouse() const
{
	auto& input_state = get_autonomaus().get_input_manager().get_state();
	int x = input_state.get_mouse_x();
	int y = input_state.get_mouse_y();
	y = this->camera.get_viewport().w - y;

	return get_tile_under_point(x, y);
}

float autonomaus::ViewCortex::get_x_rotation() const
{
	return this->x_rotation;
}

float autonomaus::ViewCortex::get_y_rotation() const
{
	return this->y_rotation;
}

float autonomaus::ViewCortex::get_z_rotation() const
{
	return 0.0f; // . . . Gotta implement them all!
}

float autonomaus::ViewCortex::get_confidence() const
{
	return this->confidence;
}

float autonomaus::ViewCortex::get_y_rotation_difference(
	const thoreau::TileCoordinate& tile_coordinate) const
{
	return get_y_rotation_difference(tile_coordinate, this->player_tile_coordinate);
}

float autonomaus::ViewCortex::get_y_rotation_difference(
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

float autonomaus::ViewCortex::get_y_rotation_difference(const glm::vec3& position) const
{
	return get_y_rotation_difference(position, this->player_position);
}

float autonomaus::ViewCortex::get_y_rotation_difference(
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

autonomaus::Cortex::Result autonomaus::ViewCortex::focus_y(
	const thoreau::TileCoordinate& tile_coordinate,
	float offset,
	float range,
	int num_attempts)
{
	glm::vec3 tile_position;
	tile_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	tile_position.y = 0.0f;
	tile_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

	return focus_y(tile_position, offset, range, num_attempts);
}

autonomaus::Cortex::Result autonomaus::ViewCortex::focus_y(
	const glm::vec3& position,
	float offset,
	float range,
	int num_attempts)
{
	auto action = new FocusYAction(
		this,
		num_attempts, range,
		offset,
		position, this->player_position);
	return perform(action);
}

autonomaus::Cortex::Result autonomaus::ViewCortex::focus_x(
	float desired_angle,
	float range,
	int num_attempts)
{
	auto action = new FocusXAction(
		this,
		num_attempts, range,
		desired_angle);
	return perform(action);
}

glm::ivec4 autonomaus::ViewCortex::get_safe_viewport() const
{
	static bool warned = false;
	if (!warned)
	{
		Log::critical("Safe viewport not yet implemented. Returning world viewport.");
		warned = true;
	}

	return camera.get_viewport();
}

void autonomaus::ViewCortex::update()
{
	Cortex::update();

	auto previous_player_position = this->player_position;
	auto previous_player_tile_coordinate = this->player_tile_coordinate;

	this->camera = Camera(get_autonomaus().get_draw_state());
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
				if (actor_position.y > best_y || distance < best_distance)
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

			auto sector_coordinate = thoreau::sector_from_tile(this->player_tile_coordinate);
			if (sector_coordinate != this->player_sector_coordinate)
			{
				player_moved = true;
				this->player_sector_coordinate = sector_coordinate;
			}
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
			Log::critical("ViewCortex state when minimap isn't visible not yet implemented");
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
		clear(HIT_TAG);
		this->tiles_dirty = true;
	}

	if (this->player_position != previous_player_position ||
		this->player_tile_coordinate != previous_player_tile_coordinate)
	{
		last_player_movement = get_current_time();
	}

	this->player_animations.clear();
	{
		auto& animated_state = this->world->get_animated_state();
		if (animated_state.has(this->player_key))
		{
			auto& player = animated_state.get(this->player_key);
			for (auto& i: this->animations)
			{
				if (i.second.match(player.animation))
				{
					this->player_animations.insert(i.first);
				}
			}
		}

		auto& npcs = *get_autonomaus().get_npc_state();
		for (auto& i: npcs)
		{
			auto& npc = i.second;
			if (npc.get_world_coordinate() == this->player_key)
			{
				this->player_animations.insert(npc.get_profile_variant());
			}
		}
	}

	clear(ACTOR_TAG);
	this->actors_dirty = true;
}

void autonomaus::ViewCortex::load_animations()
{
	auto& resource_manager = get_autonomaus().get_game_state().get_resource_manager();

	boost::filesystem::path path(resource_manager.get_path());
	path.append("queries");
	path.append("player");
	path.append("animations");

	resource_manager.for_each_file(
		path.string(),
		[this](auto& filename)
		{
			boost::filesystem::path path(filename);
			std::ifstream stream(filename, std::ios::binary);
			mantorok::DATReader reader(stream);
			mantorok::DATDeserializer<chomsky::ActorAnimation>::deserialize(
				this->animations[path.stem().string()], reader);
		});
}

void autonomaus::ViewCortex::populate_scene() const
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

void autonomaus::ViewCortex::add_sector(const thoreau::SectorCoordinate& sector) const
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

			if (tile.has_elevation())
			{
				add_hit(*current, tile);
			}
		}

		++current;
	}
}

void autonomaus::ViewCortex::add_actor(const kvlc::Bounds& bounds, const glm::mat4& transform) const
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

	dGeomSetData(obstacle.shape, (void*)(std::size_t)ACTOR_TAG);
	dGeomSetBody(obstacle.shape, obstacle.body);
	dBodySetPosition(obstacle.body, box_position.x, box_position.y, box_position.z);
	dBodySetRotation(obstacle.body, box_rotation);

	this->obstacles.push_back(obstacle);
}

void autonomaus::ViewCortex::add_tile(
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

	dGeomSetData(obstacle.shape, (void*)(std::size_t)TILE_TAG);
	dGeomSetBody(obstacle.shape, obstacle.body);
	dBodySetPosition(obstacle.body, box_position.x, box_position.y, box_position.z);

	this->obstacles.push_back(obstacle);
}

void autonomaus::ViewCortex::add_hit(
	const thoreau::TileCoordinate& tile_coordinate,
	const thoreau::Tile& tile) const
{
	// The virtual world would look like something from Minecraft.

	glm::vec3 box_position;
	box_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	box_position.y = tile.get_elevation();
	box_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

	glm::vec3 box_size(0.0f);
	box_size.x = thoreau::TILE_SIZE;
	box_size.y = std::max(tile.get_max_elevation() - tile.get_min_elevation(), 1.0f);
	box_size.z = thoreau::TILE_SIZE;

	Obstacle obstacle;
	obstacle.tag = HIT_TAG;
	obstacle.shape = dCreateBox(this->scene_space, box_size.x, box_size.y, box_size.z);
	obstacle.body = dBodyCreate(this->scene_world);

	dGeomSetData(obstacle.shape, (void*)(std::size_t)HIT_TAG);
	dGeomSetBody(obstacle.shape, obstacle.body);
	dBodySetPosition(obstacle.body, box_position.x, box_position.y, box_position.z);

	this->obstacles.push_back(obstacle);
}

void autonomaus::ViewCortex::clear(int tag)
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

void autonomaus::ViewCortex::perform_space_collision(void* data, dGeomID a, dGeomID b)
{
	int* count = (int*)data;
	dContact contact;

	if (dCollide(a, b, 1, &contact.geom, sizeof(dContact)))
	{
		++(*count);
	}
}

void autonomaus::ViewCortex::get_tiles(void* data, dGeomID a, dGeomID b)
{
	std::vector<glm::vec3>& hits = *((std::vector<glm::vec3>*)data);

	dContact contact;
	if (dCollide(a, b, 1, &contact.geom, sizeof(dContact)))
	{
		int i = (std::intptr_t)dGeomGetData(a);
		int j = (std::intptr_t)dGeomGetData(b);

		if (i == HIT_TAG || j == HIT_TAG)
		{
			hits.emplace_back(contact.geom.pos[0], contact.geom.pos[1], contact.geom.pos[2]);
		}
	}
}

autonomaus::ViewCortex::FocusAction::FocusAction(
	ViewCortex* cortex,
	int num_attempts,
	float range) :
		status(std::make_shared<Cortex::Result::Status>(Cortex::Result::Status::working)),
		final_result(status, [this] { this->cancel(); })
{
	this->cortex = cortex;
	this->num_attempts = num_attempts;
	this->range = range;
}

autonomaus::ViewCortex::FocusAction::~FocusAction()
{
	cancel();
	this->final_result.terminate();
}

autonomaus::Cortex::Result autonomaus::ViewCortex::FocusAction::get_result()
{
	return this->final_result;
}

void autonomaus::ViewCortex::FocusAction::update()
{
	switch (this->state)
	{
		case state_start:
			{
				float angle = get_angle();
				if (std::abs(angle) < this->range)
				{
					Log::info("(ViewCortex::FocusAction) Point in focus.");

					*this->status = Result::Status::success;
					this->state = state_done;
					break;
				}

				if (this->current_attempt == 0)
				{
					this->sign = glm::sign(angle);
				}
				else if (this->current_attempt >= this->num_attempts && this->num_attempts != 0)
				{
					Log::info("(ViewCortex::FocusAction) Could not focus point in {} attempts.", this->num_attempts);
					*this->status = Result::Status::failure;
					this->state = state_done;
					break;
				}
				++this->current_attempt;

				auto normal = get_normal();
				auto viewport = this->cortex->camera.get_viewport();
				auto viewport_offset = glm::vec2(viewport.x, viewport.y);
				auto viewport_size = glm::vec2(viewport.z, viewport.w);
				auto viewport_length = glm::dot(normal, viewport_size);
				auto gesture_length = RNG::nextf(MIN_LENGTH, MAX_LENGTH) * viewport_length;
				auto gesture_offset = gesture_length * this->sign;

				glm::vec2 mouse_position;
				glm::vec2 relative_mouse_position;
				{
					auto& input_state = this->cortex->get_autonomaus().get_input_manager().get_state();
					mouse_position.x = input_state.get_mouse_x();
					mouse_position.y = input_state.get_mouse_y();
					relative_mouse_position = mouse_position - viewport_offset;
				}
				float relative_to_offset = glm::dot(normal, relative_mouse_position) + gesture_offset;

				bool mouse_from_position_good = true;
				bool mouse_to_position_good = true;
				if (relative_mouse_position.x < viewport.x ||
					relative_mouse_position.x > viewport.z ||
					relative_mouse_position.y < viewport.y ||
					relative_mouse_position.y > viewport.w)
				{
					mouse_from_position_good = false;
				}

				if (relative_to_offset < 0.0f || relative_to_offset > viewport_length)
				{
					mouse_to_position_good = false;
				}

				bool mouse_position_ready = mouse_from_position_good && mouse_to_position_good;
				auto from_position = glm::vec2(0.0f);
				if (!mouse_position_ready)
				{
					bool from_position_ready = true;
					if (mouse_from_position_good)
					{
						float from_offset = glm::dot(relative_mouse_position, normal);
						if (relative_to_offset < viewport.x)
						{
							from_offset += glm::abs(relative_to_offset - glm::dot(normal, viewport_offset));
						}
						else
						{
							from_offset -= glm::abs(relative_to_offset - glm::dot(normal, viewport_size));
						}
						
						if (from_offset < viewport.x || from_offset > viewport.z)
						{
							from_position_ready = false;
						}
						else
						{
							from_position += from_offset * normal;
						}
					}
					else
					{
						from_position_ready = false;
					}

					if (!from_position_ready)
					{
						float center = viewport_length / 2.0f - gesture_length / 2.0f;
						from_position += (center + gesture_offset / 2.0f) * normal;
					}

					glm::vec2 cross = normal;
					std::swap(cross.x, cross.y);

					auto cross_mouse_offset = glm::dot(relative_mouse_position, cross);
					auto cross_viewport_length = glm::dot(viewport_size, cross);
					if (cross_mouse_offset < 0.0f || cross_mouse_offset > cross_viewport_length)
					{
						float center = cross_viewport_length / 2.0f;
						center += RNG::nextf(-DEVIATION, DEVIATION) * cross_viewport_length;
						from_position += cross * center;
					}
					else
					{
						from_position += cross * relative_mouse_position;
					}

					from_position += viewport_offset;
				}
				else
				{
					from_position = mouse_position;
				}

				auto to_position = from_position;
				to_position += normal * gesture_offset;
				to_position.x += RNG::nextf(-DEVIATION, DEVIATION) * viewport_size.x;
				to_position.y += RNG::nextf(-DEVIATION, DEVIATION) * viewport_size.y;
				to_position = glm::clamp(to_position, viewport_offset, viewport_offset + viewport_size);

				Log::info(
					"(ViewCortex::FocusAction) Dragging mouse from ({}, {}) to ({}, {}).",
					from_position.x, from_position.y,
					to_position.x, to_position.y);

				MotionSequence sequence(*this->cortex);
				sequence.drag(from_position, to_position, eiznebahn::MouseButton::button_middle);
				this->current_result = this->cortex->perform(sequence);
				this->state = state_drag;
			}
			break;
		case state_drag:
			if (this->current_result.pending())
			{
				float angle = get_angle();
				if (std::abs(angle) < this->range)
				{
					Log::info("(ViewCortex::FocusAction) Point in focus.");

					this->current_result.cancel();
					*this->status = Result::Status::success;
					this->state = state_done;
				}
			}
			else if (this->current_result.get() == Result::Status::success)
			{
				this->state = state_start;
			}
			else
			{
				Log::info("(ViewCortex::FocusAction) Drag motion failed!");
				cancel();
			}
			break;
		case state_done:
			// Nothing.
			break;
		default:
			Log::warn("(ViewCortex::FocusAction) Bad state! Value: '{}'.", this->state);
			cancel();
			break;
	};
}

void autonomaus::ViewCortex::FocusAction::cancel()
{
	if (this->state != state_done)
	{
		this->current_result.cancel();
		*this->status = Result::Status::failure;
		this->state = state_done;
	}
}
