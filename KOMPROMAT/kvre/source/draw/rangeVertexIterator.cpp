// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvre/draw/rangeVertexIterator.hpp"

kvre::RangeVertexIterator::RangeVertexIterator()
{
	this->value = 0;
}

kvre::RangeVertexIterator::RangeVertexIterator(std::size_t value)
{
	this->value = value;
}

kvre::RangeVertexIterator&
kvre::RangeVertexIterator::operator ++()
{
	// XXX: Uh-oh, doesn't handle overflow.
	++this->value;
	return *this;
}

kvre::RangeVertexIterator
kvre::RangeVertexIterator::operator ++(int)
{
	auto previous = *this;
	++this->value;
	return previous;
}

kvre::RangeVertexIterator&
kvre::RangeVertexIterator::operator --()
{
	--this->value;
	return *this;
}

kvre::RangeVertexIterator
kvre::RangeVertexIterator::operator --(int)
{
	assert(this->value > 0);
	auto previous = *this;
	--(*this);
	return previous;
};

kvre::RangeVertexIterator&
kvre::RangeVertexIterator::operator +=(
	difference_type distance)
{
	// XXX: Uh-oh, doesn't handle overflow.
	this->value += distance;
	return *this;
}

kvre::RangeVertexIterator
kvre::RangeVertexIterator::operator +(
	kvre::RangeVertexIterator::difference_type distance) const
{
	return RangeVertexIterator(this->value + distance);
}

kvre::RangeVertexIterator
operator +(
	kvre::RangeVertexIterator::difference_type distance,
	const kvre::RangeVertexIterator& current)
{
	return current + distance;
}

kvre::RangeVertexIterator&
kvre::RangeVertexIterator::operator -=(
	difference_type distance)
{
	assert(distance <= this->value);
	this->value -= distance;
	return *this;
}

kvre::RangeVertexIterator
kvre::RangeVertexIterator::operator -(
	difference_type distance) const
{
	auto result = *this;
	result -= distance;
	return result;
}

kvre::RangeVertexIterator::difference_type
kvre::RangeVertexIterator::operator -(
	const RangeVertexIterator& other) const
{
	return this->value - other.value;
}

bool kvre::RangeVertexIterator::operator <(
	const RangeVertexIterator& other) const
{
	return this->value < other.value;
}

bool kvre::RangeVertexIterator::operator <=(
	const RangeVertexIterator& other) const
{
	return this->value <= other.value;
}

bool kvre::RangeVertexIterator::operator >(
	const RangeVertexIterator& other) const
{
	return this->value > other.value;
}

bool kvre::RangeVertexIterator::operator >=(
	const RangeVertexIterator& other) const
{
	return this->value >= other.value;
}

bool kvre::RangeVertexIterator::operator ==(
	const RangeVertexIterator& other) const
{
	return this->value == other.value;
}

bool kvre::RangeVertexIterator::operator !=(
	const RangeVertexIterator& other) const
{
	return this->value != other.value;
}

kvre::RangeVertexIterator::value_type
kvre::RangeVertexIterator::operator *() const
{
	return this->value;
}

kvre::RangeVertexIterator::value_type
kvre::RangeVertexIterator::operator[](
	difference_type distance) const
{
	auto offset = *this + distance;
	return *offset;
}
