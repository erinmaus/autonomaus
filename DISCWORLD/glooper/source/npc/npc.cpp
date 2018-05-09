// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "glooper/npc/npc.hpp"

glooper::NPC::NPC(ID id)
{
	this->id = id;
}

glooper::NPC::NPC(ID id, const NPC& other)
{
	*this = other;
	this->id = id;
}

glooper::NPC::ID glooper::NPC::get_id() const
{
	return this->id;
}

const std::string& glooper::NPC::get_profile_name() const
{
	return this->profile_name;
}

void glooper::NPC::set_profile_name(const std::string& value)
{
	this->profile_name = value;
}

void glooper::NPC::set_profile_variant(const std::string& value)
{
	this->profile_variant = value;
}

const std::string& glooper::NPC::get_profile_variant() const
{
	return this->profile_variant;
}

const std::set<std::string>& glooper::NPC::get_animations() const
{
	return this->animations;
}

void glooper::NPC::set_animations(const std::set<std::string>& value)
{
	this->animations = value;
}

const glm::vec3& glooper::NPC::get_position() const
{
	return this->position;
}

void glooper::NPC::set_position(const glm::vec3& value)
{
	this->position = value;
}

const glm::vec3& glooper::NPC::get_previous_position() const
{
	return this->previous_position;
}

void glooper::NPC::set_previous_position(const glm::vec3& value)
{
	this->previous_position = value;
}

const thoreau::WorldCoordinate& glooper::NPC::get_world_coordinate() const
{
	return this->world_coordinate;
}

void glooper::NPC::set_world_coordinate(const thoreau::WorldCoordinate& value)
{
	this->world_coordinate = value;
}

const kvlc::Bounds& glooper::NPC::get_bounds() const
{
	return this->bounds;
}

void glooper::NPC::set_bounds(const kvlc::Bounds& value)
{
	this->bounds = value;
}

const glm::mat4& glooper::NPC::get_model_matrix() const
{
	return this->model_matrix;
}

void glooper::NPC::set_model_matrix(const glm::mat4& value)
{
	this->model_matrix = value;
}

bool glooper::NPC::get_is_on_screen() const
{
	return this->is_on_screen;
}

void glooper::NPC::set_is_on_screen(bool value)
{
	this->is_on_screen = value;
}

bool glooper::NPC::is_visible(const glm::mat4& view, const glm::mat4& projection) const
{
	glm::vec4 points[] = {
		glm::vec4(this->bounds.min.x, this->bounds.min.y, this->bounds.min.z, 1.0f),
		glm::vec4(this->bounds.max.x, this->bounds.min.y, this->bounds.min.z, 1.0f),
		glm::vec4(this->bounds.min.x, this->bounds.max.y, this->bounds.min.z, 1.0f),
		glm::vec4(this->bounds.min.x, this->bounds.min.y, this->bounds.max.z, 1.0f),
		glm::vec4(this->bounds.max.x, this->bounds.max.y, this->bounds.min.z, 1.0f),
		glm::vec4(this->bounds.max.x, this->bounds.min.y, this->bounds.max.z, 1.0f),
		glm::vec4(this->bounds.min.x, this->bounds.max.y, this->bounds.max.z, 1.0f),
		glm::vec4(this->bounds.max.x, this->bounds.max.y, this->bounds.max.z, 1.0f)
	};

	glm::mat4 model_view_projection = this->model_matrix * view * projection;
	for (std::size_t i = 0; i < 8; ++i)
	{
		auto value = model_view_projection * points[i];
		glm::vec3 normalized = glm::vec3(value) / value.w;

		if (normalized.x > -1.0f && normalized.x < 1.0f &&
			normalized.y > -1.0f && normalized.y < 1.0f &&
			normalized.z > -1.0f && normalized.z < 1.0f)
		{
			return true;
		}
	}

	return false;
}


bool glooper::NPC::maybe_on_screen(const glm::mat4& view, const glm::mat4& projection) const
{
	glm::vec4 points[] = {
		glm::vec4(this->bounds.min.x, this->bounds.min.y, this->bounds.min.z, 1.0f),
		glm::vec4(this->bounds.max.x, this->bounds.min.y, this->bounds.min.z, 1.0f),
		glm::vec4(this->bounds.min.x, this->bounds.max.y, this->bounds.min.z, 1.0f),
		glm::vec4(this->bounds.min.x, this->bounds.min.y, this->bounds.max.z, 1.0f),
		glm::vec4(this->bounds.max.x, this->bounds.max.y, this->bounds.min.z, 1.0f),
		glm::vec4(this->bounds.max.x, this->bounds.min.y, this->bounds.max.z, 1.0f),
		glm::vec4(this->bounds.min.x, this->bounds.max.y, this->bounds.max.z, 1.0f),
		glm::vec4(this->bounds.max.x, this->bounds.max.y, this->bounds.max.z, 1.0f)
	};

	auto view_projection = projection * view;

	for (std::size_t i = 0; i < 8; ++i)
	{
		auto point = points[i] + glm::vec4(get_position(), 0.0f);
		auto value = view_projection * point;
		glm::vec3 normalized = glm::vec3(value) / value.w;

		if (normalized.x > -1.0f && normalized.x < 1.0f &&
			normalized.z > -1.0f && normalized.z < 1.0f)
		{
			return true;
		}
	}

	return false;
}
