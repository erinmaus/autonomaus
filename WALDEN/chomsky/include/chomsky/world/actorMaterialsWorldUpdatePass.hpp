// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_ACTOR_MATERIALS_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_ACTOR_MATERIALS_WORLD_UPDATE_PASS_HPP

#include "chomsky/world/materialSet.hpp"
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/terrainElevationWorldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "thoreau/worldCoordinate.hpp"

namespace chomsky
{
	class ActorMaterialsWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TileElevationState;
		typedef WorldState<thoreau::WorldCoordinate, MaterialSet> MaterialsState;

		ActorMaterialsWorldUpdatePass(
			const TileElevationState* tile_elevation_state,
			MaterialsState* material_state);
		~ActorMaterialsWorldUpdatePass() = default;

		void start() override;

		void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;
		void invalidate_model(const kvre::Model* model) override;

	private:
		const TileElevationState* tile_elevation_state;
		MaterialsState* material_state;

		std::map<const kvre::Model*, MaterialSet> materials;
	};
}

#endif
