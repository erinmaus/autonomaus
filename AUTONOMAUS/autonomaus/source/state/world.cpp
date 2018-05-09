// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <algorithm>
#include <cmath>
#include "autonomaus/state/world.hpp"

int autonomaus::World::get_draw_type(int tag) const
{
	auto iter = this->draw_types.find(tag);
	if (iter != this->draw_types.end())
	{
		return iter->second;
	}

	return draw_type_none;
}

const autonomaus::World::AnimatedState&
autonomaus::World::get_animated_state() const
{
	return this->animated_state;
}

autonomaus::World::Metric autonomaus::World::get_second_metric() const
{
	return this->second_metric;
}

autonomaus::World::Metric autonomaus::World::get_minute_metric() const
{
	return this->minute_metric;
}

autonomaus::World::Metric autonomaus::World::get_hour_metric() const
{
	return this->hour_metric;
}

void autonomaus::World::update(float timestamp, float frame_time)
{
	if (this->last_second_update == HUGE_VALF)
	{
		this->last_second_update = timestamp;
	}
	else
	{
		float difference = timestamp - this->last_second_update;
		if (difference > 1.0f)
		{
			bubble_second(timestamp);
			this->second_updates.clear();
			this->last_second_update = timestamp;
		}

	}

	this->second_updates.push_back(frame_time * 1000.0f);
}

void autonomaus::World::bubble_second(float timestamp)
{
	auto values = this->second_updates;
	std::sort(values.begin(), values.end());

	Metric metric;
	metric.timestamp = timestamp;
	metric.valid = true;
	metric.count = (int)values.size();
	if (values.size() > 0)
	{
		metric.min = values.front();
		metric.max = values.back();
		if (values.size() == 1)
		{
			metric.median = metric.min;
		}
		else if (values.size() % 2 == 0)
		{
			int middle = values.size() / 2;
			metric.median = (values[middle - 1] + values[middle]) / 2.0f;
		}
		else
		{
			metric.median = values[values.size() / 2];
		}
	}

	if (this->last_minute_update == HUGE_VALF)
	{
		this->last_minute_update = timestamp;
	}
	else
	{
		float difference = timestamp - this->last_minute_update;
		if (difference > 60.0f)
		{
			bubble_minute(timestamp);
			this->minute_updates.clear();

			this->last_minute_update = timestamp;
		}
	}

	minute_updates.push_back(metric);
	this->second_metric = metric;
}

void autonomaus::World::bubble_minute(float timestamp)
{
	Metric metric = compute_metric(this->minute_updates);
	metric.timestamp = timestamp;

	if (this->last_hour_update == HUGE_VALF)
	{
		this->last_hour_update = timestamp;
	}
	else
	{
		float difference = timestamp - this->last_hour_update;
		if (difference > 60.0f * 60.0f)
		{
			bubble_hour(timestamp);
			this->minute_updates.clear();
			this->last_hour_update = timestamp;
		}
	}

	this->minute_updates.push_back(metric);
	this->minute_metric = metric;
}

void autonomaus::World::bubble_hour(float timestamp)
{
	this->hour_metric = compute_metric(this->minute_updates);
	this->hour_metric.timestamp = timestamp;
}

autonomaus::World::Metric autonomaus::World::compute_metric(std::vector<Metric> values)
{
	std::sort(
		values.begin(), values.end(),
		[](auto& a, auto&b) { return a.median < b.median; });

	Metric metric;
	metric.valid = true;
	if (values.size() > 0)
	{
		metric.count = 0;
		for (auto i: values)
		{
			metric.count += i.count;
		}

		metric.min = HUGE_VALF;
		for (auto i: values)
		{
			metric.min = std::min(i.min, metric.min);
		}

		metric.max = -HUGE_VALF;
		for (auto i: values)
		{
			metric.max = std::max(i.max, metric.max);
		}

		if (values.size() == 1)
		{
			metric.median = values.front().median;
		}
		else if (values.size() % 2 == 0)
		{
			int middle = values.size() / 2;
			float lower = values[middle - 1].median;
			float upper = values[middle - 0].median;
			metric.median = (lower + upper) / 2.0f;
		}
		else
		{
			metric.median = values[values.size() / 2].median;
		}
	}

	return metric;
}
