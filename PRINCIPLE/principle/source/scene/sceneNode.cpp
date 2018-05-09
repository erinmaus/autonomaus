// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/scene/scene.hpp"
#include "principle/scene/sceneNode.hpp"

principle::SceneNode::SceneNode(Scene* scene)
{
	this->scene = scene;
}

principle::Scene* principle::SceneNode::get_scene() const
{
	return this->scene;
}

principle::Material* principle::SceneNode::get_material() const
{
	return this->material;
}

void principle::SceneNode::set_material(Material* value)
{
	this->material = value;
}

const glm::mat4& principle::SceneNode::get_model_matrix() const
{
	return this->model_matrix;
}

void principle::SceneNode::set_model_matrix(const glm::mat4& value)
{
	this->model_matrix = value;
}
