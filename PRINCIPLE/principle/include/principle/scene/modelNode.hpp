// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_MODEL_NODE_HPP
#define PRINCIPLE_SCENE_MODEL_NODE_HPP

#include <vector>
#include <glm/glm.hpp>

#include "kvncer/graphics/mesh.hpp"
#include "kvncer/graphics/shader.hpp"
#include "kvncer/graphics/texture.hpp"
#include "principle/scene/sceneNode.hpp"

namespace principle
{
	class Scene;

	class ModelNode : public SceneNode
	{
	public:
		ModelNode(
			Scene* scene, kvncer::Mesh* mesh);
		ModelNode(
			Scene* scene,
			kvncer::Mesh* mesh,
			GLenum format, std::size_t offset_start, std::size_t offset_stop);

		void override_mesh_draw_parameters(
			GLenum format, std::size_t offset_start, std::size_t offset_stop);
		void restore_mesh_draw_parameters();

		kvncer::Mesh* get_mesh() const;

		std::size_t get_num_bones() const;
		void set_num_bones(std::size_t value);

		glm::mat4 get_bone_transform(std::size_t bone_index) const;
		void set_bone_transform(std::size_t bone_index, const glm::mat4& value);

		kvncer::Texture* get_texture() const;
		void set_texture(kvncer::Texture* value);

		void draw() override;

	private:
		std::vector<glm::mat4x3> bones;

		kvncer::Mesh* mesh;
		kvncer::Texture* texture = nullptr;

		bool use_mesh_draw_parameters;
		GLenum format;
		std::size_t offset_start, offset_stop;
	};
}

#endif
