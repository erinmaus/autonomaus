// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/resources/modelSelection.hpp"

principle::ModelSelection::ModelSelection(
	const kvre::Model* model,
	const kompvter::VertexIndexMapper& index_mapper)
{
	this->model = model;
	this->index_mapper = index_mapper;

	this->index_offset_start = 0;
	this->index_offset_stop = this->index_mapper.get_num_indices();
}

const kvre::Model* principle::ModelSelection::get_model() const
{
	return this->model;
}

const kompvter::VertexIndexMapper& principle::ModelSelection::get_index_mapper() const
{
	return this->index_mapper;
}

std::size_t principle::ModelSelection::get_index_offset_start() const
{
	return this->index_offset_start;
}

void principle::ModelSelection::set_index_offset_start(std::size_t value)
{
	this->index_offset_start = value;
}

std::size_t principle::ModelSelection::get_index_offset_stop() const
{
	return this->index_offset_stop;
}

void principle::ModelSelection::set_index_offset_stop(std::size_t value)
{
	this->index_offset_stop = value;
}

int principle::ModelSelection::get_tag() const
{
	return this->tag;
}

void principle::ModelSelection::set_tag(int value)
{
	this->tag = value;
}

glm::mat4 principle::ModelSelection::get_model_matrix() const
{
	return this->model_matrix;
}

void principle::ModelSelection::set_model_matrix(const glm::mat4& value)
{
	this->model_matrix = value;
}
