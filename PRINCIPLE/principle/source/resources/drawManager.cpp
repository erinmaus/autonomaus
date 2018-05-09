// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/detail/actor.hpp"
#include "chomsky/world/detail/prop.hpp"
#include "chomsky/world/detail/scenery.hpp"
#include "principle/resources/drawManager.hpp"

bool principle::DrawManager::has(int tag) const
{
	return this->tags.find(tag) != this->tags.end();
}

const principle::Draw& principle::DrawManager::get(int tag) const
{
	auto iter = this->tags.find(tag);
	assert(iter != this->tags.end());

	return iter->second;
}

std::size_t principle::DrawManager::count() const
{
	return this->draws.size();
}

int principle::DrawManager::get_draw_call_tag(std::size_t call_num) const
{
	assert(call_num < this->draws.size());
	return this->draws.at(call_num);
}

const principle::Draw& principle::DrawManager::get_draw_call(
	std::size_t call_num) const
{
	int tag = get_draw_call_tag(call_num);
	assert(has(tag));

	return get(tag);
}

void principle::DrawManager::start()
{
	reset();
}

void principle::DrawManager::reset()
{
	this->current_call = 0;
	this->draws.clear();
	this->tags.clear();
}

void principle::DrawManager::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	auto call_num = this->current_call++;

	int draw_type;
	if (chomsky::is_scenery_draw_event(event))
	{
		draw_type = Draw::draw_type_scenery;
	}
	else if (chomsky::is_prop_draw_event(event))
	{
		draw_type = Draw::draw_type_prop;
	}
	else if (chomsky::is_actor_draw_event(event))
	{
		draw_type = Draw::draw_type_actor;
	}
	else
	{
		draw_type = Draw::draw_type_unclassified;
	}

	Draw draw(call_num, tag, draw_type, event);

	this->tags.emplace(tag, draw);
	this->draws.push_back(tag);
	assert(this->draws.size() == this->current_call);
}

void principle::DrawManager::draw_terrain(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	auto call_num = this->current_call++;

	Draw draw(call_num, tag, Draw::draw_type_terrain, event);

	this->tags.emplace(tag, draw);
	this->draws.push_back(tag);
	assert(this->draws.size() == this->current_call);
}

void principle::DrawManager::draw_water(
	int tag, const kvre::Terrain* water,
	const kompvter::ModelDrawEvent& event)
{
	auto call_num = this->current_call++;

	Draw draw(call_num, tag, Draw::draw_type_water, event);

	this->tags.emplace(tag, draw);
	this->draws.push_back(tag);
	assert(this->draws.size() == this->current_call);
}

void principle::DrawManager::draw_particles(
	int tag, const kvre::Particles* particles,
	const kompvter::ModelDrawEvent& event)
{
	auto call_num = this->current_call++;

	Draw draw(call_num, tag, Draw::draw_type_particles, event);

	this->tags.emplace(tag, draw);
	this->draws.push_back(tag);
	assert(this->draws.size() == this->current_call);
}

void principle::DrawManager::invalidate_tag(int tag)
{
	this->tags.erase(tag);

	for (auto i = this->draws.begin(); i != this->draws.end(); /* No. */)
	{
		if (*i == tag)
		{
			i = this->draws.erase(i);
		}
		else
		{
			++i;
		}
	}
}

principle::DrawManager::const_iterator principle::DrawManager::begin() const
{
	return this->tags.begin();
}

principle::DrawManager::const_iterator principle::DrawManager::end() const
{
	return this->tags.end();
}
