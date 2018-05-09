// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_TILE_POINT_CLOUD_NODE_HPP
#define PRINCIPLE_SCENE_TILE_POINT_CLOUD_NODE_HPP

#include "chomsky/resources/tileSpacePointCloud.hpp"
#include "kvncer/graphics/mesh.hpp"
#include "kvncer/graphics/meshBuffer.hpp"
#include "kvncer/graphics/shader.hpp"
#include "principle/common/gameManager.hpp"
#include "principle/scene/sceneNode.hpp"

namespace principle
{
	class TilePointCloudNode : public SceneNode
	{
	public:
		TilePointCloudNode(
			Scene* scene,
			const chomsky::TileSpacePointCloud& point_cloud);
		~TilePointCloudNode() = default;

		void draw() override;

	private:
		void build(const chomsky::TileSpacePointCloud& point_cloud);

		kvncer::Mesh mesh;
		kvncer::MeshBuffer mesh_vertices;

		std::size_t num_points = 0;
	};
}

#endif
