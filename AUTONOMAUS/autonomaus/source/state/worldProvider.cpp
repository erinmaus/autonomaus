// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <future>
#include "autonomaus/core/time.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/worldProvider.hpp"
#include "chomsky/world/detail/actor.hpp"
#include "chomsky/world/detail/prop.hpp"
#include "chomsky/world/detail/scenery.hpp"

chomsky::WorldUpdateManager* autonomaus::WorldProvider::get_world_update_manager()
{
	if (get_is_attached())
	{
		assert(this->world_updater.get() != nullptr);
		return &this->world_updater->world_update_manager;
	}

	return nullptr;
}

const chomsky::UtilityWorldUpdateBlock* autonomaus::WorldProvider::get_utility_block() const
{
	if (get_is_attached())
	{
		assert(this->world_updater.get() != nullptr);
		return &this->world_updater->utility_block;
	}

	return nullptr;
}

const glooper::MapWorldUpdateBlock* autonomaus::WorldProvider::get_map_block() const
{
	if (get_is_attached())
	{
		assert(this->world_updater.get() != nullptr);
		return &this->world_updater->map_block;
	}

	return nullptr;
}

const chomsky::AnimatedWorldUpdateBlock* autonomaus::WorldProvider::get_animated_block() const
{
	if (get_is_attached())
	{
		assert(this->world_updater.get() != nullptr);
		return &this->world_updater->animated_block;
	}

	return nullptr;
}

const chomsky::StaticWorldUpdateBlock* autonomaus::WorldProvider::get_static_block() const
{
	if (get_is_attached())
	{
		assert(this->world_updater.get() != nullptr);
		return &this->world_updater->static_block;
	}

	return nullptr;
}

const chomsky::StaticWorldUpdateBlock::DirtyTileState* autonomaus::WorldProvider::get_dirty_tile_state() const
{
	if (get_is_attached())
	{
		assert(this->world_updater.get() != nullptr);
		return &this->world_updater->dirty_tile_state;
	}

	return nullptr;
}

void autonomaus::WorldProvider::attach(GameStateManager& manager, Slot& slot)
{
	Base::attach(manager, slot);

	make_world_updater();
}

void autonomaus::WorldProvider::detach()
{
	world_updater.reset();

	Base::detach();
}

void autonomaus::WorldProvider::update()
{
	float before = get_current_time();
	this->world_updater->dirty_tile_state.clear();
	this->world_updater->world_update_manager.begin_update();
	{
		this->world_updater->utility_block.update();
		this->world_updater->map_block.update();

		auto a = std::async(std::launch::async, [this] { this->world_updater->static_block.update(); });
		auto b = std::async(std::launch::async, [this] { this->world_updater->animated_block.update(); });
		a.wait();
		b.wait();

		this->world_updater->world_update_manager.update_pass(&this->world_updater->draw_type_pass);
	}
	this->world_updater->world_update_manager.end_update();
	float after = get_current_time();

	auto& world = get_slot()->get_working();
	world.update(after, after - before);

	world.animated_state.clear();
	auto& draws = *this->world_updater->animated_block.get_actor_draw_state();
	auto& bounds = *this->world_updater->animated_block.get_actor_bounds_state();
	for (auto& draw: draws)
	{
		auto& animated = world.animated_state.add(draw.first);
		animated.model_matrix = draw.second.get_model_matrix();
		animated.bounds = bounds.get(draw.first);
		animated.animation.extract(draw.second);
	}

	world.draw_types.clear();
	world.draw_types.insert(
		this->world_updater->tag_draw_type_state.begin(),
		this->world_updater->tag_draw_type_state.end());
}

std::future<kvlc::Query> autonomaus::WorldProvider::test_collision(
	GameStateManager& game_state,
	const View& view,
	const glm::vec3& center,
	float radius)
{
	typedef std::promise<kvlc::Query> QueryPromise;
	auto query_promise = std::make_shared<QueryPromise>();
	game_state.execute<WorldProvider>(
		view,
		[center, radius, query_promise](auto& provider)
		{
			auto scene = provider.world_updater->map_block.get_collision_scene();
			auto tags = provider.world_updater->map_block.get_scene_tag_draw_tag_map();

			kvlc::Query query;
			scene->test(center, radius, query);

			kvlc::Query result;
			for (auto hit: query)
			{
				auto scene_tag = hit.first;
				if (!tags->has(scene_tag))
				{
					continue;
				}

				auto draw_tag = tags->get(scene_tag);
				result.add_result(draw_tag, hit.second);
			}

			query_promise->set_value(result);
		});

	return query_promise->get_future();
}

std::future<kvlc::Query> autonomaus::WorldProvider::test_collision(
	GameStateManager& game_state,
	const View& view,
	const kvlc::Bounds& bounds)
{
	typedef std::promise<kvlc::Query> QueryPromise;
	auto query_promise = std::make_shared<QueryPromise>();
	game_state.execute<WorldProvider>(
		view,
		[bounds, query_promise](auto& provider)
		{
			auto scene = provider.world_updater->map_block.get_collision_scene();
			auto tags = provider.world_updater->map_block.get_scene_tag_draw_tag_map();

			kvlc::Query query;
			scene->test(bounds, query);

			kvlc::Query result;
			for (auto hit: query)
			{
				auto scene_tag = hit.first;
				if (!tags->has(scene_tag))
				{
					continue;
				}

				auto draw_tag = tags->get(scene_tag);
				result.add_result(draw_tag, hit.second);
			}

			query_promise->set_value(result);
		});

	return query_promise->get_future();
}

