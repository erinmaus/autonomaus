// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/resources/batchedModelManager.hpp"

bool principle::BatchedModelManager::has(int tag) const
{
	return this->draws.count(tag) != 0;
}

const kvre::BatchedModelDraws* principle::BatchedModelManager::get(int tag) const
{
	if (!has(tag))
	{
		return nullptr;
	}

	return &this->draws.find(tag)->second;
}

void principle::BatchedModelManager::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (this->draws.count(tag) == 0)
	{
		auto batch = kvre::BatchedModelDraws(
			*model,
			event.get_vertex_index_mapper(),
			event.get_draw_parameters());
		this->draws.emplace(
			tag,
			batch);
	}
}

void principle::BatchedModelManager::invalidate_tag(int tag)
{
	this->draws.erase(tag);
}
