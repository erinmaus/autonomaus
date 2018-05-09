// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/scene/camera.hpp"
#include "principle/scene/material.hpp"
#include "principle/scene/scene.hpp"

void principle::Scene::remove_node(SceneNode* node)
{
	for (auto i = this->nodes.begin(); i != this->nodes.end(); ++i)
	{
		if ((*i).get() == node)
		{
			this->nodes.erase(i);
			break;
		}
	}
}

void principle::Scene::reset()
{
	this->nodes.clear();
}

void principle::Scene::draw()
{
	draw(this->camera);
}

void principle::Scene::draw(const Camera* camera)
{
	if (camera == nullptr)
	{
		return;
	}

	for (auto& node: this->nodes)
	{
		auto material = node->get_material();
		if (material == nullptr || !material->get_is_enabled())
		{
			continue;
		}

		material->use();
		auto shader = material->get_shader();
		shader->set("principle_Model", node->get_model_matrix());
		camera->apply(shader);

		node->draw();
	}
}

const principle::Camera* principle::Scene::get_camera() const
{
	return this->camera;
}

void principle::Scene::set_camera(const Camera* value)
{
	this->camera = value;
}
