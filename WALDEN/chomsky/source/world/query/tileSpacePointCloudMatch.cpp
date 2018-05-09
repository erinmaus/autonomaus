// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <algorithm>
#include "chomsky/world/query/tileSpacePointCloudMatch.hpp"

bool chomsky::TileSpacePointCloudMatch::has(const thoreau::TileCoordinate& offset) const
{
	return this->clouds.count(offset) != 0;
}

const chomsky::TileSpacePointCloud& chomsky::TileSpacePointCloudMatch::get(
	const thoreau::TileCoordinate& offset) const
{
	assert(has(offset));
	return this->clouds.find(offset)->second;
}

void chomsky::TileSpacePointCloudMatch::set(
	const thoreau::TileCoordinate& offset, const TileSpacePointCloud& value)
{
	this->clouds[offset] = value;

	this->min_x = std::min(this->min_x, offset.x);
	this->min_y = std::min(this->min_y, offset.y);
	this->min_layer = std::min(this->min_layer, offset.layer);

	this->max_x = std::max(this->max_x, offset.x);
	this->max_y = std::max(this->max_y, offset.y);
	this->max_layer = std::max(this->max_layer, offset.layer);
}

int chomsky::TileSpacePointCloudMatch::get_width() const
{
	return this->max_x - this->min_x;
}

int chomsky::TileSpacePointCloudMatch::get_height() const
{
	return this->max_layer - this->min_layer;
}

int chomsky::TileSpacePointCloudMatch::get_depth() const
{
	return this->max_y - this->min_y;
}

bool chomsky::TileSpacePointCloudMatch::empty() const
{
	return this->clouds.empty();
}

chomsky::TileSpacePointCloudMatch::iterator
chomsky::TileSpacePointCloudMatch::begin()
{
	return this->clouds.begin();
}

chomsky::TileSpacePointCloudMatch::const_iterator
chomsky::TileSpacePointCloudMatch::begin() const
{
	return this->clouds.begin();
}

chomsky::TileSpacePointCloudMatch::iterator
chomsky::TileSpacePointCloudMatch::end()
{
	return this->clouds.end();
}


chomsky::TileSpacePointCloudMatch::const_iterator
chomsky::TileSpacePointCloudMatch::end() const
{
	return this->clouds.end();
}