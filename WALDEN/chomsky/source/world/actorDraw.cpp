// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/world/actorDraw.hpp"

const glm::mat4& chomsky::ActorDraw::get_model_matrix() const
{
	return this->model_matrix;
}

void chomsky::ActorDraw::set_model_matrix(const glm::mat4& value)
{
	this->model_matrix = value;

	auto origin = glm::vec4(glm::vec3(0.0f), 1.0f);
	auto transformed_origin = value * origin;
	this->world_position = glm::vec3(transformed_origin);
}

const glm::vec3& chomsky::ActorDraw::get_world_position() const
{
	return this->world_position;
}

void chomsky::ActorDraw::add_model(const kvre::Model* model)
{
	this->models.insert(model);
}

void chomsky::ActorDraw::add_tag(int tag)
{
	this->tags.insert(tag);
}

void chomsky::ActorDraw::add_bone(
	const kvre::Model* model, const glm::mat4& bone)
{
	this->bones.push_back(bone);
	this->bones_by_model[model].push_back(bone);
}

std::size_t chomsky::ActorDraw::get_num_bones() const
{
	return this->bones.size();
}

std::size_t chomsky::ActorDraw::get_num_bones(const kvre::Model* model) const
{
	auto iter = this->bones_by_model.find(model);
	if (iter == this->bones_by_model.end())
	{
		return 0;
	}

	return iter->second.size();
}

glm::mat4 chomsky::ActorDraw::get_bone(std::size_t index) const
{
	return this->bones.at(index);
}

glm::mat4 chomsky::ActorDraw::get_bone(const kvre::Model* model, std::size_t index) const
{
	auto iter = this->bones_by_model.find(model);
	if (iter == this->bones_by_model.end())
	{
		return glm::mat4(1.0f);
	}

	return iter->second.at(index);
}

chomsky::ActorDraw::const_model_iterator chomsky::ActorDraw::models_begin() const
{
	return this->models.begin();
}

chomsky::ActorDraw::const_model_iterator chomsky::ActorDraw::models_end() const
{
	return this->models.end();
}

chomsky::ActorDraw::const_tag_iterator chomsky::ActorDraw::tags_begin() const
{
	return this->tags.begin();
}

chomsky::ActorDraw::const_tag_iterator chomsky::ActorDraw::tags_end() const
{
	return this->tags.end();
}
