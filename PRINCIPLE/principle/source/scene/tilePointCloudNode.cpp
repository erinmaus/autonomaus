// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/scene/tilePointCloudNode.hpp"

principle::TilePointCloudNode::TilePointCloudNode(
	Scene* scene,
	const chomsky::TileSpacePointCloud& point_cloud) :
		SceneNode(scene)
{
	build(point_cloud);
}

void principle::TilePointCloudNode::draw()
{
	if (this->num_points > 0)
	{
		this->mesh.draw(GL_NONE, 0, this->num_points, GL_POINTS);
	}
}

void principle::TilePointCloudNode::build(
	const chomsky::TileSpacePointCloud& point_cloud)
{
	struct Point
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	std::vector<Point> points;
	for (auto input: point_cloud)
	{
		Point point;
		point.position = input.position;
		point.color = glm::vec4(input.color, 1.0f);

		// This allows some variance so we can see points with different colors.
		// (In other words, they won't stack). The values come from the SIZE
		// constants in chomsky::TileSpacePointCloud.
		point.position += input.color * glm::vec3(15, 255, 15);

		points.push_back(point);
	}

	if (points.empty())
	{
		return;
	}

	kompvter::Buffer buffer;
	{
		std::size_t buffer_size = points.size() * sizeof(Point);
		buffer.allocate(buffer_size);
		buffer.update(0, (const std::uint8_t*)&points[0], buffer_size);
	}
	this->mesh_vertices.set_buffer(buffer);

	this->mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_position, 
		&this->mesh_vertices,
		3, GL_FLOAT, GL_FALSE,
		offsetof(Point, position), sizeof(Point));
	this->mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_color, 
		&this->mesh_vertices,
		4, GL_FLOAT, GL_FALSE,
		offsetof(Point, color), sizeof(Point));

	this->num_points = points.size();
}
