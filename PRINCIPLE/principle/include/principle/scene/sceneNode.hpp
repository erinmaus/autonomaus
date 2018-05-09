// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_SCENE_NODE_HPP
#define PRINCIPLE_SCENE_SCENE_NODE_HPP

#include <vector>
#include <glm/glm.hpp>

namespace principle
{
	class Material;
	class Scene;

	class SceneNode
	{
	public:
		SceneNode(Scene* scene);
		virtual ~SceneNode() = default;

		Material* get_material() const;
		void set_material(Material* value);

		Scene* get_scene() const;

		const glm::mat4& get_model_matrix() const;
		void set_model_matrix(const glm::mat4& value);

		virtual void draw() = 0;

	private:
		Scene* scene;
		Material* material = nullptr;

		glm::mat4 model_matrix = glm::mat4(1.0f);
	};
}

#endif
