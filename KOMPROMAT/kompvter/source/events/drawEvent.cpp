// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/events/drawEvent.hpp"

const kompvter::VertexAttributes& kompvter::DrawEvent::get_vertex_attributes() const
{
	return this->attributes;
}

void kompvter::DrawEvent::set_vertex_attributes(const kompvter::VertexAttributes& value)
{
	this->attributes = value;
}

const kompvter::VertexIndexMapper& kompvter::DrawEvent::get_vertex_index_mapper() const
{
	return this->index_mapper;
}

void kompvter::DrawEvent::set_vertex_index_mapper(const kompvter::VertexIndexMapper& value)
{
	this->index_mapper = value;
}

const kompvter::DrawParameters& kompvter::DrawEvent::get_draw_parameters() const
{
	return this->draw_parameters;
}

void kompvter::DrawEvent::set_draw_parameters(const DrawParameters& value)
{
	this->draw_parameters = value;
}

const glm::mat4& kompvter::DrawEvent::get_projection_matrix() const
{
	return this->projection_matrix;
}

void kompvter::DrawEvent::set_projection_matrix(const glm::mat4& value)
{
	this->projection_matrix = value;
}

const glm::mat4& kompvter::DrawEvent::get_view_matrix() const
{
	return this->view_matrix;
}

void kompvter::DrawEvent::set_view_matrix(const glm::mat4& value)
{
	this->view_matrix = value;
}

const glm::mat4& kompvter::DrawEvent::get_model_matrix() const
{
	return this->model_matrix;
}

void kompvter::DrawEvent::set_model_matrix(const glm::mat4& value)
{
	this->model_matrix = value;
}
