// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/actorPointCloudWorldUpdatePass.hpp"
#include "chomsky/world/detail/actor.hpp"
#include "chomsky/world/detail/scenery.hpp"
#include "thoreau/thoreau.hpp"
#include "thoreau/tileCoordinate.hpp"

chomsky::ActorPointCloudWorldUpdatePass::ActorPointCloudWorldUpdatePass(
	const TileElevationState* tile_elevation_state,
	ActorDrawState* actor_draw_state,
	ActorPointCloudState* actor_point_cloud_state)
{
	this->tile_elevation_state = tile_elevation_state;
	this->actor_draw_state = actor_draw_state;
	this->actor_point_cloud_state = actor_point_cloud_state;
}

void chomsky::ActorPointCloudWorldUpdatePass::start()
{
	this->actor_point_cloud_state->clear();
	this->working_static_attachments.clear();
	this->working_static_models.clear();
}

void chomsky::ActorPointCloudWorldUpdatePass::stop()
{
	for (auto& i: *this->actor_draw_state)
	{
		auto key = i.first;
		auto& draw = i.second;

		ModelSet model_set(draw.models_begin(), draw.models_end());
		{
			auto static_models = this->working_static_models.equal_range(key);
			for (auto j = static_models.first; j != static_models.second; ++j)
			{
				model_set.insert(j->second);
				draw.add_model(j->second);
			}
		}

		auto iter = this->finalized_point_clouds.find(model_set);
		if (iter == this->finalized_point_clouds.end())
		{
			auto point_cloud = std::make_shared<ActorPointCloud>();
			for (auto model: model_set)
			{
				auto working_point_cloud = this->working_point_clouds.find(model);
				if (working_point_cloud == this->working_point_clouds.end())
				{
					continue;
				}

				for (auto& i: *working_point_cloud->second)
				{
					auto& tile_point_cloud = point_cloud->add(i.first);

					tile_point_cloud.merge(i.second);
				}
			}

			auto static_attachments = this->working_static_attachments.equal_range(key);
			for (auto j = static_attachments.first; j != static_attachments.second; ++j)
			{
				for (auto& k: *j->second)
				{
					auto& tile_point_cloud = point_cloud->add(k.first);

					tile_point_cloud.merge(k.second);
				}
			}

			iter = this->finalized_point_clouds.emplace(model_set, point_cloud).first;
		}

		this->actor_point_cloud_state->add(key) = iter->second;
	}
}

void chomsky::ActorPointCloudWorldUpdatePass::draw_model(
	int tag,
	const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	thoreau::WorldCoordinate world_coordinate;
	{
		auto transformed_origin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		transformed_origin = event.get_model_matrix() * transformed_origin;

		world_coordinate = thoreau::world_from_position(glm::vec3(transformed_origin));
		auto tile_coordinate = thoreau::tile_from_world(glm::vec3(transformed_origin));
		if (tile_elevation_state->has(tile_coordinate))
		{
			tile_elevation_state->get(tile_coordinate).try_find_tag(
				transformed_origin.y,
				world_coordinate.layer);
		}
	}

	if (!is_actor_draw_event(event) && !this->actor_draw_state->has(world_coordinate))
	{
		return;
	}

	auto working_point_cloud = this->working_point_clouds.find(model);
	if (working_point_cloud == this->working_point_clouds.end())
	{
		auto point_cloud = std::make_shared<ActorPointCloud>();
		for (std::size_t i = 0; i < model->get_num_vertices(); ++i)
		{
			auto position = model->get_position(i);
			auto color = glm::vec3(model->get_color(i));

			auto tile_coordinate = thoreau::tile_from_world(position);
			auto tile_relative_position = position;
			tile_relative_position.x -= tile_coordinate.x * thoreau::TILE_SIZE;
			tile_relative_position.z -= tile_coordinate.y * thoreau::TILE_SIZE;

			if (tile_relative_position.x < 0.0f)
			{
				tile_relative_position.x = thoreau::TILE_SIZE + tile_relative_position.x;
			}

			if (tile_relative_position.z < 0.0f)
			{
				tile_relative_position.z = thoreau::TILE_SIZE + tile_relative_position.z;
			}

			auto& tile_point_cloud = point_cloud->add(tile_coordinate);
			tile_point_cloud.add(tile_relative_position, color);
		}

		working_point_cloud = this->working_point_clouds.emplace(model, point_cloud).first;
	}

	if (!is_actor_draw_event(event))
	{
		this->working_static_attachments.emplace(world_coordinate, working_point_cloud->second);
		this->working_static_models.emplace(world_coordinate, model);
	}
}

void chomsky::ActorPointCloudWorldUpdatePass::invalidate_model(
	const kvre::Model* model)
{
	auto iter = this->working_point_clouds.find(model);
	if (iter != this->working_point_clouds.end())
	{
		for (auto i = this->finalized_point_clouds.begin(); i != this->finalized_point_clouds.end(); /* Nop. */)
		{
			auto& model_set = i->first;
			if (model_set.count(model) != 0)
			{
				i = this->finalized_point_clouds.erase(i);
			}
			else
			{
				++i;
			}
		}

		this->working_point_clouds.erase(model);
	}
}
