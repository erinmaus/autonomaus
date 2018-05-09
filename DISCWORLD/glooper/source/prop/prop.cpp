// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "glooper/prop/prop.hpp"

const std::string& glooper::Prop::get_name() const
{
	return this->name;
}

void glooper::Prop::set_name(const std::string& value)
{
	this->name = value;
}

const thoreau::TileCoordinate& glooper::Prop::get_tile_coordinate() const
{
	return this->tile_coordinate;
}

void glooper::Prop::set_tile_coordinate(const thoreau::TileCoordinate& value)
{
	this->tile_coordinate = value;
}

bool glooper::Prop::operator <(const Prop& other) const
{
	return this->name < other.name ||
		(!(other.name < this->name) && this->tile_coordinate < other.tile_coordinate);
}

bool glooper::Prop::operator <=(const Prop& other) const
{
	return !(other < *this);
}

bool glooper::Prop::operator >(const Prop& other) const
{
	return other < *this;
}

bool glooper::Prop::operator >=(const Prop& other) const
{
	return !(*this < other);
}

bool glooper::Prop::operator ==(const Prop& other) const
{
	return this->name == other.name && this->tile_coordinate == other.tile_coordinate;
}

bool glooper::Prop::operator !=(const Prop& other) const
{
	return !(*this == other);
}
