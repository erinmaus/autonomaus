// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <boost/functional/hash.hpp>
#include "chomsky/world/worldUpdateManager.hpp"
#include "chomsky/world/detail/tagManager.hpp"
#include "kvre/profile/profile.hpp"

void chomsky::TagManagerDrawInvocation::from_draw_model(
	const kompvter::ModelDrawEvent& event)
{
	static auto profile = kvre::Profile::get_default();

	auto fetcher = profile->get_model_vertex_positions_fetcher(event.get_vertex_attributes());
	assert(fetcher != nullptr);

	this->type = draw_event_model;
	this->position_buffer_name = fetcher->get_buffer().get_name();
	this->index_buffer_name = event.get_vertex_index_mapper().get_buffer().get_name();
	this->index_buffer_offset_start = event.get_draw_parameters().get_offset_start();
	this->index_buffer_offset_stop = event.get_draw_parameters().get_offset_stop();
	this->world_position = glm::ivec3(event.get_model_matrix() * glm::vec4(glm::vec3(0.0f), 1.0f)); 
}

void chomsky::TagManagerDrawInvocation::from_draw_terrain(
	const kompvter::ModelDrawEvent& event)
{
	static auto profile = kvre::Profile::get_default();

	auto fetcher = profile->get_terrain_vertex_positions_fetcher(event.get_vertex_attributes());
	assert(fetcher != nullptr);

	this->type = draw_event_terrain;
	this->position_buffer_name = fetcher->get_buffer().get_name();
	this->index_buffer_name = 0;
	this->index_buffer_offset_start = event.get_draw_parameters().get_offset_start();
	this->index_buffer_offset_stop = event.get_draw_parameters().get_offset_stop();
	this->world_position = glm::ivec3(event.get_model_matrix() * glm::vec4(glm::vec3(0.0f), 1.0f)); 
}

void chomsky::TagManagerDrawInvocation::from_draw_water(
	const kompvter::ModelDrawEvent& event)
{
	static auto profile = kvre::Profile::get_default();

	auto fetcher = profile->get_terrain_vertex_positions_fetcher(event.get_vertex_attributes());
	assert(fetcher != nullptr);

	this->type = draw_event_water;
	this->position_buffer_name = fetcher->get_buffer().get_name();
	this->index_buffer_name = event.get_vertex_index_mapper().get_buffer().get_name();
	this->index_buffer_offset_start = event.get_draw_parameters().get_offset_start();
	this->index_buffer_offset_stop = event.get_draw_parameters().get_offset_stop();
	this->world_position = glm::ivec3(event.get_model_matrix() * glm::vec4(glm::vec3(0.0f), 1.0f)); 
}

void chomsky::TagManagerDrawInvocation::from_draw_particle(
	const kompvter::ModelDrawEvent& event)
{
	static auto profile = kvre::Profile::get_default();

	auto fetcher = profile->get_particle_vertex_positions_fetcher(event.get_vertex_attributes());
	assert(fetcher != nullptr);

	this->type = draw_event_particle;
	this->position_buffer_name = fetcher->get_buffer().get_name();
	this->index_buffer_name = event.get_vertex_index_mapper().get_buffer().get_name();
	this->index_buffer_offset_start = event.get_draw_parameters().get_offset_start();
	this->index_buffer_offset_stop = event.get_draw_parameters().get_offset_stop();
	this->world_position = glm::ivec3(event.get_model_matrix() * glm::vec4(glm::vec3(0.0f), 1.0f)); 
}

std::size_t chomsky::TagManagerDrawInvocation::hash() const
{
	std::size_t result = 0;
	boost::hash_combine(result, this->type);
	boost::hash_combine(result, this->position_buffer_name);
	boost::hash_combine(result, this->index_buffer_name);
	boost::hash_combine(result, this->index_buffer_offset_start);
	boost::hash_combine(result, this->index_buffer_offset_stop);
	boost::hash_combine(result, this->world_position.x);
	boost::hash_combine(result, this->world_position.y);
	boost::hash_combine(result, this->world_position.z);

	return result;
}

bool chomsky::TagManagerDrawInvocation::operator ==(
	const TagManagerDrawInvocation& other) const
{
	return this->type == other.type &&
		this->position_buffer_name == other.position_buffer_name &&
		this->index_buffer_name == other.index_buffer_name &&
		this->index_buffer_offset_start == other.index_buffer_offset_start &&
		this->index_buffer_offset_stop == other.index_buffer_offset_stop &&
		this->world_position == other.world_position;
}

int chomsky::TagManager::tag_model(const kompvter::ModelDrawEvent& event)
{
	TagManagerDrawInvocation invocation;
	invocation.from_draw_model(event);

	return get_tag(invocation);
}

int chomsky::TagManager::tag_terrain(
	const kompvter::ModelDrawEvent& event)
{
	TagManagerDrawInvocation invocation;
	invocation.from_draw_terrain(event);

	return get_tag(invocation);
}

int chomsky::TagManager::tag_water(
	const kompvter::ModelDrawEvent& event)
{
	TagManagerDrawInvocation invocation;
	invocation.from_draw_water(event);

	return get_tag(invocation);
}

int chomsky::TagManager::tag_particle(
	const kompvter::ModelDrawEvent& event)
{
	TagManagerDrawInvocation invocation;
	invocation.from_draw_particle(event);

	return get_tag(invocation);
}

int chomsky::TagManager::get_tag(const TagManagerDrawInvocation& invocation)
{
	int result;

	auto iter = this->tags.find(invocation);
	if (iter == this->tags.end())
	{
		if (this->free.empty())
		{
			result = next++;
		}
		else
		{
			result = this->free.front();
			this->free.pop();
		}

		this->used.emplace(result, invocation);
		this->tags.emplace(invocation, result);
	}
	else
	{
		result = iter->second;
	}

	this->active.insert(result);
	return result;
}

void chomsky::TagManager::for_each_invalid_tag(
	std::function<void(int)> on_invalidate)
{
	for (auto i = this->used.begin(); i != this->used.end(); /* Nil. */ )
	{
		auto tag = i->first;

		if (this->active.find(tag) == this->active.end())
		{
			on_invalidate(tag);

			assert(this->tags.find(i->second) != this->tags.end());
			this->tags.erase(i->second);
			this->free.push(tag);

			i = this->used.erase(i);
		}
		else
		{
			++i;
		}
	}

	this->active.clear();
}

void chomsky::TagManager::invalidate_all()
{
	this->active.clear();
}
