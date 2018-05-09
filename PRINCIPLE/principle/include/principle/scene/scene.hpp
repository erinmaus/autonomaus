// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_SCENE_HPP
#define PRINCIPLE_SCENE_SCENE_HPP

#include <memory>
#include <utility>
#include <vector>
#include "kvncer/graphics/mesh.hpp"
#include "principle/scene/sceneNode.hpp"

namespace principle
{
	class Camera;
	class Model;

	class Scene
	{
	public:
		Scene() = default;
		~Scene() = default;

		void reset();

		void draw();
		void draw(const Camera* camera);

		template <typename Node, typename Args>
		Node* add_node(Args&& args);

		template <typename Node, typename... Args>
		Node* add_node(Args&&... args);

		void remove_node(SceneNode* node);

		const Camera* get_camera() const;
		void set_camera(const Camera* value);

	private:
		typedef std::unique_ptr<SceneNode> SceneNodePointer;
		std::vector<SceneNodePointer> nodes;

		const Camera* camera = nullptr;
	};
}

template <typename Node, typename Args>
Node* principle::Scene::add_node(Args&& args)
{
	Node* node = new Node(this, std::forward<Args>(args));
	this->nodes.emplace_back(node);

	return node;
}

template <typename Node, typename... Args>
Node* principle::Scene::add_node(Args&&... args)
{
	Node* node = new Node(this, std::forward<Args>(args)...);
	this->nodes.emplace_back(node);

	return node;
}

#endif
