// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_MAP_MAP_WORLD_UPDATE_BLOCK_HPP
#define GLOOPER_MAP_MAP_WORLD_UPDATE_BLOCK_HPP

#include "chomsky/world/collisionSceneWorldUpdatePass.hpp"
#include "chomsky/world/terrainElevationWorldUpdatePass.hpp"
#include "chomsky/world/waterElevationWorldUpdatePass.hpp"
#include "chomsky/world/transformWorldUpdatePass.hpp"
#include "chomsky/world/worldUpdateManager.hpp"
#include "chomsky/world/blocks/worldUpdateBlock.hpp"
#include "glooper/map/detail/mapElevationWorldUpdatePass.hpp"
#include "kvlc/geometry/bounds.hpp"
#include "kvlc/collision/scene.hpp"
#include "thoreau/map.hpp"

namespace glooper
{
	class MapWorldUpdateBlock : public chomsky::WorldUpdateBlock
	{
	public:
		typedef chomsky::TransformWorldUpdatePass::TransformState TransformState;
		MapWorldUpdateBlock(
			chomsky::WorldUpdateManager& world_update_manager,
			const thoreau::Map* map,
			const TransformState* transform_state,
			const kvlc::Bounds* bounds,
			thoreau::Map* runtime_map);
		~MapWorldUpdateBlock() = default;

		typedef chomsky::TerrainElevationWorldUpdatePass::TileElevationState TileElevationState;
		const TileElevationState* get_tile_elevation_state() const;
		const TileElevationState* get_water_elevation_state() const;

		typedef chomsky::CollisionSceneWorldUpdatePass::SceneTagDrawTagMap SceneTagDrawTagMap;
		const SceneTagDrawTagMap* get_scene_tag_draw_tag_map() const;

		typedef MapElevationWorldUpdatePass::RuntimeSectorFlagState RuntimeSectorFlagState;
		const RuntimeSectorFlagState* get_runtime_sector_flag_state() const;

		const kvlc::Scene* get_collision_scene() const;

		void update() override;

	private: 
		const thoreau::Map* map;
		const TransformState* transform_state;
		const kvlc::Bounds* bounds;
		thoreau::Map* runtime_map;

		TileElevationState terrain_tile_elevation_state;
		TileElevationState water_tile_elevation_state;
		TileElevationState tile_elevation_state;
		kvlc::Scene collision_scene;
		SceneTagDrawTagMap scene_tag_draw_tag_map;
		RuntimeSectorFlagState runtime_sector_flag_state;

		chomsky::CollisionSceneWorldUpdatePass collision_scene_pass;
		chomsky::TerrainElevationWorldUpdatePass terrain_elevation_pass;
		chomsky::WaterElevationWorldUpdatePass water_elevation_pass;
		MapElevationWorldUpdatePass map_elevation_pass;
	};
}

#endif
