// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_VIEW_HPP
#define AUTONOMAUS_STATE_VIEW_HPP

#include <ode/ode.h>
#include <glm/glm.hpp>
#include <vector>
#include "autonomaus/state/camera.hpp"
#include "autonomaus/state/map.hpp"
#include "autonomaus/state/world.hpp"
#include "chomsky/gui/guiContent.hpp"
#include "kvlc/geometry/ray.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "thoreau/map.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/sectorCoordinate.hpp"

namespace autonomaus
{
	class Camera;

	class View
	{
	public:
		View(const World& world, const Map& map);
		~View();

		bool occluded(const kvlc::Ray& ray, float length) const;
		bool occluded(const thoreau::TileCoordinate& tile_coordinate) const;
		bool is_in_bounds(const thoreau::TileCoordinate& tile_coordinate) const;

		void update(const Camera& camera);

		const glm::vec3& get_player_position() const;
		const thoreau::TileCoordinate& get_player_tile_coordinate() const;

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

		glm::ivec4 get_safe_viewport(const chomsky::GUIContent& root) const;

	private:
		const static int ACTOR_TAG = 0;
		const static int TILE_TAG = 1;

		const static float SAFE_VIEWPORT_MARGIN;

		const World* world;
		const glooper::Minimap* minimap;
		const thoreau::Map* map;

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
		void add_actor(
			const kvlc::Bounds& bounds,
			const glm::mat4& transform) const;
		void clear(int tag);

		dWorldID scene_world;
		dSpaceID scene_space;

		static void perform_space_collision(void* data, dGeomID o1, dGeomID o2);
	};
}

#endif
