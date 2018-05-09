// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompvter/graphics/drawParameters.hpp"

kompvter::DrawParameters::DrawParameters(
	int primitive_type,
	std::size_t offset_start, std::size_t offset_stop)
{
	assert(primitive_type >= primitive_none && primitive_type <= primitive_max);

	this->primitive_type = primitive_type;
	this->offset_start = offset_start;
	this->offset_stop = offset_stop;
}

int kompvter::DrawParameters::get_primitive_type() const
{
	return this->primitive_type;
}

std::size_t kompvter::DrawParameters::get_offset_start() const
{
	return this->offset_start;
}

std::size_t kompvter::DrawParameters::get_offset_stop() const
{
	return this->offset_stop;
}

bool kompvter::DrawParameters::operator ==(const DrawParameters& other) const
{
	return this->primitive_type == other.primitive_type &&
		this->offset_start == other.offset_start &&
		this->offset_stop == other.offset_stop;
}

bool kompvter::DrawParameters::operator !=(const DrawParameters& other) const
{
	return !(*this == other);
}
