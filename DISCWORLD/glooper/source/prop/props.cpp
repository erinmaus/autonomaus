// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "glooper/prop/props.hpp"

bool glooper::Props::has(const thoreau::TileCoordinate& tile_coordinate) const
{
	return this->tile_to_prop.count(tile_coordinate) != 0;
}

bool glooper::Props::has(const std::string& name) const
{
	return this->prop_to_tile.count(name) != 0;
}

std::size_t glooper::Props::count() const
{
	return this->props.size();
}

std::size_t glooper::Props::count(const thoreau::TileCoordinate& tile_coordinate) const
{
	return this->tile_to_prop.count(tile_coordinate);
}

std::size_t glooper::Props::count(const std::string& name) const
{
	return this->prop_to_tile.count(name);
}

bool glooper::Props::empty() const
{
	return this->props.empty();
}

void glooper::Props::clear()
{
	this->tile_to_prop.clear();
	this->prop_to_tile.clear();
	this->props.clear();
}

void glooper::Props::add(const Prop& prop)
{
	if (this->props.count(prop) == 0)
	{
		this->props.insert(prop);
		this->tile_to_prop.emplace(prop.get_tile_coordinate(), prop);
		this->prop_to_tile.emplace(prop.get_name(), prop.get_tile_coordinate());
	}
}

void glooper::Props::add(
	const std::string& name,
	const thoreau::TileCoordinate& tile_coordinate)
{
	Prop prop;
	prop.set_name(name);
	prop.set_tile_coordinate(tile_coordinate);

	add(prop);
}

void glooper::Props::remove(const Prop& prop)
{
	if (this->props.count(prop) != 0)
	{
		auto t = this->tile_to_prop.equal_range(prop.get_tile_coordinate());
		for (auto i = t.first; i != t.second; /* No. */)
		{
			if (i->second.get_name() == prop.get_name())
			{
				i = this->tile_to_prop.erase(i);
			}
			else
			{
				++i;
			}
		}

		auto p = this->prop_to_tile.equal_range(prop.get_name());
		for (auto i = p.first; i != p.second; /* No. */)
		{
			if (i->second == prop.get_tile_coordinate())
			{
				i = this->prop_to_tile.erase(i);
			}
			else
			{
				++i;
			}
		}

		this->props.erase(prop);
	}
}

void glooper::Props::remove(
	const std::string& name,
	const thoreau::TileCoordinate& tile_coordinate)
{
	Prop prop;
	prop.set_name(name);
	prop.set_tile_coordinate(tile_coordinate);

	remove(prop);
}

glooper::Props::const_iterator glooper::Props::begin() const
{
	return this->props.begin();
}

glooper::Props::const_iterator glooper::Props::end() const
{
	return this->props.end();
}

glooper::Props::const_prop_iterator glooper::Props::begin(const std::string& name) const
{
	return this->prop_to_tile.lower_bound(name);
}

glooper::Props::const_prop_iterator glooper::Props::end(const std::string& name) const
{
	return this->prop_to_tile.upper_bound(name);
}

glooper::Props::const_space_iterator glooper::Props::begin(const thoreau::TileCoordinate& tile_coordinate) const
{
	return this->tile_to_prop.lower_bound(tile_coordinate);
}
glooper::Props::const_space_iterator glooper::Props::end(const thoreau::TileCoordinate& tile_coordinate) const
{
	return this->tile_to_prop.upper_bound(tile_coordinate);
}
