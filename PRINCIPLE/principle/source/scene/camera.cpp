// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/scene/camera.hpp"

void principle::Camera::apply(kvncer::Shader* shader) const
{
	auto projection = get_projection_matrix();
	auto view = get_view_matrix();

	shader->set("principle_Projection", projection);
	shader->set("principle_View", view);
}
