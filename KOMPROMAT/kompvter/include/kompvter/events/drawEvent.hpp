// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_EVENTS_DRAW_EVENT_HPP
#define KOMPVTER_EVENTS_DRAW_EVENT_HPP

#include <cstddef>
#include <string>
#include <glm/glm.hpp>
#include "kompvter/graphics/drawParameters.hpp"
#include "kompvter/graphics/vertexAttributes.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"

namespace kompvter
{
	class DrawEvent
	{
	public:
		DrawEvent() = default;
		virtual ~DrawEvent() = default;

		const VertexAttributes& get_vertex_attributes() const;
		void set_vertex_attributes(const VertexAttributes& value);

		const VertexIndexMapper& get_vertex_index_mapper() const;
		void set_vertex_index_mapper(const VertexIndexMapper& value);

		const DrawParameters& get_draw_parameters() const;
		void set_draw_parameters(const DrawParameters& value);

		const glm::mat4& get_projection_matrix() const;
		void set_projection_matrix(const glm::mat4& value);

		const glm::mat4& get_view_matrix() const;
		void set_view_matrix(const glm::mat4& value);

		const glm::mat4& get_model_matrix() const;
		void set_model_matrix(const glm::mat4& value);

	private:
		VertexAttributes attributes;
		VertexIndexMapper index_mapper;
		DrawParameters draw_parameters;

		glm::mat4 projection_matrix = glm::mat4(1.0f);
		glm::mat4 view_matrix = glm::mat4(1.0f);
		glm::mat4 model_matrix = glm::mat4(1.0f);
	};
}

#endif
