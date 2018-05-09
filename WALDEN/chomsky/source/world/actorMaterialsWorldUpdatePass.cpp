// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/chomsky.hpp"
#include "chomsky/world/actorMaterialsWorldUpdatePass.hpp"
#include "chomsky/world/detail/actor.hpp"

chomsky::ActorMaterialsWorldUpdatePass::ActorMaterialsWorldUpdatePass(
	const TileElevationState* tile_elevation_state,
	MaterialsState* material_state)
{
	this->tile_elevation_state = tile_elevation_state;
	this->material_state = material_state;
}

void chomsky::ActorMaterialsWorldUpdatePass::start()
{
	this->material_state->clear();
}

void chomsky::ActorMaterialsWorldUpdatePass::draw_model(
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
	auto& materials = this->material_state->add(world_coordinate);

	if (!model->has_texture_metas())
	{
		return;
	}

	if (event.get_atlas_name() == 0)
	{
		return;
	}

	auto iter = this->materials.find(model);
	if (iter == this->materials.end())
	{
		iter = this->materials.emplace(model, MaterialSet()).first;

		auto profile = model->get_profile();
		auto atlas_meta = profile->get_default_atlas_meta();
		auto& texture = event.get_atlas();
		for (std::size_t i = 0; i < model->get_num_vertices(); ++i)
		{
			auto texture_meta = model->get_texture_meta(i);
			auto texture_coordinate = profile->compute_texture_coordinate_to_atlas(
				glm::vec2(0.5f), texture_meta, atlas_meta);
			auto region = texture.get_region_from_texture_coordinate(
				texture_coordinate.s,
				texture_coordinate.t);
			if (region != nullptr)
			{
				auto color = glm::vec3(model->get_color(i));
				iter->second.add(0, texture.get_name(), region->get_name(), color);
			}
		}
	}

	for (auto i: iter->second)
	{
		auto texture = std::get<MaterialSet::TEXTURE>(i);
		auto region = std::get<MaterialSet::REGION>(i);
		auto color = std::get<MaterialSet::COLOR>(i);
		materials.add(0, texture, region, color);
	}
}

void chomsky::ActorMaterialsWorldUpdatePass::invalidate_model(
	const kvre::Model* model)
{
	this->materials.erase(model);
}
