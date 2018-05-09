// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cmath>
#include <algorithm>
#include "chomsky/world/tileElevationExtents.hpp"

const int chomsky::TileElevationExtents::MIN_Y;
const int chomsky::TileElevationExtents::MAX_Y;
const int chomsky::TileElevationExtents::TAG;

void chomsky::TileElevationExtents::test(int tag, float y)
{
	auto iter = this->extents.find(tag);
	if (iter == this->extents.end())
	{
		this->extents.emplace(tag, std::make_pair(y, y));
	}
	else
	{
		iter->second.first = std::min(iter->second.first, y);
		iter->second.second = std::max(iter->second.second, y);
	}
}

std::size_t chomsky::TileElevationExtents::count() const
{
	return this->extents.size();
}

std::size_t chomsky::TileElevationExtents::count(int tag) const
{
	auto iter = this->extents.find(tag);
	if (iter != this->extents.end())
	{
		return 2;
	}

	return 0;
}

bool chomsky::TileElevationExtents::has(int tag) const
{
	return this->extents.count(tag) != 0;
}

float chomsky::TileElevationExtents::get_min(int tag) const
{
	auto iter = this->extents.find(tag);
	if (iter == this->extents.end())
	{
		return HUGE_VALF;
	}

	return iter->second.first;
}

float chomsky::TileElevationExtents::get_max(int tag) const
{
	auto iter = this->extents.find(tag);
	if (iter == this->extents.end())
	{
		return HUGE_VALF;
	}

	return iter->second.second;
}

bool chomsky::TileElevationExtents::try_find_tag(float y, int& result) const
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

		float difference = y - extent.first;
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

void chomsky::TileElevationExtents::remove_tag(int tag)
{
	this->extents.erase(tag);
}

void chomsky::TileElevationExtents::clear()
{
	this->extents.clear();
}

chomsky::TileElevationExtents::const_iterator::const_iterator(
	const ExtentCollection::const_iterator& source)
	: ExtentCollection::const_iterator(source)
{
	// Nil.
}

chomsky::TileElevationExtents::const_iterator::value_type
chomsky::TileElevationExtents::const_iterator::operator *() const
{
	auto& value = ExtentCollection::const_iterator::operator *();
	return std::make_tuple(
		value.second.first, value.second.second, value.first);
}

chomsky::TileElevationExtents::const_iterator
chomsky::TileElevationExtents::begin() const
{
	return this->extents.begin();
}

chomsky::TileElevationExtents::const_iterator
chomsky::TileElevationExtents::end() const
{
	return this->extents.end();
}
