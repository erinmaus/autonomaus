// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVRE_MODEL_BATCHED_MODEL_DRAWS_HPP
#define KVRE_MODEL_BATCHED_MODEL_DRAWS_HPP

#include <tuple>
#include "kompvter/graphics/drawParameters.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvre/model/model.hpp"

namespace kvre
{
	namespace detail
	{
		class Splitter;
	}

	class BatchedModelDraws
	{
	public:
		friend class detail::Splitter;

		BatchedModelDraws(
			const Model& model,
			const kompvter::VertexIndexMapper& index_mapper);
		BatchedModelDraws(
			const Model& model,
			const kompvter::VertexIndexMapper& index_mapper,
			const kompvter::DrawParameters& draw_parameters);
		~BatchedModelDraws() = default;

		std::size_t count() const;
		bool empty() const;

		kompvter::VertexIndexMapper get_index_mapper(std::size_t index) const;

	private:
		typedef std::vector<kompvter::VertexIndexMapper> IndexMapperCollection;
		IndexMapperCollection index_mappers;

		void collect(
			const Model& model,
			const kompvter::VertexIndexMapper& index_mapper,
			const kompvter::DrawParameters& draw_parameters);

	public:
		typedef IndexMapperCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
