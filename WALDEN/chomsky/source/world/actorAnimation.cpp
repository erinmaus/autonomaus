// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <set>
#include <tuple>
#include "chomsky/world/actorAnimation.hpp"

float chomsky::ActorAnimation::get_hit_ratio_threshold() const
{
	return this->hits;
}

void chomsky::ActorAnimation::set_hit_ratio_threshold(float value)
{
	this->hits = value;
}

float chomsky::ActorAnimation::get_distance_threshold() const
{
	return this->distance;
}

void chomsky::ActorAnimation::set_distance_threshold(float value)
{
	this->distance = value;
}

void chomsky::ActorAnimation::extract(const chomsky::ActorDraw& draw)
{
	Frame frame;
	for (std::size_t i = 0; i < draw.get_num_bones(); ++i)
	{
		auto m = draw.get_bone(i);
		if (m != glm::mat4(1.0f))
		{
			glm::vec4 t(0, 0, 0, 1);
			t = m * t;

			frame.emplace_back(t.x, t.y, t.z);
		}
	}

	this->frames.emplace_back(std::move(frame));
}

bool chomsky::ActorAnimation::match(const ActorAnimation& other) const
{
	for (auto& i: this->frames)
	{
		for (auto& j: other.frames)
		{
			if (match(i, j))
			{
				return true;
			}
		}
	}

	return false;
}

std::size_t chomsky::ActorAnimation::get_num_frames() const
{
	return this->frames.size();
}

std::size_t chomsky::ActorAnimation::clean()
{
	std::size_t count = 0;

	for (auto i = this->frames.begin(); i != this->frames.end(); /* Nop. */)
	{
		bool same = false;
		for (auto j = i + 1; j != this->frames.end(); ++j)
		{
			if (match(*i, *j))
			{
				same = true;
				break;
			}
		}

		if (same)
		{
			++count;
			i = this->frames.erase(i);
		}
		else
		{
			++i;
		}
	}

	return count;
}

void chomsky::ActorAnimation::clear()
{
	this->frames.clear();
}

bool chomsky::ActorAnimation::match(
	const Frame& query,
	const Frame& frame) const
{
	int num_matches = 0;
	for (auto i: query)
	{
		for (auto j: frame)
		{
			if (glm::distance(i, j) < this->distance)
			{
				++num_matches;
			}
		}
	}

	float ratio = ((float)num_matches) / (query.size() + frame.size());
	return ratio >= this->hits;
}
