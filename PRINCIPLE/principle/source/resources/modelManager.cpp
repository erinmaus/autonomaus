// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/resources/modelManager.hpp"

bool principle::ModelManager::has(int tag)
{
	auto iter = this->tags.find(tag);
	if (iter == this->tags.end())
	{
		return false;
	}

	if (this->models.count(iter->second) == 0)
	{
		return false;
	}

	return true;
}

const kvre::Model* principle::ModelManager::get(int tag)
{
	auto iter = this->tags.find(tag);
	if (iter != this->tags.end())
	{
		auto model = models.find(iter->second);
		assert(model != this->models.end());
		return &model->second;
	}

	return nullptr;
}

bool principle::ModelManager::tag_has_vertex_index_mapper(int tag) const
{
	auto iter = this->index_mappers.find(tag);
	return iter != this->index_mappers.end();
}

const kompvter::VertexIndexMapper&
principle::ModelManager::get_tag_vertex_index_mapper(int tag) const
{
	assert(tag_has_vertex_index_mapper(tag));

	auto iter = this->index_mappers.find(tag);
	return iter->second;
}

void principle::ModelManager::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	if (event.get_vertex_index_mapper())
	{
		this->index_mappers[tag] = event.get_vertex_index_mapper();
	}

	auto iter = this->models.find(model);
	if (iter == this->models.end())
	{
		this->models.emplace(model, *model);
	}

	this->tags.emplace(tag, model);
}

void principle::ModelManager::invalidate_tag(int tag)
{
	this->index_mappers.erase(tag);
	this->tags.erase(tag);
}

void principle::ModelManager::invalidate_model(const kvre::Model* model)
{
	this->models.erase(model);
}
