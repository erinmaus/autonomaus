// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/scene/material.hpp"
#include "principle/scene/modelNode.hpp"
#include "principle/scene/scene.hpp"

principle::ModelNode::ModelNode(
	Scene* scene, kvncer::Mesh* mesh) :
		SceneNode(scene)
{
	this->mesh = mesh;
	this->use_mesh_draw_parameters = true;
}

principle::ModelNode::ModelNode(
	Scene* scene, kvncer::Mesh* mesh,
	GLenum format, std::size_t offset_start, std::size_t offset_stop) :
		SceneNode(scene)
{
	this->mesh = mesh;
	this->use_mesh_draw_parameters = false;
	this->format = format;
	this->offset_start = offset_start;
	this->offset_stop = offset_stop;
}

void principle::ModelNode::override_mesh_draw_parameters(
	GLenum format, std::size_t offset_start, std::size_t offset_stop)
{
	this->use_mesh_draw_parameters = false;
	this->format = format;
	this->offset_start = offset_start;
	this->offset_stop = offset_stop;
}

void principle::ModelNode::restore_mesh_draw_parameters()
{
	this->use_mesh_draw_parameters = true;
}

kvncer::Mesh* principle::ModelNode::get_mesh() const
{
	return this->mesh;
}

std::size_t principle::ModelNode::get_num_bones() const
{
	return this->bones.size();
}

void principle::ModelNode::set_num_bones(std::size_t value)
{
	this->bones.resize(value, glm::mat4x3(1.0f));
}

glm::mat4 principle::ModelNode::get_bone_transform(std::size_t bone_index) const
{
	return glm::mat4(this->bones.at(bone_index));
}

void principle::ModelNode::set_bone_transform(
	std::size_t bone_index, const glm::mat4& value)
{
	this->bones.at(bone_index) = glm::mat4x3(value);
}

kvncer::Texture* principle::ModelNode::get_texture() const
{
	return this->texture;
}

void principle::ModelNode::set_texture(kvncer::Texture* value)
{
	this->texture = value;
}

void principle::ModelNode::draw()
{
	auto shader = get_material()->get_shader();
	if (this->bones.empty())
	{
		glm::mat4x3 bone(1.0f);
		shader->set(
			"principle_BoneTransforms",
			(const glm::vec4*)&bone[0][0], 3);
	}
	else
	{
		shader->set(
			"principle_BoneTransforms",
			(const glm::vec4*)&this->bones[0][0], this->bones.size() * 3);
	}

	if (this->texture != nullptr)
	{
		this->texture->bind(0);
		shader->set("principle_Texture", 1);
	}

	if (this->use_mesh_draw_parameters)
	{
		this->mesh->draw();
	}
	else
	{
		this->mesh->draw(this->format, this->offset_start, this->offset_stop);
	}
}
