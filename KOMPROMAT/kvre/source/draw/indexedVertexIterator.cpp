// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvre/draw/indexedVertexIterator.hpp"

kvre::IndexedVertexIterator::IndexedVertexIterator()
{
	this->value = 0;
}

kvre::IndexedVertexIterator::IndexedVertexIterator(
	const kompvter::VertexIndexMapper& index_mapper,
	std::size_t offset)
{
	this->index_mapper = index_mapper;
	this->value = offset;
}


kvre::IndexedVertexIterator&
kvre::IndexedVertexIterator::operator ++()
{
	// XXX: Uh-oh, doesn't handle overflow.
	++this->value;
	return *this;
}

kvre::IndexedVertexIterator
kvre::IndexedVertexIterator::operator ++(int)
{
	auto previous = *this;
	++this->value;
	return previous;
}

kvre::IndexedVertexIterator&
kvre::IndexedVertexIterator::operator --()
{
	--this->value;
	return *this;
}

kvre::IndexedVertexIterator
kvre::IndexedVertexIterator::operator --(int)
{
	assert(this->value > 0);
	auto previous = *this;
	--(*this);
	return previous;
};

kvre::IndexedVertexIterator&
kvre::IndexedVertexIterator::operator +=(
	difference_type distance)
{
	// XXX: Uh-oh, doesn't handle overflow.
	this->value += distance;
	return *this;
}

kvre::IndexedVertexIterator
kvre::IndexedVertexIterator::operator +(
	kvre::IndexedVertexIterator::difference_type distance) const
{
	return IndexedVertexIterator(
		this->index_mapper,
		this->value + distance);
}

kvre::IndexedVertexIterator
operator +(
	kvre::IndexedVertexIterator::difference_type distance,
	const kvre::IndexedVertexIterator& current)
{
	return current + distance;
}

kvre::IndexedVertexIterator&
kvre::IndexedVertexIterator::operator -=(
	difference_type distance)
{
	assert(distance <= this->value);
	this->value -= distance;
	return *this;
}

kvre::IndexedVertexIterator
kvre::IndexedVertexIterator::operator -(
	difference_type distance) const
{
	auto result = *this;
	result -= distance;
	return result;
}

kvre::IndexedVertexIterator::difference_type
kvre::IndexedVertexIterator::operator -(
	const IndexedVertexIterator& other) const
{
	return this->value - other.value;
}

bool kvre::IndexedVertexIterator::operator <(
	const IndexedVertexIterator& other) const
{
	return this->value < other.value &&
		this->index_mapper == other.index_mapper;
}

bool kvre::IndexedVertexIterator::operator <=(
	const IndexedVertexIterator& other) const
{
	return this->value <= other.value &&
		this->index_mapper == other.index_mapper;
}

bool kvre::IndexedVertexIterator::operator >(
	const IndexedVertexIterator& other) const
{
	return this->value > other.value &&
		this->index_mapper == other.index_mapper;
}

bool kvre::IndexedVertexIterator::operator >=(
	const IndexedVertexIterator& other) const
{
	return this->value >= other.value &&
		this->index_mapper == other.index_mapper;
}

bool kvre::IndexedVertexIterator::operator ==(
	const IndexedVertexIterator& other) const
{
	return this->value == other.value &&
		this->index_mapper == other.index_mapper;
}

bool kvre::IndexedVertexIterator::operator !=(
	const IndexedVertexIterator& other) const
{
	return this->value != other.value &&
		this->index_mapper == other.index_mapper;
}

kvre::IndexedVertexIterator::value_type
kvre::IndexedVertexIterator::operator *() const
{
	if (!this->index_mapper)
	{
		return this->value;
	}
	else
	{
		assert(this->value < this->index_mapper.get_num_indices());

		std::size_t result;
		if (this->index_mapper.lookup(this->value, result))
		{
			return result;
		}
		else
		{
			assert(false);
			return 0;
		}
	}
}

kvre::IndexedVertexIterator::value_type
kvre::IndexedVertexIterator::operator[](
	difference_type distance) const
{
	if (!this->index_mapper)
	{
		return this->value;
	}
	else
	{
		assert(this->value < this->index_mapper.get_num_indices());

		std::size_t result;
		if (this->index_mapper.lookup(this->value, result))
		{
			return result;
		}
		else
		{
			assert(false);
			return 0;
		}
	}
}

kvre::IndexedVertexIterator kvre::IndexedVertexIterator::begin(
	const kompvter::VertexIndexMapper& mapper)
{
	IndexedVertexIterator result;

	if (mapper)
	{
		result.index_mapper = mapper;
	}
	result.value = 0;

	return result;
}

kvre::IndexedVertexIterator kvre::IndexedVertexIterator::begin(
	const kompvter::VertexIndexMapper& mapper,
	const kompvter::DrawParameters& draw_parameters)
{
	IndexedVertexIterator result;

	if (!mapper)
	{
		result.value = draw_parameters.get_offset_start();
	}
	else
	{
		result.index_mapper = mapper;
		result.value =
			draw_parameters.get_offset_start() / mapper.get_index_size();
	}

	return result;
}

kvre::IndexedVertexIterator kvre::IndexedVertexIterator::end(
	const kompvter::VertexIndexMapper& mapper,
	const kompvter::DrawParameters& draw_parameters)
{
	IndexedVertexIterator result;

	if (!mapper)
	{
		result.value = draw_parameters.get_offset_stop();
	}
	else
	{
		result.index_mapper = mapper;
		result.value =
			draw_parameters.get_offset_stop() / mapper.get_index_size();
	}

	return result;
}

kvre::IndexedVertexIterator kvre::IndexedVertexIterator::end(
	const kompvter::VertexIndexMapper& mapper)
{
	IndexedVertexIterator result;

	if (mapper)
	{
		result.index_mapper = mapper;
		result.value = mapper.get_num_indices();
	}
	else
	{
		result.value = 0;
	}

	return result;
}
