// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_TILE_ELEVATIONS_NODE_HPP
#define PRINCIPLE_SCENE_TILE_ELEVATIONS_NODE_HPP

#include "kvncer/graphics/mesh.hpp"
#include "kvncer/graphics/meshBuffer.hpp"
#include "kvncer/graphics/shader.hpp"
#include "principle/common/gameManager.hpp"
#include "principle/scene/sceneNode.hpp"

namespace principle
{
	class TileElevationsNode : public SceneNode
	{
	public:
		TileElevationsNode(
			Scene* scene,
			const GameManager::TileElevationState* tile_elevation_state);
		~TileElevationsNode() = default;

		void draw() override;

	protected:
		virtual float get_elevation(float min_y, float max_y) const = 0;
		const GameManager::TileElevationState* get_tile_elevation_state() const;

	private:
		void build();

		const GameManager::TileElevationState* tile_elevation_state;

		kvncer::Mesh mesh;
		kvncer::MeshBuffer mesh_vertices;
		bool is_built = false;

		std::size_t num_points = 0;
	};
}

#endif
