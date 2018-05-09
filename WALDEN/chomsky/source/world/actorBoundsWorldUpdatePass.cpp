// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/chomsky.hpp"
#include "chomsky/world/actorBoundsWorldUpdatePass.hpp"
#include "chomsky/world/detail/actor.hpp"

chomsky::ActorBoundsWorldUpdatePass::ActorBoundsWorldUpdatePass(
	const TileElevationState* tile_elevation_state,
	BoundsState* bounds_state)
{
	this->tile_elevation_state = tile_elevation_state;
	this->bounds_state = bounds_state;
}

void chomsky::ActorBoundsWorldUpdatePass::start()
{
	this->bounds_state->clear();
}

void chomsky::ActorBoundsWorldUpdatePass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (!is_actor_draw_event(event))
	{
		return;
	}

	auto tile_coordinate = get_actor_position(
		event.get_model_matrix(),
		this->tile_elevation_state);
	if (!this->bounds_state->has(tile_coordinate))
	{
		auto& initial_bounds = this->bounds_state->add(tile_coordinate);
		initial_bounds.min = glm::vec3(HUGE_VALF);
		initial_bounds.max = glm::vec3(-HUGE_VALF);
	}
	
	auto& bounds = this->bounds_state->get(tile_coordinate);
	bounds.expand(model->get_bounds());
}
