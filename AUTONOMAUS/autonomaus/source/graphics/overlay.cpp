// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "autonomaus/graphics/overlay.hpp"

const float autonomaus::Overlay::DEFAULT_LIFESPAN = 1.0f;
const int autonomaus::Overlay::TAG_ID_NONE = 0;
const int autonomaus::Overlay::TAG_ID_BASE = 1;

autonomaus::Overlay::Overlay(Renderer& renderer, const DrawState& draw_state)
{
	this->renderer = &renderer;
	this->draw_state = &draw_state;
}

float autonomaus::Overlay::get_fade_duration() const
{
	return this->fade_duration;
}

void autonomaus::Overlay::set_fade_duration(float value)
{
	this->fade_duration = value;
}

autonomaus::Overlay::Tag autonomaus::Overlay::allocate()
{
	int tag_id = next_tag_id++;
	Tag tag(this, tag_id);

	this->current_tags.insert(tag_id);
	return tag;
}

void autonomaus::Overlay::free(Tag& tag)
{
	if (tag.data == nullptr)
	{
		return;
	}

	if (tag.data->overlay != this)
	{
		return;
	}

	auto iter = this->current_tags.find(tag.data->id);
	if (iter != this->current_tags.end())
	{
		this->current_tags.erase(iter);
		tag.data->overlay = nullptr;
		tag.data->id = TAG_ID_NONE;
	}
}

bool autonomaus::Overlay::start(Tag& tag, bool clear_if_exists, float lifespan)
{
	assert(this->current_tag_id == TAG_ID_NONE);
	if (!tag.valid())
	{
		tag = allocate();
	}

	assert(this->current_tags.count(tag.data->id) != 0);
	this->current_tag_id = tag.data->id;

	auto iter = this->draws.find(tag.data->id);
	if (iter == this->draws.end())
	{
		auto result = this->draws.emplace(tag.data->id, Draw()).first;
		result->second.lifespan = lifespan + get_fade_duration();
	}
	else
	{
		if (iter->second.age >= iter->second.lifespan)
		{
			float relative_lifespan = iter->second.age - iter->second.lifespan;
			relative_lifespan /= get_fade_duration();
			iter->second.age = (1.0f - relative_lifespan) * lifespan;
		}
		else if (iter->second.age >= get_fade_duration())
		{
			iter->second.age = get_fade_duration();
		}
	}

	if (clear_if_exists)
	{
		clear();
	}

	return iter == this->draws.end();
}

void autonomaus::Overlay::clear()
{
	assert(this->current_tag_id != TAG_ID_NONE);

	auto iter = this->draws.find(this->current_tag_id);
	assert(iter != this->draws.end());

	iter->second.objects.clear();
}

void autonomaus::Overlay::stop()
{
	assert(this->current_tag_id != TAG_ID_NONE);
	this->current_tag_id = TAG_ID_NONE;
}

void autonomaus::Overlay::draw(float time)
{
	float duration;
	if (this->last_draw == HUGE_VALF)
	{
		duration = 0.0f;
	}
	else
	{
		duration = time - this->last_draw;
	}
	this->last_draw = time;

	for (auto i = this->draws.begin(); i != this->draws.end(); /* Nop. */)
	{
		i->second.age += duration;

		if (i->second.age >= i->second.lifespan + this->fade_duration)
		{
			i = this->draws.erase(i);
		}
		else
		{
			start_draw(i->first, i->second.age, i->second.lifespan);
			for (auto& object: i->second.objects)
			{
				object->draw();
			}
			stop_draw();

			++i;
		}
	}
}

const autonomaus::DrawState& autonomaus::Overlay::get_draw_state() const
{
	return *this->draw_state;
}

void autonomaus::Overlay::add_draw(DrawObject* object)
{
	assert(this->current_tag_id != TAG_ID_NONE);

	auto iter = this->draws.find(this->current_tag_id);
	assert(iter != this->draws.end());
	iter->second.objects.emplace_back(object);
}

autonomaus::Renderer& autonomaus::Overlay::get_renderer() const
{
	return *this->renderer;
}

autonomaus::Overlay::Tag::Tag(const Tag& other)
{
	this->data = other.data;
	ref();
}

autonomaus::Overlay::Tag::~Tag()
{
	unref();
}

bool autonomaus::Overlay::Tag::valid() const
{
	return !(this->data == nullptr ||
		this->data->overlay == nullptr ||
		this->data->id == TAG_ID_NONE);
}

autonomaus::Overlay::Tag& autonomaus::Overlay::Tag::operator =(const Tag& other)
{
	unref();

	this->data = other.data;
	ref();

	return *this;
}

autonomaus::Overlay::Tag::Tag(Overlay* overlay, int id)
{
	this->data = new Data();
	this->data->overlay = overlay;
	this->data->id = id;
}

void autonomaus::Overlay::Tag::ref()
{
	if (this->data != nullptr)
	{
		++this->data->count;
	}
}

void autonomaus::Overlay::Tag::unref()
{
	if (this->data != nullptr)
	{
		--this->data->count;
		if (this->data->count == 0)
		{
			if (this->data->overlay != nullptr)
			{
				this->data->overlay->free(*this);
			}

			delete this->data;
			this->data = nullptr;
		}
	}
}
