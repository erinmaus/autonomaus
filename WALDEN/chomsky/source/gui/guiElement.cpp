// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cmath>
#include <algorithm>
#include "chomsky/gui/guiElement.hpp"

const std::size_t chomsky::GUIElement::NUM_VERTICES;
const std::size_t chomsky::GUIElement::TOP_LEFT;
const std::size_t chomsky::GUIElement::TOP_RIGHT;
const std::size_t chomsky::GUIElement::BOTTOM_RIGHT;
const std::size_t chomsky::GUIElement::BOTTOM_LEFT;

chomsky::GUIElement::GUIElement(
	const Vertex vertices[], std::size_t num_vertices,
	std::size_t offset,
	const std::string& key,
	int texture_name, int texture_region_name, bool is_textured)
{
	assert(num_vertices == NUM_VERTICES);

	int left = std::numeric_limits<int>::max(), right = std::numeric_limits<int>::min();
	int top = std::numeric_limits<int>::min(), bottom = std::numeric_limits<int>::max();
	for (std::size_t i = 0; i < NUM_VERTICES; ++i)
	{
		this->vertices[i] = vertices[i];
		left = std::min(left, vertices[i].position.x);
		right = std::max(right, vertices[i].position.x);
		top = std::max(top, vertices[i].position.y);
		bottom = std::min(bottom, vertices[i].position.y);
	}

	this->x = left;
	this->y = bottom;
	this->width = right - left;
	this->height = top - bottom;

	float x1 = this->vertices[TOP_LEFT].position.x;
	float y1 = this->vertices[TOP_LEFT].position.y;
	float x2 = this->vertices[TOP_RIGHT].position.x;
	float y2 = this->vertices[TOP_RIGHT].position.y;
	float r = std::atan2(y2 - y1, x2 - x1);
	if (r < 0.0f)
	{
		this->rotation = M_PI * 2.0f + r;
	}
	else
	{
		this->rotation = r;
	}

	this->offset = offset;

	this->key = key;
	this->texture_name = texture_name;
	this->texture_region_name = texture_region_name;
	this->is_textured = is_textured;

	for (std::size_t i = 0; i < NUM_VERTICES; ++i)
	{
		this->color += this->vertices[i].color;
	}
	this->color /= NUM_VERTICES;
}

std::size_t chomsky::GUIElement::get_offset() const
{
	return this->offset;
}

int chomsky::GUIElement::get_x() const
{
	return this->x;
}

int chomsky::GUIElement::get_y() const
{
	return this->y;
}

int chomsky::GUIElement::get_width() const
{
	return this->width;
}

int chomsky::GUIElement::get_height() const
{
	return this->height;
}

float chomsky::GUIElement::get_rotation() const
{
	return this->rotation;
}

const std::string& chomsky::GUIElement::get_key() const
{
	return this->key;
}

void chomsky::GUIElement::set_key(const std::string& value)
{
	this->key = value;
}

int chomsky::GUIElement::get_texture_name() const
{
	return this->texture_name;
}

int chomsky::GUIElement::get_texture_region_name() const
{
	return this->texture_region_name;
}

bool chomsky::GUIElement::get_is_textured() const
{
	return this->is_textured;
}

glm::vec4 chomsky::GUIElement::get_color() const
{
	return this->color;
}

std::size_t chomsky::GUIElement::get_num_vertices() const
{
	return this->num_vertices;
}

chomsky::GUIElement::Vertex chomsky::GUIElement::get_vertex(
	std::size_t index) const
{
	assert(index < this->num_vertices);
	return this->vertices[index];
}
