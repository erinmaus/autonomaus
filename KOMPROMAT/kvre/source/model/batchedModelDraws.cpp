// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <map>
#include <set>
#include <vector>
#include "kvre/draw/indexedVertexIterator.hpp"
#include "kvre/model/batchedModelDraws.hpp"
#include "kvre/model/detail/splitter.hpp"

kvre::BatchedModelDraws::BatchedModelDraws(
	const Model& model,
	const kompvter::VertexIndexMapper& index_mapper)
{
	kompvter::DrawParameters draw_parameters(
		kompvter::DrawParameters::primitive_triangles,
		0, index_mapper.get_num_indices() * index_mapper.get_index_size());
	collect(model, index_mapper, draw_parameters);
}

kvre::BatchedModelDraws::BatchedModelDraws(
	const Model& model,
	const kompvter::VertexIndexMapper& index_mapper,
	const kompvter::DrawParameters& draw_parameters)
{
	collect(model, index_mapper, draw_parameters);
}

std::size_t kvre::BatchedModelDraws::count() const
{
	return this->index_mappers.size();
}

bool kvre::BatchedModelDraws::empty() const
{
	return this->index_mappers.empty();
}

void kvre::BatchedModelDraws::collect(
	const Model& model,
	const kompvter::VertexIndexMapper& index_mapper,
	const kompvter::DrawParameters& draw_parameters)
{
	detail::Splitter splitter(model, index_mapper, draw_parameters);
	splitter.collect(*this);
}

kvre::BatchedModelDraws::const_iterator kvre::BatchedModelDraws::begin() const
{
	return this->index_mappers.begin();
}

kvre::BatchedModelDraws::const_iterator kvre::BatchedModelDraws::end() const
{
	return this->index_mappers.end();
}
