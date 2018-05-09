// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cmath>
#include <cstdlib>
#include "thoreau/tile.hpp"

float thoreau::Tile::get_elevation() const
{
	return (this->min_elevation + this->max_elevation) / 2.0f;
}

float thoreau::Tile::get_min_elevation() const
{
	return this->min_elevation;
}

float thoreau::Tile::get_max_elevation() const
{
	return this->max_elevation;
}

void thoreau::Tile::set_elevation(float min_value, float max_value)
{
	this->min_elevation = min_value;
	this->max_elevation = max_value;
}

bool thoreau::Tile::has_elevation() const
{
	return this->min_elevation != -HUGE_VALF && this->max_elevation != HUGE_VALF;
}

float thoreau::Tile::get_height() const
{
	return this->height;
}

void thoreau::Tile::set_height(float value)
{
	this->height = value;
}

int thoreau::Tile::get_flags() const
{
	return this->flags;
}

void thoreau::Tile::set_flags(int value)
{
	this->flags = value;
}

void thoreau::Tile::append_flags(int value)
{
	this->flags |= value;
}

void thoreau::Tile::unset_flags(int value)
{
	this->flags &= ~value;
}

void thoreau::Tile::mark_seen()
{
	append_flags(flag_seen);
}

bool thoreau::Tile::is_wall() const
{
	return this->flags & flag_wall;
}

bool thoreau::Tile::is_impassable() const
{
	return !is_passable();
}

bool thoreau::Tile::is_passable() const
{
	if (!has_elevation())
	{
		return false;
	}

	if (this->flags & flag_portal)
	{
		return true;
	}

	if (this->flags & flag_passable)
	{
		return true;
	}

	if (this->flags & flag_water || this->flags & flag_impassable)
	{
		return false;
	}

	return true;
}

bool thoreau::Tile::is_door() const
{
	return this->flags & flag_door;
}

bool thoreau::Tile::is_portal() const
{
	return this->flags & flag_portal;
}

bool thoreau::Tile::was_seen() const
{
	return this->flags & flag_seen;
}
