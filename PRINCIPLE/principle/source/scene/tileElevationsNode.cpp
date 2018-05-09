// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <tuple>
#include "kompvter/graphics/buffer.hpp"
#include "thoreau/thoreau.hpp"
#include "principle/scene/tileElevationsNode.hpp"

principle::TileElevationsNode::TileElevationsNode(
	Scene* scene,
	const GameManager::TileElevationState* tile_elevation_state) :
		SceneNode(scene)
{
	assert(tile_elevation_state != nullptr);
	this->tile_elevation_state = tile_elevation_state;
}

const principle::GameManager::TileElevationState*
principle::TileElevationsNode::get_tile_elevation_state() const
{
	return this->tile_elevation_state;
}

void principle::TileElevationsNode::build()
{
	std::vector<glm::vec3> points;
	for (auto& tile: *this->tile_elevation_state)
	{
		float x = (tile.first.x + 0.5f) * thoreau::TILE_SIZE;
		float z = (tile.first.y + 0.5f) * thoreau::TILE_SIZE;

		for (auto extent: tile.second)
		{
			float y = get_elevation(std::get<0>(extent), std::get<1>(extent));

			points.emplace_back(x, y, z);
		}
	}

	kompvter::Buffer buffer;
	{
		std::size_t buffer_size = points.size() * sizeof(glm::vec3);
		buffer.allocate(buffer_size);
		buffer.update(0, (const std::uint8_t*)&points[0], buffer_size);
	}

	this->mesh_vertices.set_buffer(buffer);
	this->mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_position, 
		&this->mesh_vertices,
		3, GL_FLOAT, GL_FALSE,
		0, sizeof(glm::vec3));
	this->num_points = points.size();
}

void principle::TileElevationsNode::draw()
{
	if (!is_built)
	{
		build();
		this->is_built = true;
	}

	this->mesh.draw(GL_NONE, 0, this->num_points, GL_POINTS);
}
