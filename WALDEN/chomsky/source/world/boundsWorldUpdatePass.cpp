// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/boundsWorldUpdatePass.hpp"
#include "chomsky/world/detail/scenery.hpp"
#include "chomsky/world/detail/prop.hpp"
#include "chomsky/world/detail/actor.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"

chomsky::BoundsWorldUpdatePass::BoundsWorldUpdatePass(
	const TransformState* transform_state,
	kvlc::Bounds* bounds)
{
	this->transform_state = transform_state;
	this->bounds = bounds;
}

void chomsky::BoundsWorldUpdatePass::stop()
{
	*this->bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
	for (auto& i: this->tags)
	{
		this->bounds->expand(i.second);
	}
}

void chomsky::BoundsWorldUpdatePass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	expand(tag, event);
}

void chomsky::BoundsWorldUpdatePass::draw_terrain(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	expand(tag, event);
}

void chomsky::BoundsWorldUpdatePass::invalidate_tag(int tag)
{
	this->tags.erase(tag);
}

void chomsky::BoundsWorldUpdatePass::expand(
	int tag, const kompvter::ModelDrawEvent& event)
{
	if (this->tags.count(tag) != 0)
	{
		return;
	}

	if (!this->transform_state->has(tag))
	{
		return;
	}

	auto begin = kvre::IndexedVertexIterator::begin(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());
	auto end = kvre::IndexedVertexIterator::end(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());

	auto& points = this->transform_state->get(tag);
	kvlc::Bounds bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
	for (auto i = begin; i != end; ++i)
	{
		auto point = points[*i];
		bounds.expand(point);
	}

	this->tags[tag] = bounds;
}
