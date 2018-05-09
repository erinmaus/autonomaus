// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVRE_DRAW_RANGE_VERTEX_ITERATOR_HPP
#define KVRE_DRAW_RANGE_VERTEX_ITERATOR_HPP

#include <cassert>
#include <iterator>

namespace kvre
{
	// Incredibly verbose. Disgusting, really.
	struct RangeVertexIterator : public std::iterator<std::bidirectional_iterator_tag, std::size_t, std::size_t, const std::size_t*, std::size_t>
	{
	public:
		RangeVertexIterator();
		explicit RangeVertexIterator(std::size_t value);
		RangeVertexIterator(const RangeVertexIterator& other) = default;
		~RangeVertexIterator() = default;

		RangeVertexIterator& operator ++();
		RangeVertexIterator operator ++(int);
		RangeVertexIterator& operator --();
		RangeVertexIterator operator --(int);

		RangeVertexIterator& operator +=(difference_type distance);
		RangeVertexIterator operator +(difference_type distance) const;
		friend RangeVertexIterator operator +(difference_type distance, const RangeVertexIterator& current);
		RangeVertexIterator& operator -=(difference_type distance);
		RangeVertexIterator operator -(difference_type distance) const;
		difference_type operator -(const RangeVertexIterator& other) const;

		bool operator <(const RangeVertexIterator& other) const;
		bool operator <=(const RangeVertexIterator& other) const;
		bool operator >(const RangeVertexIterator& other) const;
		bool operator >=(const RangeVertexIterator& other) const;
		bool operator ==(const RangeVertexIterator& other) const;
		bool operator !=(const RangeVertexIterator& other) const;

		value_type operator *() const;
		value_type operator[](difference_type distance) const;

	public:
		std::size_t value;
	};
}

#endif
