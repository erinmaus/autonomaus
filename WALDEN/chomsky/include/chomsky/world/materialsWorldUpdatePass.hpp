// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_MATERIALS_WORLD_UPDATE_PASS_HPP
#define CHOMSKY_WORLD_MATERIALS_WORLD_UPDATE_PASS_HPP

#include <future>
#include <unordered_set>
#include "chomsky/world/materialSet.hpp"
#include "chomsky/world/tileElevationExtents.hpp"
#include "chomsky/world/terrainElevationWorldUpdatePass.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/world/worldUpdatePass.hpp"
#include "kompvter/graphics/texture.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace chomsky
{
	class MaterialsWorldUpdatePass : public WorldUpdatePass
	{
	public:
		typedef WorldState<thoreau::TileCoordinate, TileElevationExtents> TileElevationState;
		typedef WorldState<thoreau::TileCoordinate, MaterialSet> MaterialsState;
		typedef WorldState<thoreau::TileCoordinate, bool> DirtyTileState;

		MaterialsWorldUpdatePass(
			const TileElevationState* tile_elevation_state,
			DirtyTileState* dirty_tile_state,
			MaterialsState* material_state);
		~MaterialsWorldUpdatePass() = default;

		void stop() override;

		void draw_model(
			int tag,
			const kvre::Model* model,
			const kompvter::ModelDrawEvent& event) override;

		void invalidate_tag(int tag) override;
		void invalidate_model(const kvre::Model* model) override;

	private:
		const TileElevationState* tile_elevation_state;
		DirtyTileState* dirty_tile_state;
		MaterialsState* material_state;

		std::unordered_set<int> tags;

		typedef std::tuple<int, std::size_t, std::size_t, const kvre::Model*> DrawTuple;
		DrawTuple make_draw(
			const kompvter::Texture& atlas,
			const kompvter::DrawParameters& draw_parameters,
			const kvre::Model* model);

		std::map<DrawTuple, MaterialsState> materials;
		std::multimap<const kvre::Model*, DrawTuple> models;
		void add_model(
			const kompvter::Texture& atlas,
			const kompvter::DrawParameters& draw_parameters,
			const kompvter::VertexIndexMapper& index_mapper,
			const glm::mat4& matrix,
			bool is_scenery,
			const kvre::Model* model,
			MaterialsState& model_materials);

		std::vector<std::future<void>> pending_models;

		struct Draw
		{
			int tag;
			DrawTuple key;
			thoreau::TileCoordinate offset;
		};
		std::vector<Draw> pending_draws;
	};
}

#endif
