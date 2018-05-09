// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/chomsky.hpp"
#include "chomsky/world/actorDrawWorldUpdatePass.hpp"
#include "chomsky/world/detail/actor.hpp"

chomsky::ActorDrawWorldUpdatePass::ActorDrawWorldUpdatePass(
	const TileElevationState* tile_elevation_state,
	DrawState* draw_state)
{
	this->tile_elevation_state = tile_elevation_state;
	this->draw_state = draw_state;
}

void chomsky::ActorDrawWorldUpdatePass::start()
{
	this->draw_state->clear();
}

void chomsky::ActorDrawWorldUpdatePass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (!is_actor_draw_event(event))
	{
		return;
	}

	auto world_coordinate = get_actor_position(
		event.get_model_matrix(),
		this->tile_elevation_state);
	if (!this->draw_state->has(world_coordinate))
	{
		auto& initial_draw = this->draw_state->add(world_coordinate);
		initial_draw.set_model_matrix(event.get_model_matrix());
	}
	
	auto& draw = this->draw_state->get(world_coordinate);
	draw.add_tag(tag);
	draw.add_model(model);

	for (std::size_t i = 0; i < event.get_num_bone_transforms(); ++i)
	{
		draw.add_bone(model, event.get_bone_transform(i));
	}
}
