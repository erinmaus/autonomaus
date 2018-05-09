// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <chrono>
#include <algorithm>
#include "chomsky/world/worldUpdateManager.hpp"
#include "kvre/profile/profile.hpp"

void chomsky::WorldUpdateManager::add_pass(WorldUpdatePass* pass)
{
	this->passes.push_back(pass);
}

void chomsky::WorldUpdateManager::remove_pass(WorldUpdatePass* pass)
{
	for (auto i = this->passes.begin(); i != this->passes.end(); /* Nothing. */)
	{
		if (*i == pass)
		{
			i = this->passes.erase(i);
		}
		else
		{
			++i;
		}
	}
}

void chomsky::WorldUpdateManager::begin_update()
{
	// Nothing.
}

void chomsky::WorldUpdateManager::end_update()
{
	this->tag_manager.for_each_invalid_tag(
		[this](int tag)
		{
			for (auto pass: this->passes)
			{
				pass->invalidate_tag(tag);
			}
		});

	this->model_draws.clear();
	this->terrain_draws.clear();
	this->water_draws.clear();
	this->particle_draws.clear();
}

void chomsky::WorldUpdateManager::update_pass(WorldUpdatePass* pass)
{
	pass->start();
	after_start(pass);

	for (auto& i: this->terrain_draws)
	{
		auto& event = i.first;
		auto terrain = i.second;

		int tag = this->tag_manager.tag_terrain(event);
		pass->draw_terrain(tag, terrain, event);
		after_draw_terrain(pass, tag, terrain, event);
	}

	for (auto& i: this->model_draws)
	{
		auto& event = i.first;
		auto model = i.second;

		int tag = this->tag_manager.tag_model(event);
		pass->draw_model(tag, model, event);
		after_draw_model(pass, tag, model, event);
	}

	for (auto& i: this->water_draws)
	{
		auto& event = i.first;
		auto water = i.second;

		int tag = this->tag_manager.tag_water(event);
		pass->draw_water(tag, water, event);
		after_draw_water(pass, tag, water, event);
	}

	for (auto& i: this->particle_draws)
	{
		auto& event = i.first;
		auto particles = i.second;

		int tag = this->tag_manager.tag_particle(event);
		pass->draw_particles(tag, particles, event);
		after_draw_particles(pass, tag, particles, event);
	}

	pass->stop();
	after_stop(pass);
}

void chomsky::WorldUpdateManager::update()
{
	begin_update();

	for (auto pass: this->passes)
	{
		update_pass(pass);
	}

	end_update();
}

void chomsky::WorldUpdateManager::reset()
{
	assert(!this->is_updating);

	this->tag_manager.invalidate_all();
	this->tag_manager.for_each_invalid_tag(
		[this](int tag)
		{
			for (auto pass: this->passes)
			{
				pass->invalidate_tag(tag);
			}
		});

	for (auto model: this->models)
	{
		for (auto pass: this->passes)
		{
			pass->invalidate_model(model);
		}
	}

	for (auto terrain: this->terrains)
	{
		for (auto pass: this->passes)
		{
			pass->invalidate_terrain(terrain);
		}
	}

	for (auto water: this->waters)
	{
		for (auto pass: this->passes)
		{
			pass->invalidate_water(water);
		}
	}

	for (auto particle: this->particles)
	{
		for (auto pass: this->passes)
		{
			pass->invalidate_particles(particle);
		}
	}

	this->model_draws.clear();
	this->terrain_draws.clear();
}

void chomsky::WorldUpdateManager::on_buffer_create(
	const kompvter::BufferEvent& event)
{
	invalidate_buffer(event.get_name());
}

void chomsky::WorldUpdateManager::on_buffer_delete(
	const kompvter::BufferEvent& event)
{
	invalidate_buffer(event.get_name());
}

void chomsky::WorldUpdateManager::on_buffer_allocate(
	const kompvter::BufferEvent& event)
{
	invalidate_buffer(event.get_name());
}

void chomsky::WorldUpdateManager::on_buffer_clear(
	const kompvter::BufferEvent& event)
{
	invalidate_buffer(event.get_name());
}

void chomsky::WorldUpdateManager::on_buffer_update(
	const kompvter::BufferEvent& event)
{
	invalidate_buffer(event.get_name());
}

void chomsky::WorldUpdateManager::on_draw_enabled(float timestamp)
{
	assert(!this->is_updating);
	this->is_updating = true;

	this->terrains.clear_invalid();
	this->models.clear_invalid();
	this->waters.clear_invalid();
	this->particles.clear_invalid();
}

void chomsky::WorldUpdateManager::on_draw_disabled(float timestamp)
{
	assert(this->is_updating);
	this->is_updating = false;
}

void chomsky::WorldUpdateManager::on_draw_model(
	const kompvter::ModelDrawEvent& event)
{
	if (!this->is_updating)
	{
		return;
	}

	int model_buffer;
	{
		auto profile = kvre::Profile::get_default();
		auto& attributes = event.get_vertex_attributes();
		auto fetcher = profile->get_model_vertex_positions_fetcher(attributes);
		assert(fetcher != nullptr);
		auto& buffer = fetcher->get_buffer();
		model_buffer = buffer.get_name();
	}

	kvre::Model* model = nullptr;
	if (this->models.has_resource_with_buffer(model_buffer))
	{
		model = this->models.get_resource_with_buffer(model_buffer);
	}
	
	if (model == nullptr)
	{
		model = new kvre::Model(event.get_vertex_attributes());
		this->models.add_resource(model);
		this->models.attach_buffer_to_resource(model_buffer, model);
	}

	this->model_draws.emplace_back(event, model);
}