std::future<kvlc::Query> autonomaus::WorldProvider::test_collision(
	GameStateManager& game_state,
	const View& view,
	const kvlc::Ray& ray,
	float length)
{
	typedef std::promise<kvlc::Query> QueryPromise;
	auto query_promise = std::make_shared<QueryPromise>();
	game_state.execute<WorldProvider>(
		view,
		[ray, length, query_promise](auto& provider)
		{
			auto scene = provider.world_updater->map_block.get_collision_scene();
			auto tags = provider.world_updater->map_block.get_scene_tag_draw_tag_map();

			kvlc::Query query;
			scene->test(ray, length, query);

			kvlc::Query result;
			for (auto hit: query)
			{
				auto scene_tag = hit.first;
				if (!tags->has(scene_tag))
				{
					continue;
				}

				auto draw_tag = tags->get(scene_tag);
				result.add_result(draw_tag, hit.second);
			}

			query_promise->set_value(result);
		});

	return query_promise->get_future();
}

std::future<kvlc::Query> autonomaus::WorldProvider::test_spray(
	GameStateManager& game_state,
	const View& view,
	const glm::vec3& center,
	float radius,
	int count)
{
	typedef std::promise<kvlc::Query> QueryPromise;
	auto query_promise = std::make_shared<QueryPromise>();
	game_state.execute<WorldProvider>(
		view,
		[center, radius, count, query_promise](auto& provider)
		{
			auto scene = provider.world_updater->map_block.get_collision_scene();
			auto tags = provider.world_updater->map_block.get_scene_tag_draw_tag_map();

			kvlc::Query result;
			for (int i = 0; i < count; ++i)
			{
				float s = ((float)i / (count - 1)) * 2.0f - 1.0f;

				for (int j = 0; j < count; ++j)
				{
					float t = ((float)j / (count - 1)) * 2.0f - 1.0f;

					for (int k = 0; k < count; ++k)
					{
						float u = ((float)k / (count - 1)) * 2.0f - 1.0f;

						kvlc::Ray ray;
						ray.direction.x = s;
						ray.direction.y = t;
						ray.direction.z = u;
						ray.direction = glm::normalize(ray.direction);
						ray.position = center;

						kvlc::Query query;
						scene->test(ray, radius, query);

						for (auto hit: query)
						{
							auto scene_tag = hit.first;
							if (!tags->has(scene_tag))
							{
								continue;
							}

							auto draw_tag = tags->get(scene_tag);
							result.add_result(draw_tag, hit.second);
						}
					}
				}
			}
			

			query_promise->set_value(result);
		});

	return query_promise->get_future();
}

autonomaus::WorldProvider::DrawTypeWorldUpdatePass::DrawTypeWorldUpdatePass(
	TagDrawTypeState* tag_draw_type_state)
{
	this->draws = tag_draw_type_state;
}

void autonomaus::WorldProvider::DrawTypeWorldUpdatePass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	int draw_type;
	if (chomsky::is_scenery_draw_event(event))
	{
		draw_type = World::draw_type_scenery;
	}
	else if (chomsky::is_prop_draw_event(event))
	{
		draw_type = World::draw_type_prop;
	}
	else if (chomsky::is_actor_draw_event(event))
	{
		draw_type = World::draw_type_actor;
	}
	else
	{
		draw_type = World::draw_type_unclassified;
	}

	(*this->draws)[tag] = draw_type;
}

void autonomaus::WorldProvider::DrawTypeWorldUpdatePass::draw_terrain(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	(*this->draws)[tag] = World::draw_type_terrain;
}

void autonomaus::WorldProvider::DrawTypeWorldUpdatePass::draw_water(
	int tag, const kvre::Terrain* water,
	const kompvter::ModelDrawEvent& event)
{
	(*this->draws)[tag] = World::draw_type_water;
}

void autonomaus::WorldProvider::DrawTypeWorldUpdatePass::draw_particles(
	int tag, const kvre::Particles* particles,
	const kompvter::ModelDrawEvent& event)
{
	(*this->draws)[tag] = World::draw_type_particles;
}

void autonomaus::WorldProvider::DrawTypeWorldUpdatePass::invalidate_tag(int tag)
{
	(*this->draws).erase(tag);
}

autonomaus::WorldProvider::WorldUpdater::WorldUpdater(GameStateManager& manager) :
	updater(manager.get_event_processors()),
	utility_block(world_update_manager),
	map_block(world_update_manager, &manager.get_resource_manager().get_map(), utility_block.get_transform_state(), utility_block.get_bounds(), &manager.get_resource_manager().get_runtime_map()),
	static_block(world_update_manager, &this->dirty_tile_state, utility_block.get_transform_state(), map_block.get_tile_elevation_state()),
	animated_block(world_update_manager, map_block.get_tile_elevation_state()),
	draw_type_pass(&tag_draw_type_state)
{
	this->updater.get_frame_processor().add(&this->world_update_manager);
	this->updater.get_buffer_processor().add(&this->world_update_manager);
	this->updater.get_model_processor().add(&this->world_update_manager);
}

autonomaus::WorldProvider::WorldUpdater::~WorldUpdater()
{
	this->updater.get_frame_processor().remove(&this->world_update_manager);
	this->updater.get_buffer_processor().remove(&this->world_update_manager);
	this->updater.get_model_processor().remove(&this->world_update_manager);
}

void autonomaus::WorldProvider::make_world_updater()
{
	assert(get_is_attached());
	this->world_updater = std::make_unique<WorldUpdater>(*get_manager());
}
