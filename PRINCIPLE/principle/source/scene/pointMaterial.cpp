// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/scene/pointMaterial.hpp"

float principle::PointMaterial::get_point_size() const
{
	return this->point_size;
}

void principle::PointMaterial::set_point_size(float value)
{
	this->point_size = value;
}

glm::vec4 principle::PointMaterial::get_color() const
{
	return this->color;
}

void principle::PointMaterial::set_color(const glm::vec4& value)
{
	this->color = value;
}

void principle::PointMaterial::use()
{
	Material::use();

	glPointSize(this->point_size);

	auto shader = get_shader();
	shader->set("principle_Color", this->color);
	shader->set("principle_PointSize", this->point_size);
}

void principle::PointMaterial::build()
{
	auto shader = get_shader();
	shader->bind_input("vert_Position", kvncer::Shader::vertex_input_position);
	shader->bind_input("vert_Color", kvncer::Shader::vertex_input_color);
}
