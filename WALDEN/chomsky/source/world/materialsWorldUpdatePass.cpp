// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <boost/container/flat_set.hpp>
#include "chomsky/chomsky.hpp"
#include "chomsky/world/materialsWorldUpdatePass.hpp"
#include "chomsky/world/detail/common.hpp"
#include "chomsky/world/detail/actor.hpp"
#include "chomsky/world/detail/prop.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"

chomsky::MaterialsWorldUpdatePass::MaterialsWorldUpdatePass(
	const TileElevationState* tile_elevation_state,
	DirtyTileState* dirty_tile_state,
	MaterialsState* material_state)
{
	this->tile_elevation_state = tile_elevation_state;
	this->dirty_tile_state = dirty_tile_state;
	this->material_state = material_state;
}

void chomsky::MaterialsWorldUpdatePass::stop()
{
	for (auto& i: this->pending_models)
	{
		i.wait();
	}
	this->pending_models.clear();

	for (auto& i: this->pending_draws)
	{
		auto& model_materials = this->materials[i.key];
		for (auto& j: model_materials)
		{
			auto tile_coordinate = j.first;
			tile_coordinate.x += i.offset.x;
			tile_coordinate.y += i.offset.y;
			tile_coordinate.layer += i.offset.layer;

			for (auto k: j.second)
			{
				auto texture = std::get<MaterialSet::TEXTURE>(k);
				auto region = std::get<MaterialSet::REGION>(k);
				auto color = std::get<MaterialSet::COLOR>(k);

				auto& tile_materials = this->material_state->add(tile_coordinate);
				tile_materials.add(i.tag, texture, region, color);

				if (this->dirty_tile_state != nullptr)
				{
					this->dirty_tile_state->add(tile_coordinate) = true;
				}
			}
		}
	}
	this->pending_draws.clear();
}

void chomsky::MaterialsWorldUpdatePass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (this->tags.count(tag) != 0)
	{
		if (is_actor_draw_event(event))
		{
			invalidate_tag(tag);
		}

		return;
	}

	if (is_actor_draw_event(event))
	{
		return;
	}

	if (!model->has_texture_metas())
	{
		return;
	}

	if (event.get_atlas_name() == 0)
	{
		return;
	}

	auto key = make_draw(event.get_atlas(), event.get_draw_parameters(), model);
	if (this->materials.count(key) == 0)
	{
		this->models.emplace(model, key);
		auto& model_materials = this->materials[key];
		auto result = std::async(
			std::launch::async,
			[this, model, event, &model_materials]
			{
				this->add_model(
					event.get_atlas(),
					event.get_draw_parameters(),
					event.get_vertex_index_mapper(),
					event.get_model_matrix(),
					!is_prop_draw_event(event),
					model,
					model_materials);
			});
	}

	auto world_position = event.get_model_matrix() * glm::vec4(glm::vec3(0.0f), 1.0f);
	auto offset = thoreau::tile_from_world(glm::vec3(world_position));
	if (is_prop_draw_event(event) && tile_elevation_state->has(offset))
	{
		auto& extents = tile_elevation_state->get(offset);
		extents.try_find_tag(world_position.y, offset.layer);
	}

	this->pending_draws.emplace_back();
	auto& draw = this->pending_draws.back();
	draw.tag = tag;
	draw.key = key;
	draw.offset = offset;

	this->tags.insert(tag);
}

chomsky::MaterialsWorldUpdatePass::DrawTuple
chomsky::MaterialsWorldUpdatePass::make_draw(
	const kompvter::Texture& atlas,
	const kompvter::DrawParameters& draw_parameters,
	const kvre::Model* model)
{
	return std::make_tuple(
		atlas.get_name(),
		draw_parameters.get_offset_start(),
		draw_parameters.get_offset_stop(),
		model);
}

void chomsky::MaterialsWorldUpdatePass::add_model(
	const kompvter::Texture& atlas,
	const kompvter::DrawParameters& draw_parameters,
	const kompvter::VertexIndexMapper& index_mapper,
	const glm::mat4& model_matrix,
	bool is_scenery,
	const kvre::Model* model,
	MaterialsState& model_materials)
{
	thoreau::TileCoordinate tile_coordinate_offset;
	if (is_scenery)
	{
		auto world_position = model_matrix * glm::vec4(glm::vec3(0.0f), 1.0f);
		tile_coordinate_offset = thoreau::tile_from_world(glm::vec3(world_position));
	}
	else
	{
		tile_coordinate_offset = thoreau::TileCoordinate(0, 0, 0);
	}

	auto profile = model->get_profile();
	auto atlas_meta = profile->get_default_atlas_meta();

	auto begin = kvre::IndexedVertexIterator::begin(index_mapper, draw_parameters);
	auto end = kvre::IndexedVertexIterator::end(index_mapper, draw_parameters);

	std::unordered_map<std::uint32_t, int> st_to_materials;
	while (begin != end)
	{
		auto index = *begin;
		++begin;

		auto texture_meta = model->get_texture_meta(index);
		std::uint32_t key = ((std::uint32_t)(texture_meta.x) << 16) | (std::uint32_t)(texture_meta.y);
		int region_name = -1;
		{
			auto iter = st_to_materials.find(key);
			if (iter == st_to_materials.end())
			{
				auto texture_coordinate = profile->compute_texture_coordinate_to_atlas(
					glm::vec2(0.5f), texture_meta, atlas_meta);
				auto region = atlas.get_region_from_texture_coordinate(
					texture_coordinate.s,
					texture_coordinate.t);

				region_name = (region == nullptr) ? -1 : region->get_name();
				st_to_materials[key] = region_name;
			}
			else
			{
				region_name = iter->second;
			}
		}

		auto position = model->get_position(index);
		auto tile_coordinate = thoreau::tile_from_world(position);
		if (is_scenery)
		{
			auto t = tile_coordinate;
			t.x += tile_coordinate_offset.x;
			t.y += tile_coordinate_offset.y;

			if (this->tile_elevation_state->has(t))
			{
				auto& extents = this->tile_elevation_state->get(t);
				extents.try_find_tag(position.y, tile_coordinate.layer);
			}
		}

		if (region_name != -1)
		{
			auto& tile_materials = model_materials.add(tile_coordinate);
			auto color = glm::vec3(model->get_color(index));
			tile_materials.add(0, atlas.get_name(), region_name, color);
		}
	}
}

void chomsky::MaterialsWorldUpdatePass::invalidate_tag(int tag)
{
	if (this->tags.count(tag) != 0)
	{
		for (auto i = this->material_state->begin(); i != this->material_state->end(); /* Nop. */)
		{
			i->second.remove(tag);
			if (i->second.empty())
			{
				i = this->material_state->remove(i);
			}
			else
			{
				++i;
			}
		}

		this->tags.erase(tag);
	}
}

void chomsky::MaterialsWorldUpdatePass::invalidate_model(const kvre::Model* model)
{
	auto range = this->models.equal_range(model);
	for (auto i = range.first; i != range.second; ++i)
	{
		this->materials.erase(i->second);
	}

	this->models.erase(model);
}
