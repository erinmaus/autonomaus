
// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/graphics/vertexAttributes.hpp"
#include "kompvter/graphics/vertexFetcher.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kompvter/graphics/drawParameters.hpp"
#include "rosalind/common/re.hpp"
#include "rosalind/processors/drawProcessor.hpp"
#include "touchstone/gl/enums.hpp"
#include "touchstone/gl/resources/vertexAttribute.hpp"

rosalind::DrawProcessor::DrawProcessor(
	const touchstone::Context* context,
	const kompvter::BufferManager* buffer_manager)
{
	this->context = context;
	this->buffer_manager = buffer_manager;
}

bool rosalind::DrawProcessor::try_process_draw(const touchstone::Call* call)
{
	switch (call->get_token())
	{
		case touchstone::call::DrawArrays::token:
			process_draw((const touchstone::call::DrawArrays*)call);
			break;
		case touchstone::call::DrawElements::token:
			process_draw((const touchstone::call::DrawElements*)call);
			break;
		case touchstone::call::DrawRangeElements::token:
			process_draw((const touchstone::call::DrawRangeElements*)call);
			break;
		case touchstone::call::MultiDrawArrays::token:
			process_draw((const touchstone::call::MultiDrawArrays*)call);
			break;
		case touchstone::call::MultiDrawElements::token:
			process_draw((const touchstone::call::MultiDrawElements*)call);
			break;
		default:
			return false;
	}

	return match();
}

void rosalind::DrawProcessor::process_draw(const touchstone::call::DrawArrays* call)
{
	if (!match())
	{
		return;
	}

	process_draw(
		gl_primitive_to_kompvter(call->get_parameter_mode()),
		kompvter::VertexIndexMapper::format_none,
		call->get_parameter_first(),
		call->get_parameter_first() + call->get_parameter_count());
}

void rosalind::DrawProcessor::process_draw(const touchstone::call::DrawElements* call)
{
	if (!match())
	{
		return;
	}

	process_draw(
		gl_primitive_to_kompvter(call->get_parameter_mode()),
		gl_element_type_to_kompvter(call->get_parameter_type()),
		calculate_offset_start(call->get_parameter_indices()),
		calculate_offset_stop(call->get_parameter_indices(), call->get_parameter_count(), call->get_parameter_type()));
}

void rosalind::DrawProcessor::process_draw(const touchstone::call::DrawRangeElements* call)
{
	if (!match())
	{
		return;
	}

	process_draw(
		gl_primitive_to_kompvter(call->get_parameter_mode()),
		gl_element_type_to_kompvter(call->get_parameter_type()),
		calculate_offset_start(call->get_parameter_indices()),
		calculate_offset_stop(call->get_parameter_indices(), call->get_parameter_count(), call->get_parameter_type()));
}

void rosalind::DrawProcessor::process_draw(const touchstone::call::MultiDrawArrays* call)
{
	if (!match())
	{
		return;
	}

	for (GLsizei i = 0; i < call->get_parameter_drawcount(); ++i)
	{
		process_draw(
			gl_primitive_to_kompvter(call->get_parameter_mode()),
			kompvter::VertexIndexMapper::format_none,
			call->get_parameter_first()[i],
			call->get_parameter_first()[i] + call->get_parameter_count()[i]);
	}
}

void rosalind::DrawProcessor::process_draw(const touchstone::call::MultiDrawElements* call)
{
	if (!match())
	{
		return;
	}

	for (GLsizei i = 0; i < call->get_parameter_drawcount(); ++i)
	{
		process_draw(
			gl_primitive_to_kompvter(call->get_parameter_mode()),
			gl_element_type_to_kompvter(call->get_parameter_type()),
			calculate_offset_start(call->get_parameter_indices()[i]),
			calculate_offset_stop(call->get_parameter_indices()[i], call->get_parameter_count()[i], call->get_parameter_type()));
	}
}

