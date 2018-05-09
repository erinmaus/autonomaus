// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <algorithm>
#include "autonomaus/input/mousePathFilter.hpp"

const eiznebahn::MousePath autonomaus::MousePathFilter::EMPTY = eiznebahn::MousePath();

void autonomaus::MousePathFilter::clear()
{
	this->paths.clear();
	this->current = 0;
	this->limit = 0;
}

void autonomaus::MousePathFilter::start(std::size_t min)
{
	this->current = this->paths.size();
	this->limit = min;
}

bool autonomaus::MousePathFilter::stop()
{
	return this->current > 0;
}

bool autonomaus::MousePathFilter::select(std::size_t count)
{
	if (count < this->limit)
	{
		return false;
	}

	this->current = std::min(count, this->paths.size());

	return true;
}

std::size_t autonomaus::MousePathFilter::direction(float value, float threshold)
{
	return filter([](auto& p) { return p.get_direction(); }, value, threshold);
}

std::size_t autonomaus::MousePathFilter::length(float value, float threshold)
{
	return filter([](auto& p) { return p.get_length(); }, value, threshold);
}

std::size_t autonomaus::MousePathFilter::drift(float value, float threshold)
{
	return filter([](auto& p) { return p.get_drift() / p.get_length(); }, value, threshold);
}

std::size_t autonomaus::MousePathFilter::duration(float value)
{
	return filter([](auto& p) { return p.get_duration(); }, value, HUGE_VALF);

}

std::size_t autonomaus::MousePathFilter::count() const
{
	return this->current;
}

const eiznebahn::MousePath& autonomaus::MousePathFilter::get(
	std::size_t index) const
{
	if (index < this->current)
	{
		return *this->paths.at(index);
	}

	return EMPTY;
}

std::size_t autonomaus::MousePathFilter::filter(
	const std::function<float(const eiznebahn::MousePath& path)>& get,
	float value, float threshold)
{
	std::sort(
		this->paths.begin(), this->paths.begin() + this->current,
		[&get, value](auto a, auto b)
		{
			float i = std::fabs(get(*a) - value);
			float j = std::fabs(get(*b) - value);
			return i < j;
		});

	if (threshold == HUGE_VALF)
	{
		return this->current;
	}

	std::size_t count = 0;
	for (std::size_t i = 0; i < this->paths.size(); ++i)
	{
		float difference = std::fabs(get(*paths[i]) - value);
		if (difference < threshold)
		{
			++count;
		}
		else
		{
			break;
		}
	}

	return count;
}
