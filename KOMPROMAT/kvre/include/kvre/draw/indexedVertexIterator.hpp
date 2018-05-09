// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVRE_DRAW_INDEXED_VERTEX_LOOKUP_HPP
#define KVRE_DRAW_INDEXED_VERTEX_LOOKUP_HPP

#include <cassert>
#include <iterator>
#include "kompvter/graphics/drawParameters.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvre/draw/rangeVertexIterator.hpp"

namespace kvre
{
	struct IndexedVertexIterator : public std::iterator<std::bidirectional_iterator_tag, std::size_t, std::size_t, const std::size_t*, std::size_t>
	{
	public:
		IndexedVertexIterator();
		explicit IndexedVertexIterator(
			const kompvter::VertexIndexMapper& index_mapper,
			std::size_t offset = 0);
		IndexedVertexIterator(const IndexedVertexIterator& other) = default;
		~IndexedVertexIterator() = default;

		IndexedVertexIterator& operator ++();
		IndexedVertexIterator operator ++(int);
		IndexedVertexIterator& operator --();
		IndexedVertexIterator operator --(int);

		IndexedVertexIterator& operator +=(difference_type distance);
		IndexedVertexIterator operator +(difference_type distance) const;
		friend IndexedVertexIterator operator +(difference_type distance, const IndexedVertexIterator& current);
		IndexedVertexIterator& operator -=(difference_type distance);
		IndexedVertexIterator operator -(difference_type distance) const;
		difference_type operator -(const IndexedVertexIterator& other) const;

		bool operator <(const IndexedVertexIterator& other) const;
		bool operator <=(const IndexedVertexIterator& other) const;
		bool operator >(const IndexedVertexIterator& other) const;
		bool operator >=(const IndexedVertexIterator& other) const;
		bool operator ==(const IndexedVertexIterator& other) const;
		bool operator !=(const IndexedVertexIterator& other) const;

		value_type operator *() const;
		value_type operator[](difference_type distance) const;

		static IndexedVertexIterator begin(
			const kompvter::VertexIndexMapper& mapper);
		static IndexedVertexIterator begin(
			const kompvter::VertexIndexMapper& mapper,
			const kompvter::DrawParameters& draw_parameters);

		static IndexedVertexIterator end(
			const kompvter::VertexIndexMapper& mapper);
		static IndexedVertexIterator end(
			const kompvter::VertexIndexMapper& mapper,
			const kompvter::DrawParameters& draw_parameters);

	private:
		std::size_t value;
		kompvter::VertexIndexMapper index_mapper;
	};
}

#endif
