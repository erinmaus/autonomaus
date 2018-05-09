// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_RESOURCES_MODEL_SELECTION_MANAGER_HPP
#define PRINCIPLE_RESOURCES_MODEL_SELECTION_MANAGER_HPP

#include <memory>
#include <unordered_map>
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvncer/graphics/shader.hpp"
#include "kvncer/graphics/meshBuffer.hpp"
#include "kvre/model/model.hpp"
#include "principle/resources/modelSelection.hpp"
#include "principle/scene/modelMaterial.hpp"
#include "principle/scene/camera.hpp"
#include "principle/scene/modelNode.hpp"
#include "principle/scene/scene.hpp"
#include "principle/scene/sceneNode.hpp"

namespace principle
{
	class ModelSelection;

	class ModelSelectionManager
	{
	public:
		struct iterator;
		struct const_iterator;

		ModelSelectionManager();
		~ModelSelectionManager();

		void draw(
			const Camera& camera,
			const glm::vec4& tint_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

		ModelSelection* select(
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper);

		void unselect(ModelSelection* selection);
		void unselect_all();

		std::size_t count() const;
		bool empty() const;

		ModelSelection* at(std::size_t index);
		const ModelSelection* at(std::size_t index) const;

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;

	private:
		kvncer::Shader* selected_node_shader;
		principle::ModelMaterial selected_node_material;
		principle::Scene scene;

		typedef std::unique_ptr<ModelSelection> ModelSelectionPointer;
		typedef std::vector<ModelSelectionPointer> ModelSelectionCollection;
		ModelSelectionCollection selections;

		std::map<const kvre::Model*, std::weak_ptr<kvncer::MeshBuffer>> vertex_buffers;

		struct ModelSelectionDrawData
		{
			ModelSelection* selection;
			kvncer::Mesh mesh;
			std::shared_ptr<kvncer::MeshBuffer> vertex_buffer;
			kvncer::MeshBuffer index_buffer;
			ModelNode* node;
		};
		typedef std::unique_ptr<ModelSelectionDrawData> ModelSelectionDrawDataPointer;
		std::vector<ModelSelectionDrawDataPointer> selection_draw_data;

	public:
		struct iterator : public ModelSelectionCollection::iterator
		{
		public:
			typedef ModelSelection value_type;
			typedef ModelSelection* pointer_type;
			typedef ModelSelection& reference_type;

			iterator(ModelSelectionCollection::iterator source);

			reference_type operator *() const;
			pointer_type operator ->() const;
		};

		struct const_iterator : public ModelSelectionCollection::const_iterator
		{
		public:
			typedef ModelSelection value_type;
			typedef ModelSelection* pointer_type;
			typedef ModelSelection& reference_type;

			const_iterator(ModelSelectionCollection::const_iterator source);

			reference_type operator *() const;
			pointer_type operator ->() const;
		};
	};
}

#endif
