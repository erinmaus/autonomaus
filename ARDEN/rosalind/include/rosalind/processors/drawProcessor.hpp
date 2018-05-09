// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ROSALIND_PROCESSORS_DRAW_PROCESSOR_HPP
#define ROSALIND_PROCESSORS_DRAW_PROCESSOR_HPP

#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/trace/call.hpp"
#include "kompvter/events/drawEvent.hpp"
#include "kompvter/graphics/bufferManager.hpp"

namespace rosalind
{
	class DrawProcessor
	{
	protected:
		DrawProcessor(
			const touchstone::Context* context,
			const kompvter::BufferManager* buffer_manager);
		virtual ~DrawProcessor() = default;

		bool try_process_draw(const touchstone::Call* call);

		virtual bool match() const = 0;
		virtual void on_draw(
			const kompvter::DrawEvent& event) = 0;

	private:
		void process_draw(const touchstone::call::DrawArrays* call);
		void process_draw(const touchstone::call::DrawElements* call);
		void process_draw(const touchstone::call::DrawRangeElements* call);
		void process_draw(const touchstone::call::MultiDrawArrays* call);
		void process_draw(const touchstone::call::MultiDrawElements* call);
		void process_draw(
			int primitive_mode, int element_type,
			std::size_t offset_start, std::size_t offset_stop);

		static int gl_primitive_to_kompvter(GLenum primitive);
		static int gl_element_type_to_kompvter(GLenum element_type);
		static bool gl_vertex_attrib_type_to_kompvter(GLenum type, int& result);
		static std::size_t calculate_offset_start(const void* indices);
		static std::size_t calculate_offset_stop(const void* indices, GLsizei count, GLenum element_type);

		const touchstone::Context* context;
		const kompvter::BufferManager* buffer_manager;
	};
}

#endif
