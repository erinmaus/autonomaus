// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_VIEW_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_VIEW_CORTEX_HPP

#include <ode/ode.h>
#include <glm/glm.hpp>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include "autonomaus/state/camera.hpp"
#include "autonomaus/state/map.hpp"
#include "autonomaus/state/world.hpp"
#include "autonomaus/state/memory/cortex.hpp"
#include "chomsky/gui/guiContent.hpp"
#include "chomsky/world/actorAnimation.hpp"
#include "kvlc/geometry/ray.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "thoreau/map.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/sectorCoordinate.hpp"

namespace autonomaus
{
	class ViewCortex : public Cortex
	{
	public:
		ViewCortex(Autonomaus& autonomaus);
		~ViewCortex();

		bool possibly_occluded(const kvlc::Ray& ray, float length) const;
		bool possibly_occluded(const thoreau::TileCoordinate& tile_coordinate) const;
		bool possibly_occluded(const glm::vec3& position) const;

		bool is_in_bounds(const thoreau::TileCoordinate& tile_coordinate) const;
		bool is_in_bounds(const glm::vec3& position) const;

		Cortex::Result reset_target_idle_time();

		float get_target_idle_time() const;

		const glm::vec3& get_target_position() const;
		const thoreau::TileCoordinate& get_target_tile_coordinate() const;

		std::unordered_set<std::string> get_target_animations() const;

		thoreau::TileCoordinate get_tile_under_point(int x, int y) const;
		thoreau::TileCoordinate get_tile_under_mouse() const;

		float get_x_rotation() const;
		float get_y_rotation() const;
		float get_z_rotation() const;

		float get_confidence() const;

		float get_y_rotation_difference(
			const thoreau::TileCoordinate& tile) const;
		float get_y_rotation_difference(
			const thoreau::TileCoordinate& tile,
			const thoreau::TileCoordinate& reference) const;
		float get_y_rotation_difference(const glm::vec3& position) const;
		float get_y_rotation_difference(
			const glm::vec3& position,
			const glm::vec3& reference) const;

		Cortex::Result focus_y(
			const thoreau::TileCoordinate& tile_coordinate,
			float offset = 0.0f,
			float range = M_PI / 4.0f,
			int num_attempts = 1);
		Cortex::Result focus_y(
			const glm::vec3& position,
			float offset = 0.0f,
			float range = M_PI / 4.0f,
			int num_attempts = 1);

		Cortex::Result focus_x(
			float desired_angle,
			float range = M_PI / 8.0f,
			int num_attempts = 1);

		glm::ivec4 get_safe_viewport() const;

		void update() override;

	private:
		const static int ACTOR_TAG = 1;
		const static int TILE_TAG = 2;
		const static int HIT_TAG = 3;

		const static float SAFE_VIEWPORT_MARGIN;

		const World* world;
		const glooper::Minimap* minimap;
		const thoreau::Map* map;

		std::map<std::string, chomsky::ActorAnimation> animations;
		void load_animations();

		float last_player_movement = HUGE_VALF;
		std::unordered_set<std::string> player_animations;
		glm::vec3 player_position = glm::vec3(0.0f);
		thoreau::TileCoordinate player_sector_coordinate = thoreau::SectorCoordinate(0, 0);
		thoreau::TileCoordinate player_tile_coordinate = thoreau::TileCoordinate(0, 0);
		float confidence = -HUGE_VALF;
		thoreau::WorldCoordinate player_key;

		float y_rotation = 0.0f;
		float x_rotation = 0.0f;
		glm::vec3 camera_position;
		Camera camera;

		struct Obstacle
		{
			int tag;
			dGeomID shape;
			dBodyID body;
		};
		mutable std::vector<Obstacle> obstacles;
		mutable bool tiles_dirty = false;
		mutable bool actors_dirty = false;
		void populate_scene() const;

		void add_sector(const thoreau::SectorCoordinate& sector) const;
		void add_tile(
			const thoreau::TileCoordinate& tile_coordinate,
			const thoreau::Tile& tile) const;
		void add_hit(
			const thoreau::TileCoordinate& tile_coordinate,
			const thoreau::Tile& tile) const;
		void add_actor(
			const kvlc::Bounds& bounds,
			const glm::mat4& transform) const;
		void clear(int tag);

		dWorldID scene_world;
		dSpaceID scene_space;

		static void perform_space_collision(void* data, dGeomID o1, dGeomID o2);
		static void get_tiles(void* data, dGeomID o1, dGeomID o2);

		struct FocusAction;
		struct FocusXAction;
		struct FocusYAction;
	};
}

#endif
