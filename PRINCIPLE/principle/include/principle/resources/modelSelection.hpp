// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_RESOURCES_MODEL_SELECTION_HPP
#define PRINCIPLE_RESOURCES_MODEL_SELECTION_HPP

#include <glm/glm.hpp>
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvre/model/model.hpp"

namespace principle
{
	class ModelSelection
	{
	public:
		ModelSelection(
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper);
		~ModelSelection() = default;

		const kvre::Model* get_model() const;
		const kompvter::VertexIndexMapper& get_index_mapper() const;

		std::size_t get_index_offset_start() const;
		void set_index_offset_start(std::size_t value);

		std::size_t get_index_offset_stop() const;
		void set_index_offset_stop(std::size_t value);

		int get_tag() const;
		void set_tag(int value);

		glm::mat4 get_model_matrix() const;
		void set_model_matrix(const glm::mat4& value);

	private:
		const kvre::Model* model;
		kompvter::VertexIndexMapper index_mapper;

		glm::mat4 model_matrix = glm::mat4(1.0f);

		std::size_t index_offset_start, index_offset_stop;

		int tag = 0;
	};
}

#endif
