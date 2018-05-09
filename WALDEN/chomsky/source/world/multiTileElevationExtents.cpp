// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cmath>
#include "chomsky/world/multiTileElevationExtents.hpp"

const int chomsky::MultiTileElevationExtents::MIN_Y;
const int chomsky::MultiTileElevationExtents::MAX_Y;
const int chomsky::MultiTileElevationExtents::TAG;

void chomsky::MultiTileElevationExtents::test(int tag, float y)
{
	this->extents[tag].insert(y);
}

std::size_t chomsky::MultiTileElevationExtents::count() const
{
	return this->extents.size();
}

std::size_t chomsky::MultiTileElevationExtents::count(int tag) const
{
	auto iter = this->extents.find(tag);
	if (iter != this->extents.end())
	{
		return iter->second.size();
	}

	return 0;
}

bool chomsky::MultiTileElevationExtents::has(int tag) const
{
	return this->extents.count(tag) != 0;
}

float chomsky::MultiTileElevationExtents::get_min(int tag) const
{
	auto iter = this->extents.find(tag);
	if (iter == this->extents.end())
	{
		return HUGE_VALF;
	}

	return *iter->second.begin();
}

float chomsky::MultiTileElevationExtents::get_min_above(int tag, float threshold) const
{
	auto iter = this->extents.find(tag);
	if (iter != this->extents.end())
	{
		for (auto i: iter->second)
		{
			if (i > threshold)
			{
				return i;
			}
		}
	}

	return HUGE_VALF;
}

float chomsky::MultiTileElevationExtents::get_max(int tag) const
{
	auto iter = this->extents.find(tag);
	if (iter == this->extents.end())
	{
		return HUGE_VALF;
	}

	return *iter->second.rbegin();
}

float chomsky::MultiTileElevationExtents::get_max_below(int tag, float threshold) const
{
	auto iter = this->extents.find(tag);
	if (iter != this->extents.end())
	{
		for (auto i = iter->second.rbegin(); i != iter->second.rend(); ++i)
		{
			if (*i < threshold)
			{
				return *i;
			}
		}
	}

	return HUGE_VALF;
}

bool chomsky::MultiTileElevationExtents::try_find_tag(float y, int& result) const
{
	if (this->extents.empty())
	{
		return false;
	}

	int best_tag = 0;
	float best_difference = HUGE_VALF;
	for (auto& i: this->extents)
	{
		auto& extent = i.second;

		float difference = y - *extent.begin();
		if (difference < best_difference && difference >= 0.0f)
		{
			best_tag = i.first;
			best_difference = difference;
		}
	}

	if (best_difference == HUGE_VALF)
	{
		return false;
	}

	result = best_tag;
	return true;
}

void chomsky::MultiTileElevationExtents::remove_tag(int tag)
{
	this->extents.erase(tag);
}

void chomsky::MultiTileElevationExtents::clear()
{
	this->extents.clear();
}

chomsky::MultiTileElevationExtents::const_iterator::const_iterator(
	const ExtentCollection::const_iterator& source)
	: ExtentCollection::const_iterator(source)
{
	// Nil.
}

chomsky::MultiTileElevationExtents::const_iterator::value_type
chomsky::MultiTileElevationExtents::const_iterator::operator *() const
{
	auto& value = ExtentCollection::const_iterator::operator *();
	return std::make_tuple(
		*value.second.begin(), *value.second.rbegin(), value.first);
}

chomsky::MultiTileElevationExtents::const_iterator
chomsky::MultiTileElevationExtents::begin() const
{
	return this->extents.begin();
}

chomsky::MultiTileElevationExtents::const_iterator
chomsky::MultiTileElevationExtents::end() const
{
	return this->extents.end();
}
