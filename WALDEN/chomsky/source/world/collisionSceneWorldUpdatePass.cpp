// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/detail/actor.hpp"
#include "chomsky/world/collisionSceneWorldUpdatePass.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"

chomsky::CollisionSceneWorldUpdatePass::CollisionSceneWorldUpdatePass(
	const TransformState* transform_state,
	const kvlc::Bounds* bounds,
	kvlc::Scene* scene,
	SceneTagDrawTagMap* scene_tag_draw_tag_map)
{
	this->transform_state = transform_state;
	this->bounds = bounds;
	this->scene = scene;
	this->scene_tag_draw_tag_map = scene_tag_draw_tag_map;
}

void chomsky::CollisionSceneWorldUpdatePass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (this->tags.count(tag) != 0)
	{
		if (chomsky::is_actor_draw_event(event))
		{
			invalidate_tag(tag);
		}

		return;
	}

	add_mesh(tag, event);
}

void chomsky::CollisionSceneWorldUpdatePass::draw_terrain(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	if (this->tags.count(tag) != 0)
	{
		return;
	}

	add_mesh(tag, event);
}

void chomsky::CollisionSceneWorldUpdatePass::invalidate_tag(int tag)
{
	auto iter = this->tags.find(tag);
	if (iter != this->tags.end())
	{
		if (iter->second != 0)
		{
			this->scene->remove(iter->second);
			this->scene_tag_draw_tag_map->remove(iter->second);
		}

		this->tags.erase(iter);
	}
}

void chomsky::CollisionSceneWorldUpdatePass::add_mesh(
	int tag, const kompvter::ModelDrawEvent& event)
{
	if (!this->transform_state->has(tag))
	{
		return;
	}

	auto& points = this->transform_state->get(tag);
	auto begin = kvre::IndexedVertexIterator::begin(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());
	auto end = kvre::IndexedVertexIterator::end(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());
	auto num_indices = std::distance(begin, end);
	if (num_indices == 0 || num_indices % 3 != 0)
	{
		return;
	}

	kvlc::Mesh mesh;
	mesh.set_num_points(points->size());
	auto mesh_points = mesh.get_points();
	for (std::size_t i = 0; i < points->size(); ++i)
	{
		mesh_points[i] = glm::vec4(points[i], 0.0f);
	}

	mesh.set_num_triangles(num_indices / 3);
	auto mesh_indices = mesh.get_triangles();
	for (std::size_t i = 0; i < num_indices; ++i)
	{
		auto current_index = *(begin + i);
		mesh_indices[i] = current_index;
	}

	auto scene_tag = this->scene->add(mesh);
	this->tags[tag] = scene_tag;
	this->scene_tag_draw_tag_map->add(scene_tag) = tag;
}