void chomsky::WorldUpdateManager::on_draw_terrain(
	const kompvter::ModelDrawEvent& event)
{
	if (!this->is_updating)
	{
		return;
	}

	int terrain_buffer;
	{
		auto profile = kvre::Profile::get_default();
		auto& attributes = event.get_vertex_attributes();
		auto fetcher = profile->get_terrain_vertex_positions_fetcher(attributes);
		assert(fetcher != nullptr);
		auto& buffer = fetcher->get_buffer();
		terrain_buffer = buffer.get_name();
	}

	kvre::Terrain* terrain;
	if (this->terrains.has_resource_with_buffer(terrain_buffer))
	{
		terrain = this->terrains.get_resource_with_buffer(terrain_buffer);
	}
	else
	{
		terrain = new kvre::Terrain(event.get_vertex_attributes());
		this->terrains.add_resource(terrain);
		this->terrains.attach_buffer_to_resource(terrain_buffer, terrain);
	}

	this->terrain_draws.emplace_back(event, terrain);
}

void chomsky::WorldUpdateManager::on_draw_water(
	const kompvter::ModelDrawEvent& event)
{
	if (!this->is_updating)
	{
		return;
	}

	int water_buffer;
	{
		auto profile = kvre::Profile::get_default();
		auto& attributes = event.get_vertex_attributes();
		auto fetcher = profile->get_terrain_vertex_positions_fetcher(attributes);
		assert(fetcher != nullptr);
		auto& buffer = fetcher->get_buffer();
		water_buffer = buffer.get_name();
	}

	kvre::Terrain* water;
	if (this->waters.has_resource_with_buffer(water_buffer))
	{
		water = this->waters.get_resource_with_buffer(water_buffer);
	}
	else
	{
		water = new kvre::Terrain(event.get_vertex_attributes());
		this->waters.add_resource(water);
		this->waters.attach_buffer_to_resource(water_buffer, water);
	}

	this->water_draws.emplace_back(event, water);
}

void chomsky::WorldUpdateManager::on_draw_particle(
	const kompvter::ModelDrawEvent& event)
{
	if (!this->is_updating)
	{
		return;
	}

	int particle_buffer;
	{
		auto profile = kvre::Profile::get_default();
		auto& attributes = event.get_vertex_attributes();
		auto fetcher = profile->get_particle_vertex_positions_fetcher(attributes);
		assert(fetcher != nullptr);
		auto& buffer = fetcher->get_buffer();
		particle_buffer = buffer.get_name();
	}

	kvre::Particles* particle;
	if (this->particles.has_resource_with_buffer(particle_buffer))
	{
		particle = this->particles.get_resource_with_buffer(particle_buffer);
	}
	else
	{
		particle = new kvre::Particles(event.get_vertex_attributes());
		this->particles.add_resource(particle);
		this->particles.attach_buffer_to_resource(particle_buffer, particle);
	}

	this->particle_draws.emplace_back(event, particle);
}

void chomsky::WorldUpdateManager::after_draw_terrain(
	WorldUpdatePass* pass,
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	// Nothing.
}

void chomsky::WorldUpdateManager::after_draw_model(
	WorldUpdatePass* pass,
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	// Nothing.
}

void chomsky::WorldUpdateManager::after_draw_water(
	WorldUpdatePass* pass,
	int tag, const kvre::Terrain* water,
	const kompvter::ModelDrawEvent& event)
{
	// Nothing.
}

void chomsky::WorldUpdateManager::after_draw_particles(
	WorldUpdatePass* pass,
	int tag, const kvre::Particles* particles,
	const kompvter::ModelDrawEvent& event)
{
	// Nothing.
}

void chomsky::WorldUpdateManager::after_start(WorldUpdatePass* pass)
{
	// Nothing.
}

void chomsky::WorldUpdateManager::after_stop(WorldUpdatePass* pass)
{
	// Nothing.
}

void chomsky::WorldUpdateManager::invalidate_buffer(int buffer)
{
	if (this->terrains.has_resource_with_buffer(buffer))
	{
		auto resource = this->terrains.get_resource_with_buffer(buffer);
		for (auto pass: this->passes)
		{
			pass->invalidate_terrain(resource);
		}

		this->terrains.invalidate_resource_with_buffer(buffer);
	}

	if (this->models.has_resource_with_buffer(buffer))
	{
		auto resource = this->models.get_resource_with_buffer(buffer);
		for (auto pass: this->passes)
		{
			pass->invalidate_model(resource);
		}

		this->models.invalidate_resource_with_buffer(buffer);
	}

	if (this->waters.has_resource_with_buffer(buffer))
	{
		auto resource = this->waters.get_resource_with_buffer(buffer);
		for (auto pass: this->passes)
		{
			pass->invalidate_water(resource);
		}

		this->waters.invalidate_resource_with_buffer(buffer);
	}

	if (this->particles.has_resource_with_buffer(buffer))
	{
		auto resource = this->particles.get_resource_with_buffer(buffer);
		for (auto pass: this->passes)
		{
			pass->invalidate_particles(resource);
		}

		this->particles.invalidate_resource_with_buffer(buffer);
	}
}
