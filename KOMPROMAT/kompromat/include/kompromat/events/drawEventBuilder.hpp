// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_EVENTS_DRAW_EVENT_BUILDER_HPP
#define KOMPROMAT_EVENTS_DRAW_EVENT_BUILDER_HPP

#include <cstddef>
#include <string>
#include <glm/glm.hpp>
#include "kompvter/events/drawEvent.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/vertexAttributes.hpp"
#include "paley/event/event.hpp"

namespace kompromat
{
	class DrawEventBuilder
	{
	public:
		explicit DrawEventBuilder(kompvter::DrawEvent& event);
		virtual ~DrawEventBuilder() = default;

	protected:
		void process(
			const paley::Event& e,
			kompvter::BufferManager& buffer_manager);

	private:
		void process_vertex_index_mapper(
			const paley::Event& e,
			kompvter::BufferManager& buffer_manager);
		void process_vertex_attributes(
			const paley::Event& e,
			kompvter::BufferManager& buffer_manager);
		void process_draw_parameters(const paley::Event& e);
		void process_vertex_attribute(
			kompvter::VertexAttributes& attributes,
			const std::string& name,
			const paley::EventField& attribute,
			kompvter::BufferManager& buffer_manager);
		void process_matrix(
			const paley::Event& e,
			const std::string& field,
			glm::mat4& result);

		kompvter::DrawEvent& event;
	};
}

#endif
