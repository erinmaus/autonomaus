// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "thoreau/detail/coordinate.hpp"

const int thoreau::Coordinate::MIN_COORDINATE;
const int thoreau::Coordinate::MAX_COORDINATE;
const int thoreau::Coordinate::MIN_LAYER;
const int thoreau::Coordinate::MAX_LAYER;
const std::uint64_t thoreau::Coordinate::COORDINATE_MASK;
const std::uint64_t thoreau::Coordinate::COORDINATE_BITS;
const std::uint64_t thoreau::Coordinate::LAYER_MASK;
const std::uint64_t thoreau::Coordinate::LAYER_BITS;

thoreau::Coordinate::Coordinate(int x, int y, int layer)
{
	assert(x >= MIN_COORDINATE);
	assert(x <= MAX_COORDINATE);
	assert(y >= MIN_COORDINATE);
	assert(y <= MAX_COORDINATE);
	assert(layer >= MIN_LAYER);
	assert(layer <= MAX_LAYER);

	this->x = x;
	this->y = y;
	this->layer = layer;
}

thoreau::Coordinate::Coordinate(Packed other)
{
	from_packed(other);
}

thoreau::Coordinate::Packed thoreau::Coordinate::to_packed() const
{
	assert(this->x >= MIN_COORDINATE);
	assert(this->x <= MAX_COORDINATE);
	assert(this->y >= MIN_COORDINATE);
	assert(this->y <= MAX_COORDINATE);
	assert(this->layer >= MIN_LAYER);
	assert(this->layer <= MAX_LAYER);

	std::uint64_t packed_x = (std::uint64_t)(this->x & COORDINATE_MASK);
	std::uint64_t packed_y = (std::uint64_t)(this->y & COORDINATE_MASK);
	std::uint64_t packed_layer = (std::uint64_t)((std::uint8_t)this->layer);

	return packed_x |
		(packed_y << COORDINATE_BITS) |
		(packed_layer << (COORDINATE_BITS + COORDINATE_BITS));
}

thoreau::Coordinate::Packed thoreau::Coordinate::to_packed(
	int x, int y, int layer)
{
	Coordinate tile(x, y, layer);
	return tile.to_packed();
}

void thoreau::Coordinate::from_packed(Packed value)
{
	std::uint64_t packed_x = (value & COORDINATE_MASK);
	std::uint64_t packed_y = (value >> COORDINATE_BITS) & COORDINATE_MASK;
	std::uint64_t packed_layer = (value >> (COORDINATE_BITS + COORDINATE_BITS)) & LAYER_MASK;

	const std::int32_t SHIFT_BITS = 32 - COORDINATE_BITS;
	assert(SHIFT_BITS >= 0);

	this->x = (((std::int32_t)packed_x) << SHIFT_BITS) >> SHIFT_BITS;
	this->y = (((std::int32_t)packed_y) << SHIFT_BITS) >> SHIFT_BITS;
	this->layer = (std::uint8_t)packed_layer;

	assert(this->x >= MIN_COORDINATE);
	assert(this->x <= MAX_COORDINATE);
	assert(this->y >= MIN_COORDINATE);
	assert(this->y <= MAX_COORDINATE);
	assert(this->layer >= MIN_LAYER);
	assert(this->layer <= MAX_LAYER);
}

bool thoreau::Coordinate::match_xy(const Coordinate& other) const
{
	return this->x == other.x && this->y == other.y;
}

bool thoreau::Coordinate::match_xy(int x, int y) const
{
	return this->x == x && this->y == y;
}

bool thoreau::Coordinate::match_xy(
	const Coordinate &a, const Coordinate &b)
{
	return a.x == b.x && a.y == b.y;
}

bool thoreau::Coordinate::operator <(const Coordinate& other) const
{
	return to_packed() < other.to_packed();
}

bool thoreau::Coordinate::operator <=(const thoreau::Coordinate& other) const
{
	return (*this < other) || (*this == other);
}

bool thoreau::Coordinate::operator >(const Coordinate& other) const
{
	return to_packed() > other.to_packed();
}

bool thoreau::Coordinate::operator >=(const thoreau::Coordinate& other) const
{
	return (*this > other) || (*this == other);
}

bool thoreau::Coordinate::operator ==(const Coordinate& other) const
{
	return this->layer == other.layer &&
		this->x == other.x &&
		this->y == other.y;
}

bool thoreau::Coordinate::operator !=(const Coordinate& other) const
{
	return !(*this == other);
}
