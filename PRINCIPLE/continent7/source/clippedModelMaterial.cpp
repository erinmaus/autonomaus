// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "continent7/clippedModelMaterial.hpp"

const kvlc::Bounds& continent7::ClippedModelMaterial::get_bounds() const
{
	return this->bounds;
}

void continent7::ClippedModelMaterial::set_bounds(const kvlc::Bounds& value)
{
	this->bounds = value;
}

void continent7::ClippedModelMaterial::show()
{
	this->is_visible = true;
}

void continent7::ClippedModelMaterial::hide()
{
	this->is_visible = false;
}

void continent7::ClippedModelMaterial::use()
{
	principle::ModelMaterial::use();
	auto shader = get_shader();
	shader->set("principle_MinBounds", this->bounds.min);
	shader->set("principle_MaxBounds", this->bounds.max);
}

bool continent7::ClippedModelMaterial::get_is_enabled() const
{
	return principle::ModelMaterial::get_is_enabled() && this->is_visible;
}
