// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/resources/draw.hpp"

principle::Draw::Draw(
	std::size_t call_num,
	int tag,
	int draw_type,
	const kompvter::ModelDrawEvent& event)
{
	assert(draw_type >= draw_type_none && draw_type < draw_type_max);

	this->call_num = call_num;
	this->tag = tag;
	this->draw_type = draw_type;
	this->model_matrix = event.get_model_matrix();
	this->event = event;

	auto translated_origin = glm::vec4(glm::vec3(0.0f), 1.0f);
	translated_origin = model_matrix * translated_origin;
	this->translation = glm::vec3(translated_origin);
}

std::size_t principle::Draw::get_call_num() const
{
	return this->call_num;
}

int principle::Draw::get_tag() const
{
	return this->tag;
}

int principle::Draw::get_draw_type() const
{
	return this->draw_type;
}

glm::mat4 principle::Draw::get_model_matrix() const
{
	return this->model_matrix;
}

glm::vec3 principle::Draw::get_translation() const
{
	return this->translation;
}

const kompvter::ModelDrawEvent& principle::Draw::get_event() const
{
	return this->event;
}

thoreau::TileCoordinate principle::Draw::get_tile_coordinate() const
{
	return thoreau::tile_from_world(this->translation);
}

thoreau::WorldCoordinate principle::Draw::get_world_coordinate() const
{
	return thoreau::WorldCoordinate(
		this->translation.x, this->translation.z);
}

const kompvter::DrawParameters& principle::Draw::get_draw_parameters() const
{
	return this->event.get_draw_parameters();
}

void principle::Draw::copy_bone_transforms(ModelNode* node) const
{
	node->set_num_bones(this->event.get_num_bone_transforms());
	for (std::size_t i = 0; i < this->event.get_num_bone_transforms(); ++i)
	{
		node->set_bone_transform(i, this->event.get_bone_transform(i));
	}
}