void rosalind::DrawProcessor::process_draw(
	int primitive_mode, int element_type,
	std::size_t offset_start, std::size_t offset_stop)
{
	kompvter::DrawEvent draw_event;

	auto& vertex_array = this->context->get_current_resource<touchstone::VertexArrayResource>();
	auto& program = this->context->get_current_resource<touchstone::ProgramResource>();

	if (vertex_array.has_element_array_buffer())
	{
		auto name = vertex_array.get_element_array_buffer();
		auto buffer = this->buffer_manager->get_buffer(name);
		draw_event.set_vertex_index_mapper(kompvter::VertexIndexMapper(buffer, element_type));
	}

	kompvter::DrawParameters draw_parameters(primitive_mode, offset_start, offset_stop);
	draw_event.set_draw_parameters(draw_parameters);

	kompvter::VertexAttributes vertex_attributes;
	program.for_each_attrib(
		[this, &vertex_array, &vertex_attributes](const std::string& name, GLuint index)
		{
			if (!vertex_array.has_vertex_attribute(index) ||
				!vertex_array.is_vertex_attribute_enabled(index))
			{
				return;
			}

			auto attribute = vertex_array.get_vertex_attribute(index);
			if (attribute.buffer == 0 ||
				!this->buffer_manager->has_buffer(attribute.buffer))
			{
				return;
			}

			int format;
			if (!gl_vertex_attrib_type_to_kompvter(attribute.format, format))
			{
				return;
			}

			kompvter::VertexFetcher fetcher(
				this->buffer_manager->get_buffer(attribute.buffer),
				attribute.num_components,
				format, attribute.normalized,
				attribute.stride, attribute.offset);
			vertex_attributes.add(name, fetcher);
		});
	draw_event.set_vertex_attributes(vertex_attributes);

	if (program.has_uniform_block_binding("ViewTransforms"))
	{
		auto index = program.get_uniform_block_binding("ViewTransforms");
		auto name = this->context->get_current_resource<touchstone::BufferResource>(GL_UNIFORM_BUFFER, index).get_name();

		if (this->buffer_manager->has_buffer(name))
		{
			auto buffer = this->buffer_manager->get_buffer(name);
			assert(buffer.get_data_size() == sizeof(re::ViewTransform));

			auto view_transform = (const re::ViewTransform*)buffer.get_data();
			draw_event.set_view_matrix(view_transform->uViewMatrix);
			draw_event.set_projection_matrix(view_transform->uProjectionMatrix);
		}
	}

	if (program.has_uniform("uModelMatrix"))
	{
		auto uniform = program.get_uniform("uModelMatrix");
		draw_event.set_model_matrix(uniform.values.at(0).matrix4);
	}

	on_draw(draw_event);
}

int rosalind::DrawProcessor::gl_primitive_to_kompvter(GLenum primitive)
{
	switch (primitive)
	{
		case GL_TRIANGLES:
			return kompvter::DrawParameters::primitive_triangles;
		case GL_TRIANGLE_FAN:
			return kompvter::DrawParameters::primitive_triangle_fan;
		case GL_TRIANGLE_STRIP:
			return kompvter::DrawParameters::primitive_triangle_strip;
		case GL_POINTS:
			return kompvter::DrawParameters::primitive_points;
		case GL_LINES:
			return kompvter::DrawParameters::primitive_lines;
		case GL_LINE_STRIP:
			return kompvter::DrawParameters::primitive_line_strip;
		case GL_LINE_LOOP:
			return kompvter::DrawParameters::primitive_line_loop;
		default:
			return kompvter::DrawParameters::primitive_none;
	}
}

int rosalind::DrawProcessor::gl_element_type_to_kompvter(GLenum element_type)
{
	switch (element_type)
	{
		case GL_UNSIGNED_BYTE:
			return kompvter::VertexIndexMapper::format_unsigned_byte;
		case GL_UNSIGNED_SHORT:
			return kompvter::VertexIndexMapper::format_unsigned_short;
		case GL_UNSIGNED_INT:
			return kompvter::VertexIndexMapper::format_unsigned_integer;
		default:
			assert(false);
			return kompvter::VertexIndexMapper::format_none;
	}
}

bool rosalind::DrawProcessor::gl_vertex_attrib_type_to_kompvter(
	GLenum type, int& format)
{
	switch (type)
	{
		case GL_BYTE:
			format = kompvter::VertexFetcher::format_byte;
			break;
		case GL_UNSIGNED_BYTE:
			format = kompvter::VertexFetcher::format_unsigned_byte;
			break;
		case GL_SHORT:
			format = kompvter::VertexFetcher::format_short;
			break;
		case GL_UNSIGNED_SHORT:
			format = kompvter::VertexFetcher::format_unsigned_short;
			break;
		case GL_INT:
			format = kompvter::VertexFetcher::format_integer;
			break;
		case GL_UNSIGNED_INT:
			format = kompvter::VertexFetcher::format_unsigned_integer;
			break;
		case GL_FLOAT:
			format = kompvter::VertexFetcher::format_float;
			break;
		case GL_HALF_FLOAT:
			format = kompvter::VertexFetcher::format_half_float;
			break;
		default:
			return false;
	}

	return true;
}

std::size_t rosalind::DrawProcessor::calculate_offset_start(const void* indices)
{
	return (std::size_t)indices;
}

std::size_t rosalind::DrawProcessor::calculate_offset_stop(
	const void* indices, GLsizei count, GLenum element_type)
{
	auto offset_start = (std::intptr_t)indices;
	switch (element_type)
	{
		case GL_UNSIGNED_BYTE:
			return offset_start + count;
		case GL_UNSIGNED_SHORT:
			return offset_start + count * 2;
		case GL_UNSIGNED_INT:
			return offset_start + count * 4;
		default:
			assert(false);
			return offset_start;
	}
}
