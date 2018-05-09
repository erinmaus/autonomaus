// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CONTINENT1_TILE_SNAPSHOT_HPP
#define CONTINENT1_TILE_SNAPSHOT_HPP

#include <glm/glm.hpp>
#include "continent1/drawSnapshot.hpp"
#include "kompvter/graphics/buffer.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvre/model/model.hpp"
#include "kvre/terrain/terrain.hpp"
#include "thoreau/tileCoordinate.hpp"

namespace continent1
{
	class TileSnapshot
	{
	public:
		TileSnapshot() = default;
		TileSnapshot(const thoreau::TileCoordinate& tile_coordinate);
		~TileSnapshot() = default;

		const thoreau::TileCoordinate& get_tile_coordinate() const;

		bool add_model(
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper,
			const glm::mat4& model_matrix,
			int tag);
		bool add_terrain(
			const kvre::Terrain* terrain,
			const glm::mat4& model_matrix,
			int tag);
		bool add_water(
			const kvre::Terrain* water,
			const glm::mat4& model_matrix,
			int tag);

		std::size_t count() const;
		bool empty() const;

		const DrawSnapshot& get(std::size_t index) const;

	private:
		typedef std::vector<DrawSnapshot> DrawCollection;
		DrawCollection draws;

		thoreau::TileCoordinate tile_coordinate;
		kvlc::Bounds bounds;

	public:
		typedef DrawCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
